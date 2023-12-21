//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: mic2speaker.cpp
// description:
// License: MIT
//

#include <stdio.h>

#include "ADCAudio.h"
#include "PWMAudio.h"
#include "pico/stdlib.h"
#include "psu_mode.h"

int main() {
  stdio_init_all();

  // reduces noise in ADC
  disable_power_save();

  ADCAudio audio_input(16, 10000);
  PWMAudio audio_output(0, 10000);
  while (true) {
    uint16_t *samples;
    audio_input.input_samples(samples);
    audio_output.output_samples(samples, 1024);
  }

  return 0;
}
