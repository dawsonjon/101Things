//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: effects.cpp
// description:
// License: MIT
//

#include "effects.h"
#include "fixed.h"
#include <cmath>
#include <cstdio>

const uint16_t audio_sample_rate_Hz = 20000;
const uint16_t samples_per_ms = audio_sample_rate_Hz / 1000;
FixedPoint sin_table[1024];

uint16_t frequency_Hz_to_steps(float frequency_Hz) {
  return static_cast<uint32_t>(65536 * frequency_Hz / audio_sample_rate_Hz);
}

// Low Frequency Oscillator Class
FixedPoint lfo ::get_sample(uint16_t frequency_steps, FixedPoint amplitude) {
  FixedPoint sample = sin_table[p >> 6] * amplitude; // 10 MSBs (16-10 = 4)
  p += frequency_steps;
  return sample;
}

// Delay Line Class
void delay_line ::input_sample(FixedPoint sample) {
  buffer[input_pointer] = sample;
  input_pointer = (input_pointer + 1) & 0xfff;
}

FixedPoint delay_line ::tap(uint16_t delay) {
  return buffer[(input_pointer - delay + 1) & 0xfff];
}

void effects ::update_settings(s_effect &settings) {
  m_settings = settings;
  eq1.set_eq(m_settings.eq_gains);
}

