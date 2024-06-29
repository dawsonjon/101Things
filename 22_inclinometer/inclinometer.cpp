//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2024
// filename: inclinometer.cpp
// description: FWD Inclinometer
// License: MIT
//

#include <stdio.h>
#include <cmath>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/spi.h>
#include <hardware/i2c.h>
#include <hardware/flash.h>
#include "st7735.h"
#include "mpu6050.h"

#include "font.h"
#include "s1_side.h"
#include "s1_back.h"

uint16_t RGB565( uint8_t R, uint8_t G, uint8_t B )
{
  //Create a 16 bit rgb value from the given R,G,B from 0-255.
  uint16_t color = ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | B >> 3;

  //swap bytes
  return color >> 8 | color << 8;

}


class frame_buffer
{

  public:


  void image(const uint16_t pixels[], uint16_t x, uint16_t y, uint16_t w, uint16_t h)
  {
    uint32_t i=0;
    for(uint32_t k=0; k<h; ++k)
    {
      for(uint32_t j=0; j<w; ++j)
      {
        buffer[((y+k)*width)+(x+j)] = pixels[i++];
      }
    }
  }

  void image_rotate(const uint16_t pixels[], float angle)
  {

    //assumes that image is the same size as the frame buffer
    //rotate around centre of display
    uint16_t cx = width>>1;
    uint16_t cy = height>>1;
    int32_t s = sinf(-angle * M_PI/180) * 1024.0; //10 fraction bits
    int32_t c = cosf(-angle * M_PI/180) * 1024.0; //10 fraction bits

    for(uint32_t y=0; y<height; ++y)
    {
      for(uint32_t x=0; x<width; ++x)
      {

        //rotate x, y coordinate to work out where it should be coppied from
        int32_t xm = x-cx;
        int32_t ym = y-cy;
        int32_t xfrom = (((xm * c) - (ym * s)) >> 10) + cx;
        int32_t yfrom = (((xm * s) + (ym * c)) >> 10) + cy;

        if( xfrom < width && yfrom < height && xfrom > 0 && yfrom > 0)
        {
          buffer[y*width+x] = pixels[yfrom*width + xfrom];
        }
        else
        {
          buffer[y*width+x] = RGB565(255, 255, 255); //fill missing pixels in white
        }

      }
    }
  }

