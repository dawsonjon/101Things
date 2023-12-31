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

#include "modulator.h"
#include "nco.h"

void transmitter_start(double frequency_Hz) {
  const bool enable_serial_data = true;
  const uint8_t rf_pin = 8;

  // Use PIO to output phase/frequency controlled oscillator
  nco rf_nco(rf_pin, frequency_Hz);

  // create modulator
  modulator audio_modulator;

  //scale FM deviation
  const double fm_deviation_Hz = 75e3; //2.5kHz
  const uint32_t fm_deviation_f15 =
      round(2 * 32768.0 * fm_deviation_Hz / rf_nco.get_sample_frequency_Hz(2));

  int16_t audio;
  int16_t phase;

  // use gpio for debug
  uint8_t debug_pin = 1;
  gpio_init(debug_pin);
  gpio_set_dir(debug_pin, GPIO_OUT);

  while (1)
  {
    // get a sample to transmit
    if(enable_serial_data) {
      audio = fgetc(stdin);
      audio <<= 8;
    }

    // demodulate
    gpio_put(debug_pin, 1);
    audio_modulator.process_sample(audio, phase, fm_deviation_f15);
    gpio_put(debug_pin, 0);

    // output phase
    rf_nco.output_sample(phase, 2);
  }
}

int main() {
  stdio_init_all();
  stdio_set_translate_crlf(&stdio_usb, false);

  printf("starting\n");
  transmitter_start(88e6/3);
}
