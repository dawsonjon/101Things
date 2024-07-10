//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2023
// filename: transmitter.cpp
// description: Ham Transmitter for Pi Pico
// License: MIT
//

#include "pico/stdlib.h"
#include <cmath>
#include <stdio.h>
#include <ctype.h>

#include "nco.h"

void frequency_generator(double frequency_Hz) {
  const uint8_t rf_pin = 8;

  // Use PIO to output phase/frequency controlled oscillator
  nco rf_nco(rf_pin, frequency_Hz);
  const double sample_frequency_Hz = 10e3;
  const uint8_t waveforms_per_sample =
      rf_nco.get_waveforms_per_sample(sample_frequency_Hz);

  while (1) {
    // output phase
    rf_nco.output_sample(0, waveforms_per_sample);
  }
}

int main()
{
  stdio_init_all();
  frequency_generator(10e6);
}
