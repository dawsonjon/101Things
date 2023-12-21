//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: test_harness.cpp
// description:
// License: MIT
//

#include "effects.h"
#include "fixed.h"
#include <cstdint>
#include <cstdio>

int main() {

  FILE *inf = fopen("testfile", "r");
  FILE *outf = fopen("outfile", "w");
  effects e;
  e.initialise();
  s_effect settings;

  settings.pre_gain = FixedPoint::from_float(4.0);
  settings.eq_gains[0] = eq_const(1.0) / 16;
  settings.eq_gains[1] = eq_const(1.0) / 16;
  settings.eq_gains[2] = eq_const(1.0) / 16;
  settings.eq_gains[3] = eq_const(1.0) / 16;
  settings.eq_gains[4] = eq_const(1.0) / 16;
  settings.distortion_effect = DISTORTION_OFF;
  settings.distortion_offset = FixedPoint::from_float(0.0);
  settings.distortion_gain = FixedPoint::from_float(8.0);
  settings.delay_effect = DELAY_OFF;
  settings.delay_delay_ms = 200.0;
  settings.delay_feedback = FixedPoint::from_float(0.8);
  settings.delay_mix = FixedPoint::from_float(0.1);
  settings.modulator_effect = PHASER;
  settings.flanger_depth_ms = FixedPoint::from_float(1.0);
  settings.flanger_rate_steps = frequency_Hz_to_steps(1.0);
  settings.flanger_delay_ms = 200u;
  settings.flanger_feedback = FixedPoint::from_float(0.9);
  settings.chorus1_depth_ms = FixedPoint::from_float(1.0);
  settings.chorus1_rate_steps = frequency_Hz_to_steps(1.1);
  settings.chorus1_delay_ms = 20u;
  settings.chorus1_feedback = FixedPoint::from_float(0.9);
  settings.chorus2_depth_ms = FixedPoint::from_float(1.0);
  settings.chorus2_rate_steps = frequency_Hz_to_steps(1.2);
  settings.chorus2_delay_ms = 21u;
  settings.chorus2_feedback = FixedPoint::from_float(0.9);
  settings.chorus3_depth_ms = FixedPoint::from_float(1.0);
  settings.chorus3_rate_steps = frequency_Hz_to_steps(1.3);
  settings.chorus3_delay_ms = 22u;
  settings.chorus3_feedback = FixedPoint::from_float(0.9);
  settings.tremolo_rate_steps = frequency_Hz_to_steps(10.0);
  settings.vibrato_depth_ms = FixedPoint::from_float(1.0);
  settings.vibrato_rate_steps = frequency_Hz_to_steps(2.0);
  settings.phaser_rate_steps = frequency_Hz_to_steps(10.0f);
  settings.phaser_depth = FixedPoint::from_float(0.8f);
  e.update_settings(settings);

  while (!feof(inf)) {
    int16_t sample = fgetc(inf);
    sample |= fgetc(inf) << 8;
    sample -= 32768;

    e.process_sample(sample);

    fputc((sample & 0xff), outf);
    fputc((sample >> 8), outf);
  }
  fclose(inf);
  fclose(outf);

  return 0;
}
