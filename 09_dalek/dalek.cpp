#include <stdio.h>
#include <cmath>

#include "ADCAudio.h"
#include "PWMAudio.h"
#include "pico/stdlib.h"

int main() {
  stdio_init_all();

  uint16_t audio_sample_rate_Hz = 10000;

  ADCAudio audio_input(16, audio_sample_rate_Hz);
  PWMAudio audio_output(0, audio_sample_rate_Hz);

  uint32_t phase=0;
  uint32_t frequency = static_cast<uint32_t>(4294967296.0 * 30.0/audio_sample_rate_Hz);
  int32_t dc = 0;
  int32_t accumulator = 0;

  //pre-generate sin/cos lookup tables
  int16_t sin_table[1024];
  float scaling_factor = (1 << 15) - 1;
  for(uint16_t idx=0; idx<1024; idx++)
  {
    sin_table[idx] = sin(2.0*M_PI*idx/1024.0) * scaling_factor;
  }


  while (true) {
    uint16_t *samples;
    audio_input.input_samples(samples);

    //apply frequency shift to signal
    for(uint16_t idx=0; idx<1024; ++idx)
    {

      //wanted signal 0 to fs/2
      int16_t i = samples[idx] - dc; //convert to signed representation
      accumulator += i;

      //apply modulator
      uint32_t phase_MSB = phase >> 22; //keep 10 MSBs of phase 32-10 = 22
      phase += frequency;
      int16_t i_new = ((int32_t)i * sin_table[phase_MSB]) >> 15;

      //convert to unsigned
      samples[idx] = i_new + 2048;

    }
    dc = accumulator/1024;

    audio_output.output_samples(samples, 1024);
  }

  return 0;
}