// Effects Class
void effects ::process_sample(int16_t &s) {
  FixedPoint sample = s;

  // DC removal
  dc = dc + ((sample - dc) >> 1);
  sample = sample - dc;

  // Preamplifier
  sample = sample * m_settings.pre_gain;
  
  // Graphic Equalizer
  int32_t raw_sample = sample.get();
  eq1.process_sample(raw_sample);
  sample.set(raw_sample);

  // Distortion
  FixedPoint magnitude;
  bool positive;
  if (m_settings.distortion_effect != DISTORTION_OFF) {
    sample = sample * m_settings.distortion_gain;
    sample = sample + m_settings.distortion_offset;
    positive = sample >= 0;
    magnitude = positive ? sample : -sample;
  }

  switch (m_settings.distortion_effect) {

  case CUBIC:
    if (sample > FixedPoint::from_float(1.0f)) {
      sample = FixedPoint::from_float(2.0f / 3.0f);
    } else if (sample < FixedPoint::from_float(-1.0f)) {
      sample = FixedPoint::from_float(-2.0f / 3.0f);
    } else {
      // x - x^3/3
      sample = sample - (sample * sample * sample),
      FixedPoint::from_float(1.0f / 3.0f);
    }

    sample = sample * FixedPoint::from_float(1.5f);
    break;

  case QUADRATIC:
    if (sample > FixedPoint::from_float(2.0f / 3.0f)) {
      sample = FixedPoint::from_float(1.0f);
    } else if (sample < FixedPoint::from_float(-2.0f / 3.0f)) {
      sample = FixedPoint::from_float(-1.0f);
    } else if (sample > FixedPoint::from_float(1.0f / 3.0f)) {
      //-3x^2 + 4x - 1/3
      sample = (FixedPoint::from_float(-3.0f) * sample * sample) +
               (FixedPoint::from_float(4.0f) * sample) -
               FixedPoint::from_float(1.0f / 3.0f);
    } else if (sample < -FixedPoint::from_float(1.0f / 3.0f)) {
      // 3x^2 + 4x + 1/3
      sample = (FixedPoint::from_float(3.0f) * sample * sample) +
               (FixedPoint::from_float(4.0f) * sample) +
               FixedPoint::from_float(1.0f / 3.0f);
    } else {
      sample = sample * FixedPoint::from_float(2.0f);
    }
    break;

  case FULL_WAVE:
    sample = magnitude;
    break;

  case HALF_WAVE:
    sample = positive ? magnitude : FixedPoint::from_float(0.0f);
    break;

  case FOLDBACK:
    //-4x^2 + 4x
    magnitude = (FixedPoint::from_float(-4.0f) * magnitude * magnitude) +
                (FixedPoint::from_float(4.0f) * magnitude);
    sample = positive ? magnitude : -magnitude;
    break;

  case FUZZ:
    if(magnitude < FixedPoint::from_float(0.5f))
    {
      magnitude = FixedPoint::from_float(2.0f) * magnitude;
    }
    else
    {
      magnitude = FixedPoint::from_float(1.0f);
    }
    sample = positive ? magnitude : -magnitude;
    break;

  }

  // delay
  const uint8_t num_delays = 4;
  FixedPoint delays[num_delays] = {0};
  FixedPoint mixed[num_delays] = {0};
  FixedPoint sum = 0;

  switch (m_settings.delay_effect) {
  case DELAY:
    delay_line1.input_sample(sample);
    sample =
        sample + delay_line1.tap(m_settings.delay_delay_ms * samples_per_ms) *
                     m_settings.delay_mix;
    break;

  case ECHO:
    delays[0] = delay_line1.tap(m_settings.delay_delay_ms * samples_per_ms);
    mixed[0] = sample + delays[0] * m_settings.delay_feedback;
    delay_line1.input_sample(mixed[0]);
    sample = sample + (delays[0] * m_settings.delay_mix);
    break;

  case REVERB:

    delays[0] = delay_line1.tap(150 * samples_per_ms);
    delays[1] = delay_line2.tap(160 * samples_per_ms);
    delays[2] = delay_line3.tap(170 * samples_per_ms);
    delays[3] = delay_line4.tap(180 * samples_per_ms);

    for (int i = 0; i < num_delays; ++i) {
      mixed[i] = delays[i];
      sum = sum + mixed[i];
    }
    sum = sum * FixedPoint::from_float(-0.5); //-2.0/num_delays
    for (int i = 0; i < num_delays; ++i) {
      mixed[i] = mixed[i] + sum;
    }
    for (int i = 0; i < num_delays; ++i) {
      mixed[i] = sample + mixed[i] * m_settings.delay_feedback;
    }

    delay_line1.input_sample(mixed[0]);
    delay_line2.input_sample(mixed[1]);
    delay_line3.input_sample(mixed[2]);
    delay_line4.input_sample(mixed[3]);

    sample = sample + (delays[0] * m_settings.delay_mix);

    break;
  }

  // Modulation Effects
  FixedPoint modulation;
  switch (m_settings.modulator_effect) {
  case VIBRATO:
    modulator_delay_line.input_sample(sample);
    modulation = lfo1.get_sample(m_settings.vibrato_rate_steps,
                                 m_settings.vibrato_depth_ms *
                                     FixedPoint::from_float(samples_per_ms));
    modulation = modulation + (m_settings.vibrato_depth_ms *
                               FixedPoint::from_float(samples_per_ms * 2));
    sample = modulator_delay_line.tap(modulation.to_int());
    break;

  case TREMOLO:
    modulation = lfo1.get_sample(m_settings.tremolo_rate_steps,
                                 FixedPoint::from_float(0.5)) +
                 FixedPoint::from_float(1.0);
    modulation = (modulation * m_settings.tremolo_depth) +
                 FixedPoint::from_float(1.0) - m_settings.tremolo_depth;
    sample = sample * modulation;
    break;

  case CHORUS:
    modulator_delay_line.input_sample(sample);
    modulation = lfo1.get_sample(m_settings.chorus1_rate_steps,
                                 m_settings.chorus1_depth_ms *
                                     FixedPoint::from_float(samples_per_ms));
    sample =
        sample +
        (modulator_delay_line.tap(m_settings.chorus1_delay_ms * samples_per_ms +
                                  modulation.to_int()) *
         m_settings.chorus1_feedback);
    modulation = lfo1.get_sample(m_settings.chorus2_rate_steps,
                                 m_settings.chorus2_depth_ms *
                                     FixedPoint::from_float(samples_per_ms));
    sample =
        sample +
        (modulator_delay_line.tap(m_settings.chorus2_delay_ms * samples_per_ms +
                                  modulation.to_int()) *
         m_settings.chorus2_feedback);
    modulation = lfo1.get_sample(m_settings.chorus3_rate_steps,
                                 m_settings.chorus3_depth_ms *
                                     FixedPoint::from_float(samples_per_ms));
    sample =
        sample +
        (modulator_delay_line.tap(m_settings.chorus3_delay_ms * samples_per_ms +
                                  modulation.to_int()) *
         m_settings.chorus3_feedback);
    break;

  case FLANGER:
    modulator_delay_line.input_sample(sample);
    modulation = lfo1.get_sample(m_settings.flanger_rate_steps,
                                 m_settings.flanger_depth_ms *
                                     FixedPoint::from_float(samples_per_ms));
    sample =
        sample +
        (modulator_delay_line.tap(m_settings.flanger_delay_ms * samples_per_ms +
                                  modulation.to_int()) *
         m_settings.flanger_feedback);
    break;

  case PHASER:
    modulation =
        lfo1.get_sample(m_settings.phaser_rate_steps, m_settings.phaser_depth);
    sample = sample + all_pass1.process_sample(sample, modulation);
    break;
  }

  // hard clipping
  raw_sample = sample.get();
  if (sample > 32767) {
    s = 32767;
  } else if (sample < -32767) {
    s = -32767;
  } else {
    s = raw_sample;
  }
}

void effects ::initialise() {
  for (uint16_t idx = 0; idx < 1024; idx++) {
    sin_table[idx] = FixedPoint::from_float(sin(2.0 * M_PI * idx / 1024.0));
  }
}
