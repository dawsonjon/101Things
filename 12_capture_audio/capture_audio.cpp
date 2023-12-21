//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: capture_audio.cpp
// description:
// License: MIT
//

#include <stdio.h>

#include "ADCAudio.h"
#include "pico/stdlib.h"
#include "psu_mode.h"
#include <math.h>

int main() {
  stdio_init_all();
  stdio_set_translate_crlf(&stdio_usb, false);

  disable_power_save();

  ADCAudio audio_input(16, 20000 * 16);
  while (true) {
    uint16_t *samples;
    uint16_t new_samples[64];
    audio_input.input_samples(samples);
    for (uint16_t j = 0; j < 64; j++) {
      int16_t acc = 0;
      for (uint16_t i = 0; i < 16; i++) {
        acc += samples[j * 16 + i];
      }
      new_samples[j] = acc;
    }
    fwrite("AAAACCCC", 1, 8, stdout);
    fwrite(new_samples, 2, 64, stdout);
  }
}
