//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2023
// filename: guitar_effects.cpp
// description:
// License: MIT
//

#include <stdio.h>

#include "ADCAudio.h"
#include "PWMAudio.h"
#include "effects.h"
#include "guitar_effects_ui.h"
#include "pico/stdlib.h"
#include "psu_mode.h"
#include <math.h>

int main() {
  stdio_init_all();
  stdio_set_translate_crlf(&stdio_usb, false);

  guitar_effects_ui ui;
  ui.init();
  effects e;
  e.initialise();
  s_effect settings;

  // reduces noise in ADC measurements
  disable_power_save();

  const uint8_t oversample = 16;
  ADCAudio audio_input(16, oversample * 20000);
  PWMAudio audio_output(0, 20000);

  uint16_t *input_samples;
  uint8_t ping = 0;
  uint16_t output_samples[2][64];

  while (true) {

    // process a block of data
    for (uint8_t i = 0; i < 20; i++) {
      audio_input.input_samples(input_samples);

      uint16_t input_sample_number = 0;
      uint64_t start = time_us_64();
      for (uint16_t output_sample_number = 0; output_sample_number < 64;
           output_sample_number++) {
        // sum 16 samples to give 1
        int16_t sample = 0;
        for (uint8_t j = 0; j < oversample; j++) {
          sample += input_samples[input_sample_number++];
        }

        // convert to signed
        sample -= 32768;

        // apply effects
        e.process_sample(sample);

        // convert to unsigned 8-bit
        output_samples[ping][output_sample_number] = (sample + 32768) >> 4;
      }
      float elapsed = time_us_64() - start;
      // printf("time: %f us\n", elapsed/64);
      audio_output.output_samples(output_samples[ping], 64);
      ping ^= 1;
    }

    // interact with user
    ui.do_ui(e, settings);
  }
}
