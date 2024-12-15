//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: sstv_decoder.ino
// description:
//
// SSTV Decoder using pi-pico.
//
// Accepts audio on ADC input, and displays on an ILI943x display.
// Works with the Martin and Scottie modes.
//
// License: MIT
//

#include "hardware/spi.h"
#include "ili934x.h"
#include "font_8x5.h"
#include "font_16x12.h"
#include "decode_sstv.h"
#include "ADCAudio.h"
#include "splash.h"

//Pinout

#define PIN_MISO 12
#define PIN_CS   13
#define PIN_SCK  14
#define PIN_MOSI 15 
#define PIN_DC   11
#define SPI_PORT spi1

#define ROTATION R0DEG
//#define ROTATION R90DEG,
//#define ROTATION R180DEG,
//#define ROTATION R270DEG,
//#define ROTATION MIRRORED0DEG,
//#define ROTATION MIRRORED90DEG,
//#define ROTATION MIRRORED180DEG,
//#define ROTATION MIRRORED270DEG

#define INVERT_COLOURS false
//#define INVERT_COLOURS true

#define STRETCH true
//#define STRETCH false

ILI934X *display;

void setup() {
  configure_display();
  Serial.println("Pico SSTV Copyright (C) Jonathan P Dawson 2024");
  Serial.println("github: https://github.com/dawsonjon/101Things");
  Serial.println("docs: 101-things.readthedocs.io");
}


void loop() {
  ADCAudio adc_audio;
  adc_audio.begin(28, 15000);
  c_sstv_decoder sstv_decoder;
  int16_t dc;
  uint8_t line_rgb[320][3];
  uint16_t last_pixel_y=0;
  
  while(1)
  {
    uint16_t *samples;
    adc_audio.input_samples(samples);

    for(uint16_t idx=0; idx<1024; idx++)
    {
      dc = dc + (samples[idx] - dc)/2;
      int16_t sample = samples[idx] - dc;
      uint16_t pixel_y;
      uint16_t pixel_x;
      uint8_t pixel_colour;
      uint8_t pixel;
      int16_t frequency;
      const bool new_pixel = sstv_decoder.decode_audio(sample, pixel_y, pixel_x, pixel_colour, pixel, frequency);

      if(new_pixel)
      {
          s_sstv_mode mode;
          sstv_decoder.get_mode(mode);

          if(pixel_y > last_pixel_y)
          {


            //convert from 24 bit to 16 bit colour
            uint16_t line_rgb565[320];
            for(uint16_t x=0; x<320; ++x)
            {
              line_rgb565[x] = display->colour565(line_rgb[x][0], line_rgb[x][1], line_rgb[x][2]);
              line_rgb[x][0] = line_rgb[x][1] = line_rgb[x][2] = 0;
            }

            //send line to display
            display->writeHLine(0, last_pixel_y*240/256, 320, line_rgb565);

            //update progress


            display->fillRect(320-(21*6)-2, 240-10, 10, 21*6+2, COLOUR_BLACK);
            char buffer[21];
            if(mode.mode==martin)
            {
              snprintf(buffer, 21, "Martin: %ux%u/%u", mode.width, last_pixel_y+1, mode.height);
            }
            else
            {
              snprintf(buffer, 21, "Scottie: %ux%u/%u", mode.width, last_pixel_y+1, mode.height);
            }
            display->drawString(320-(21*6), 240-8, font_8x5, buffer, COLOUR_WHITE, COLOUR_BLACK);
            Serial.println(buffer);

          }
          last_pixel_y = pixel_y;
          

          if(pixel_x < 320 && pixel_y < 256 && pixel_colour < 3) {
            if(STRETCH && mode.width==160 && pixel_x < 160)
            {
              line_rgb[pixel_x*2][pixel_colour] = pixel;
              line_rgb[pixel_x*2+1][pixel_colour] = pixel;
            }
            else
            {
              line_rgb[pixel_x][pixel_colour] = pixel;
            }

          }
          
      }

    }
  }
  adc_audio.end();
}

void draw_splash_screen()
{
  uint32_t z=0;
  for(uint16_t y=0;y<240; ++y)
  {
    display->writeHLine(0, y, 320, &splash[z]);
    z+=320;
  }
}

void configure_display()
{
  spi_init(SPI_PORT, 62500000);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
  gpio_init(PIN_CS);
  gpio_set_dir(PIN_CS, GPIO_OUT);
  gpio_init(PIN_DC);
  gpio_set_dir(PIN_DC, GPIO_OUT);
  display = new ILI934X(SPI_PORT, PIN_CS, PIN_DC, 320, 240, R0DEG);
  display->setRotation(ROTATION, INVERT_COLOURS);
  display->init();
  display->powerOn(true);
  display->clear();
  draw_splash_screen();
}