  void fill(uint16_t color, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
  {
    uint32_t i=0;
    for(uint32_t j=0; j<w; ++j)
    {
      for(uint32_t k=0; k<h; ++k)
      {
        buffer[((y+k)*width)+(x+j)] = color;
      }
    }
  }

  void text(const char string[], const uint16_t x, const uint16_t y, const uint16_t color, const uint8_t font[])
  {
    uint8_t font_width = font[0];
    uint8_t font_height = font[1];
    uint8_t first_char = font[2];
    uint8_t num_chars = font[3];

    uint16_t idx=0;
    while(string[idx])
    {
      character((uint8_t)(string[idx]), x+(idx*font_width), y, color, font);
      ++idx;
    }
  }

  void character(const uint8_t c, const uint16_t x, const uint16_t y, const uint16_t color, const uint8_t font[])
  {
    uint8_t font_width = font[0];
    uint8_t font_height = font[1];
    uint8_t first_char = font[2];
    uint8_t num_chars = font[3];

    uint16_t idx = 4 + ((c-first_char)*font_width*font_height/8);
    uint8_t data = font[idx++];
    uint8_t bits_left = 8;    

    for(uint8_t k = 0; k<font_height; ++k)
    {
      for(uint8_t j = 0; j<font_width; ++j)
      {
        if(data & 0x80){
          buffer[((y+k)*width)+(x+j)] = color;
        }
        data <<= 1;
        bits_left--;
        if(bits_left == 0)
        {
          data = font[idx++];
          bits_left = 8;
        }
      }
    }
  }

  uint8_t * get_buffer()
  {
    return (uint8_t*) buffer;
  }

  private:
  static const uint8_t width = 160;
  static const uint8_t height = 128;
  uint16_t buffer[width * height];

};

// map constant onto a sector of flash 
static const uint8_t __in_flash() __attribute__((aligned(FLASH_SECTOR_SIZE))) 
flash_sector[FLASH_SECTOR_SIZE] = {0xff}; 

float pitch_offset = 0.0f;
float roll_offset = 0.0f;

void save_setup_data()
{
    // store SSID for next time
    uint8_t sector_to_write[FLASH_SECTOR_SIZE] = {0};
    float * sector_as_floats = (float *)sector_to_write;
    sector_as_floats[0] = pitch_offset;
    sector_as_floats[1] = roll_offset;

    const uint32_t address = (uint32_t) & (flash_sector[0]);
    const uint32_t flash_address = address - XIP_BASE;

    //!!! PICO is **very** fussy about flash erasing, there must be no code
    //! running in flash.  !!!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    const uint32_t ints =
        save_and_disable_interrupts(); // disable all interrupts

    // safe to erase flash here
    //--------------------------------------------------------------------------------------------
    flash_range_erase(flash_address, FLASH_SECTOR_SIZE);
    flash_range_program(flash_address, sector_to_write, FLASH_SECTOR_SIZE);
    //--------------------------------------------------------------------------------------------

    restore_interrupts(ints); // restore interrupts
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //!!! Normal operation resumed
}

void get_setup_data()
{
  volatile uint8_t *flash_sector_address = (uint8_t *)&flash_sector[0]; 
  volatile float *sector_as_floats = (float *)flash_sector_address;

  // settings stored
  if (flash_sector_address[0] != 0xff) {
    printf("Retrieving stored settings...\n");
    pitch_offset = sector_as_floats[0];
    roll_offset = sector_as_floats[1];
    printf("pitch offset: %3.0f roll offset: %3.0f\r\n", pitch_offset, roll_offset);
  }

  // settings not stored - assume zero
  else {
    printf("No settings stored, use defaults\n");
    pitch_offset = 0.0f;
    roll_offset = 0.0f;
    save_setup_data();
  }

}

void setup_menu(float pitch, float roll)
{

  while(1)
  {
    printf("Menu\n");
    printf(" p - enter pitch offset\n");
    printf(" r - enter roll offset\n");
    printf(" a - auto offset\n");
    printf(" x - save and exit menu\n");

    char command = getchar();
    if (command == 'p') {
      printf("pitch offfset degrees: \r\n");
      char line [256];
      float data;
      while(1)
      {
        fgets(line, 256, stdin);
        if (sscanf(line, "%f", &data) == 1) break;
      }
      pitch_offset = data;
      printf("pitch offset: %3.0f roll offset: %3.0f\r\n", pitch_offset, roll_offset);
    } else if (command == 'r') {
      printf("roll offset degrees: \n");
      char line [256];
      float data;
      while(1)
      {
        fgets(line, 256, stdin);
        if (sscanf(line, "%f", &data) == 1) break;
      }
      roll_offset = data;
      printf("pitch offset: %3.0f roll offset: %3.0f\r\n", pitch_offset, roll_offset);
    } else if (command == 'a') {
      printf("auto_offset\r\n");
      pitch_offset = -pitch;
      roll_offset = -roll;
      printf("pitch offset: %3.0f roll offset: %3.0f\r\n", pitch_offset, roll_offset);
    } else if (command == 'x') {
      save_setup_data();
      break;
    }
  }

}

int main() {
  stdio_init_all();
  stdio_set_translate_crlf(&stdio_usb, false);

  printf("initializing display...");
  st7735 display1(spi1, 12, 10, 11, 7, 6, 5);
  st7735 display2(spi1, 12, 10, 11, 7, 6, 4);
  display1.reset();
  display2.reset();
  display1.initr();
  display2.initr();
  display1.set_rotation(3);
  display2.set_rotation(3);
  display1.set_rgb(true);
  display2.set_rgb(true);
  printf("initialized\n");

  frame_buffer the_frame_buffer;

  printf("initializing accelerometer...");
  mpu6050 accelerometer(i2c0, 16, 17);
  printf("initialized\n");

  get_setup_data();
  
  while(1)
  {

    float pitch, roll;
    accelerometer.get_pitch_roll(pitch, roll);
    printf("pitch: %f roll: %f\n", pitch, roll);
    
    char angle_text[4];

    snprintf(angle_text, 4, "%3.0f", pitch+pitch_offset); 
    the_frame_buffer.image_rotate(s1_side, pitch+pitch_offset);
    the_frame_buffer.text(angle_text, 56, 96, RGB565(255, 70, 0), Grotesk16x32);
    display1.draw(the_frame_buffer.get_buffer(), 160*128*2);
    snprintf(angle_text, 4, "%3.0f", roll+roll_offset); 
    the_frame_buffer.image_rotate(s1_back, roll+roll_offset);
    the_frame_buffer.text(angle_text, 56, 96, RGB565(255, 70, 0), Grotesk16x32);
    display2.draw(the_frame_buffer.get_buffer(), 160*128*2);


    printf("press x to enter setup\n");

    char cmd = getchar_timeout_us(100000);
    if (cmd == 'x') setup_menu(pitch, roll);

  }


  printf("initialized\n");

}

