#include <stdio.h>
#include <cmath>

#include "ADCAudio.h"
#include "pico/stdlib.h"
#include "psu_mode.h"


unsigned bit_reverse(unsigned forward, unsigned m){
    unsigned reversed=0;
    unsigned i;
    for(i=0; i<m; i++){
        reversed <<= 1;
        reversed |= forward & 1;
        forward >>= 1;
    }
    return reversed;
}

void fft(float reals[], float imaginaries[], unsigned m){

    int16_t stage, subdft_size, span, j, i, ip;
    float temp_real, temp_imaginary, imaginary_twiddle, real_twiddle;
    const unsigned n = 1<<m;

    //bit reverse data
    for(i=0; i<n; i++){
        ip = bit_reverse(i, m);
        if(i < ip){
            temp_real = reals[i];
            temp_imaginary = imaginaries[i];
            reals[i] = reals[ip];
            imaginaries[i] = imaginaries[ip];
            reals[ip] = temp_real;
            imaginaries[ip] = temp_imaginary;
        }
    }

    //butterfly multiplies
    for(stage=0; stage<m; stage++){
        subdft_size = 2 << stage;
        span = subdft_size >> 1;

        for(j=0; j<span; j++){
            for(i=j; i<n; i+=subdft_size){
                ip=i+span;

                real_twiddle=cosf((float)j*M_PI/span);
                imaginary_twiddle=-sinf((float)j*M_PI/span);

                temp_real      = (reals[ip]*real_twiddle)      - (imaginaries[ip]*imaginary_twiddle);
                temp_imaginary = (reals[ip]*imaginary_twiddle) + (imaginaries[ip]*real_twiddle);

                reals[ip]       = reals[i]-temp_real;
                imaginaries[ip] = imaginaries[i]-temp_imaginary;

                reals[i]       = reals[i]+temp_real;
                imaginaries[i] = imaginaries[i]+temp_imaginary;

            }
        }

    }
}

float interpolate(float real[], float imag[], uint16_t f)
{
    float yp1 = sqrt((real[f+1] * real[f+1]) + (imag[f+1] * imag[f+1]));
    float y0 = sqrt((real[f] * real[f]) + (imag[f] * imag[f]));
    float ym1 = sqrt((real[f-1] * real[f-1]) + (imag[f-1] * imag[f-1]));
    float p = (yp1 - ym1)/(2.0f*(2.0f*y0 - yp1 - ym1));
    return f+p;
}

int main() {
  stdio_init_all();
  disable_power_save();

  uint16_t audio_sample_rate_Hz = 10000;

  ADCAudio audio_input(16, audio_sample_rate_Hz);
  int32_t dc = 0;
  int32_t accumulator = 0;

  while (true) {
    uint16_t *samples;
    float real[1024];
    float imag[1024];
    audio_input.input_samples(samples);

    for(uint16_t idx=0; idx<1024; ++idx)
    {

      real[idx] = samples[idx] - dc; //convert to signed representation
      imag[idx] = 0.0f;
      accumulator += real[idx];

    }
    dc = accumulator/1024;

    //window
    for(uint16_t idx=0; idx<1024; idx++)
    {
      float window = 0.54f - 0.46f*cosf(2.0f*M_PI*idx/1024.0f);
      real[idx] = real[idx]*window;
      imag[idx] = imag[idx]*window;
    }

    fft(real, imag, 10); //2^10 = 1024 points

    float maxval = 0;
    float maxfreq = 0;
    for(uint16_t idx=7; idx<40; ++idx)
    {
      float magnitude = sqrt(real[idx] * real[idx]) + (imag[idx] * imag[idx]);
      float f1 = interpolate(real, imag, idx);
      for(uint16_t harmonic=0; harmonic < 2; ++harmonic)
      {
        uint16_t harmonic_frequency = floor((harmonic+2) * f1);
        if(harmonic_frequency < 512)
        {
          magnitude *= sqrt((real[harmonic_frequency] * real[harmonic_frequency]) + (imag[harmonic_frequency] * imag[harmonic_frequency]));
        }
      }
      
      if (magnitude > maxval)
      {
        maxfreq = f1;
        maxval = magnitude;
      }

      printf("%f ", magnitude);
      
    }
    printf("\n");

    float frequency_Hz = (float)audio_sample_rate_Hz * (maxfreq)/1024.0f;
    printf("%f, %.2fHz\n", maxfreq, frequency_Hz);

  }

  return 0;
}
