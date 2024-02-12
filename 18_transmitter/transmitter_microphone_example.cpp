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

// Microphone example, transmit audio from microphone input

#include "pico/stdlib.h"
#include <cmath>
#include <stdio.h>
#include <ctype.h>

#include "transmit.h"
#include "psu_mode.h"

// example application
int main() {
  stdio_init_all();
  disable_power_save();

  // Hard code frequency and mode
  uint32_t frequency = 14.175e6;
  tx_mode_t mode = USB;


  // start transmitter using microphone
  transmit(mode, frequency);
}
