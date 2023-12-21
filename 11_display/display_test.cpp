//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: display_test.cpp
// description:
// License: MIT
//

#include "pico/stdlib.h"
#include "ssd1306.h"
#include <stdio.h>

int main() {
  stdio_init_all();

  // configure display
  i2c_init(i2c1, 400000);
  const uint8_t PIN_DISPLAY_SDA = 18;
  const uint8_t PIN_DISPLAY_SCL = 19;
  gpio_set_function(PIN_DISPLAY_SDA, GPIO_FUNC_I2C);
  gpio_set_function(PIN_DISPLAY_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(PIN_DISPLAY_SDA);
  gpio_pull_up(PIN_DISPLAY_SCL);
  ssd1306_t disp;
  disp.external_vcc = false;
  ssd1306_init(&disp, 128, 64, 0x3C, i2c1);

  const uint8_t PIN_A = 22;
  const uint8_t PIN_B = 21;
  const uint8_t PIN_C = 20;
  const uint8_t PIN_D = 17;
  gpio_init(PIN_A);
  gpio_set_dir(PIN_A, GPIO_IN);
  gpio_pull_up(PIN_A);
  gpio_init(PIN_B);
  gpio_set_dir(PIN_B, GPIO_IN);
  gpio_pull_up(PIN_B);
  gpio_init(PIN_C);
  gpio_set_dir(PIN_C, GPIO_IN);
  gpio_pull_up(PIN_C);
  gpio_init(PIN_D);
  gpio_set_dir(PIN_D, GPIO_IN);
  gpio_pull_up(PIN_D);

  while (1) {
    if (!gpio_get(PIN_A)) {
      ssd1306_clear(&disp);
      ssd1306_draw_string(&disp, 0, 0, 1, "BUTTON A");
      ssd1306_show(&disp);
    }
    if (!gpio_get(PIN_B)) {
      ssd1306_clear(&disp);
      ssd1306_draw_string(&disp, 0, 0, 1, "BUTTON B");
      ssd1306_show(&disp);
    }
    if (!gpio_get(PIN_C)) {
      ssd1306_clear(&disp);
      ssd1306_draw_string(&disp, 0, 0, 1, "BUTTON C");
      ssd1306_show(&disp);
    }
    if (!gpio_get(PIN_D)) {
      ssd1306_clear(&disp);
      ssd1306_draw_string(&disp, 0, 0, 1, "BUTTON D");
      ssd1306_show(&disp);
    }
    sleep_ms(100);
  }
}
