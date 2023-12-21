//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2023
// filename: modulator_test.cpp
// description:
// License: MIT
//

#include "modulator.h"
#include <cmath>

int main() {

  modulator uut;

  double sample_rate = 125e6 / (256 * 32);

  for (uint16_t idx = 0; idx < 8192; idx++) {
    // impulse
    int16_t audio = idx == 0 ? 32767 : 0;

    uint16_t magnitude;
    int16_t phase;
    int16_t i;
    int16_t q;
    uut.process_sample(LSB, audio, i, q, magnitude, phase, 0);
    printf("%i %i %i %i %i\n", audio, i, q, magnitude, phase);
  }

  for (uint16_t idx = 0; idx < 8192; idx++) {
    // one tone
    int16_t audio = 32768 * sin(2.0 * M_PI * idx * 0.7e3 / sample_rate);

    uint16_t magnitude;
    int16_t phase;
    int16_t i;
    int16_t q;
    uut.process_sample(LSB, audio, i, q, magnitude, phase, 0);
    printf("%i %i %i %i %i\n", audio, i, q, magnitude, phase);
  }

  for (uint16_t idx = 0; idx < 8192; idx++) {
    // two tone
    int16_t audio = 16383 * sin(2.0 * M_PI * idx * 0.7e3 / sample_rate);
    audio += 16383 * sin(2.0 * M_PI * idx * 1.9e3 / sample_rate);

    uint16_t magnitude;
    int16_t phase;
    int16_t i;
    int16_t q;
    uut.process_sample(LSB, audio, i, q, magnitude, phase, 0);
    printf("%i %i %i %i %i\n", audio, i, q, magnitude, phase);
  }
}
