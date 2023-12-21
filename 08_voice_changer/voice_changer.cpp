//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2023
// filename: voice_changer.cpp
// description:
// License: MIT
//

#include <cmath>
#include <stdio.h>

#include "ADCAudio.h"
#include "PWMAudio.h"
#include "half_band_filter2.h"
#include "pico/stdlib.h"
#include "psu_mode.h"

int main() {
  stdio_init_all();

  disable_power_save();

  uint16_t audio_sample_rate_Hz = 10000;
  double frequency_shift_Hz = 900;

  ADCAudio audio_input(16, audio_sample_rate_Hz);
  PWMAudio audio_output(0, audio_sample_rate_Hz);
  half_band_filter2 half_band_filter;
  uint32_t downshift_phase = 0;
  uint32_t upshift_phase = 0;
  uint32_t frequency_shift_steps = static_cast<uint32_t>(
      4294967296.0 * frequency_shift_Hz / audio_sample_rate_Hz);
  uint32_t downshift_frequency = -(1u << 30u) + frequency_shift_steps;
  uint32_t upshift_frequency = (1u << 30u); // FS/4
  int32_t dc = 0;
  int32_t accumulator = 0;

  // pre-generate sin/cos lookup tables
  int16_t sin_table[1024];
  float scaling_factor = (1 << 15) - 1;
  for (uint16_t idx = 0; idx < 1024; idx++) {
    sin_table[idx] = sin(2.0 * M_PI * idx / 1024.0) * scaling_factor;
  }

  while (true) {
    uint16_t *samples;
    audio_input.input_samples(samples);

    // apply frequency shift to signal
    for (uint16_t idx = 0; idx < 1024; ++idx) {

      // wanted signal 0 to fs/2
      int16_t i = samples[idx] - dc; // convert to signed representation
      accumulator += i;
      int16_t q = 0;

      // shift down by fs/4 + offset
      // wanted signal -fs/4 to fs/4
      uint32_t phase_MSB =
          downshift_phase >> 22; // keep 10 MSBs of phase 32-10 = 22
      downshift_phase += downshift_frequency;
      int16_t rotation_i = sin_table[(phase_MSB + 256) & 0x3ff];
      int16_t rotation_q = -sin_table[phase_MSB];
      int16_t i_shifted =
          (((int32_t)i * rotation_i) - ((int32_t)q * rotation_q)) >> 15;
      int16_t q_shifted =
          (((int32_t)q * rotation_i) + ((int32_t)i * rotation_q)) >> 15;

      // filter -fs/4 to fs/4
      half_band_filter.filter(i_shifted, q_shifted);

      // shift up by fs/4
      // wanted signal 0 to fs/2
      phase_MSB = upshift_phase >> 22;    // keep 10 MSBs of phase 32-10 = 22
      upshift_phase += upshift_frequency; // fs/4
      rotation_i = sin_table[(phase_MSB + 256) & 0x3ff];
      rotation_q = -sin_table[phase_MSB];
      const int16_t i_new = (((int32_t)i_shifted * rotation_i) -
                             ((int32_t)q_shifted * rotation_q)) >>
                            15;

      samples[idx] = i_new + 2048;
    }
    dc = accumulator / 1024;

    audio_output.output_samples(samples, 1024);
  }

  return 0;
}
