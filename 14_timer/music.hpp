#ifndef _MUSIC_H__
#define _MUSIC_H__

#include "PWMAudio.h"
#include <cmath>

struct note_t {
  uint16_t pitch_Hz;
  uint32_t duration_samples;
};

class music {
  uint16_t samples[2][1024];
  uint8_t ping = 0;
  uint32_t phase = 0;
  uint32_t audio_sample_rate;
  float sin_table[1024];

public:
  music(uint32_t fs) {
    audio_sample_rate = fs;
    for (uint16_t idx = 0; idx < 1024; idx++) {
      sin_table[idx] = sin(2.0 * M_PI * idx / 1024.0);
    }
  }

  void play_note(PWMAudio audio_output, note_t note);
  void play_tune(PWMAudio audio_output, const note_t notes[], uint16_t length);
};

#endif
