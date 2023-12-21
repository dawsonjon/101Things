//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: music.cpp
// description:
// License: MIT
//

#include "music.hpp"
#include <algorithm>

void music::play_note(PWMAudio audio_output, note_t note)
{

  const uint32_t frequency_steps =
    static_cast<uint32_t>(4294967296.0 * note.pitch_Hz / audio_sample_rate);
  const float amplitude = note.pitch_Hz==0?0:2047; //frequency of zero means silence

  uint32_t samples_left = note.duration_samples - 50;
  while(samples_left) 
  {
    uint32_t block_size = std::min(static_cast<uint32_t>(1024u), samples_left);
    for (uint16_t idx = 0; idx < block_size; idx++) {
      const float sample = sin_table[phase >> 22] * amplitude; // 10 MSBs (32-10 = 22)
      phase += frequency_steps;
      samples[ping][idx] = sample + 2048;
    }
    audio_output.output_samples(samples[ping], block_size);
    ping ^= 1u;
    samples_left -= block_size;
  }

  //small gap between notes
  for (uint16_t idx = 0; idx < 50; idx++) {
    samples[ping][idx] = 2048;
  }
  audio_output.output_samples(samples[ping], 50);
  ping ^= 1u;

}

void music::play_tune(PWMAudio audio_output, const note_t notes[], uint16_t length)
{
  for(uint16_t i=0; i<length; i++)
  {
    play_note(audio_output, notes[i]);
  }
  //silence
  for (uint16_t idx = 0; idx < 50; idx++) {
    samples[ping][idx] = 0;
  }
  audio_output.output_samples(samples[ping], 50);
}
