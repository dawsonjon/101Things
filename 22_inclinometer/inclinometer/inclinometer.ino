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

void setup() {

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
  accelerometer.zero_gyro();

  uint64_t time_us=time_us_64();
  float pitch_gyro = 0.0f;
  float roll_gyro = 0.0f;

  float pitch, roll, pitch_rate, roll_rate;
  while(1)
  {
    uint64_t delta_time_us;
    for(uint8_t idx=0; idx<100; ++idx)
    {
      accelerometer.get_pitch_roll(pitch, roll);
      accelerometer.get_pitch_roll_rate(pitch_rate, roll_rate);

      delta_time_us = time_us_64() - time_us;
      time_us += delta_time_us;

      pitch_gyro = (0.9999 * (pitch_gyro + (pitch_rate * delta_time_us / 1e6))) + (0.0001*pitch);
      roll_gyro  = (0.9999 * (roll_gyro + (roll_rate * delta_time_us / 1e6))) + (0.0001*roll);
    }

    printf("pitch: %+8.1f roll: %+8.1f pitch_rate %+8.1f roll_rate %+8.1f pitch_gyro %+8.1f roll_gyro %+8.1f delta t %+8.1f\n", pitch, roll, pitch_rate, roll_rate, pitch_gyro, roll_gyro, delta_time_us/1e6);
    char angle_text[4];
    snprintf(angle_text, 4, "%3.0f", pitch_gyro); 
    the_frame_buffer.image_rotate(s1_side, pitch_gyro);
    the_frame_buffer.text(angle_text, 56, 96, RGB565(255, 70, 0), Grotesk16x32);
    display1.draw(the_frame_buffer.get_buffer(), 160*128*2);

    snprintf(angle_text, 4, "%3.0f", roll_gyro); 
    the_frame_buffer.image_rotate(s1_back, roll_gyro);
    the_frame_buffer.text(angle_text, 56, 96, RGB565(255, 70, 0), Grotesk16x32);
    display2.draw(the_frame_buffer.get_buffer(), 160*128*2);
  }
}

void loop() {
    
}
