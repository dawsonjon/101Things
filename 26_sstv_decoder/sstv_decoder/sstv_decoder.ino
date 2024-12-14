#include "hardware/spi.h"
#include "ili934x.h"
#include "decode_sstv.h"
#include "ADCAudio.h"

#define PIN_MISO 12
#define PIN_CS   13
#define PIN_SCK  14
#define PIN_MOSI 15 
#define PIN_DC   11
#define PIN_RST  10
#define SPI_PORT spi1

ILI934X *display;

void configure_display()
{
  sleep_ms(1000);

  //spi_init(SPI_PORT, 62500000);
  spi_init(SPI_PORT, 40000000);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
  gpio_init(PIN_CS);
  gpio_set_dir(PIN_CS, GPIO_OUT);
  gpio_init(PIN_DC);
  gpio_set_dir(PIN_DC, GPIO_OUT);
  gpio_init(PIN_RST);
  gpio_set_dir(PIN_RST, GPIO_OUT);
  display = new ILI934X(SPI_PORT, PIN_CS, PIN_DC, PIN_RST, 320, 240, R0DEG);

  display->init();
  display->setRotation(R0DEG, false);
  display->powerOn(true);
  display->clear();

}

void setup() {
}


ADCAudio adc_audio;

void loop() {
  configure_display();

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
          if(pixel_y > last_pixel_y)
          {
            Serial.println(last_pixel_y);

            //convert from 24 bit to 16 bit colour
            uint16_t line_rgb565[320];
            for(uint16_t x=0; x<320; ++x)
            {
              line_rgb565[x] = display->colour565(line_rgb[x][0], line_rgb[x][1], line_rgb[x][2]);
            }

            //send line to display
            display->writeHLine(0, last_pixel_y, 320, line_rgb565);
          }
          last_pixel_y = pixel_y;
          

          if(pixel_x < 320 && pixel_y < 256 && pixel_colour < 3) {
            line_rgb[pixel_x][pixel_colour] = pixel;
          }
          
      }

    }
  }
  adc_audio.end();
}
