#include "fft.h"
#include <cmath>
#include <stdint.h>

static const uint16_t max_m = 10;//the largest size of FFT supported
static const uint16_t max_n_over_2 = 1<<(max_m-1);
static float cos_table[max_n_over_2];
static float sin_table[max_n_over_2];

void fft_initialise()
{
  for(int i=0; i<max_n_over_2; ++i)
  {
    cos_table[i]=cosf((float)i*M_PI/max_n_over_2);
    sin_table[i]=-sinf((float)i*M_PI/max_n_over_2);
  }
}

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

                real_twiddle=cos_table[j<<(max_m-stage-1)];
                imaginary_twiddle=-sin_table[j<<(max_m-stage-1)];

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

void ifft(float reals[], float imaginaries[], unsigned m){
  fft(imaginaries, reals, m);
  int16_t n = 1 << m;
  for(uint16_t idx=0u; idx<n; ++idx)
  {
    float fraction;
    int exponent;
    fraction = frexpf(imaginaries[idx], &exponent);
    imaginaries[idx] = ldexpf(fraction, exponent-m);
    fraction = frexpf(reals[idx], &exponent);
    reals[idx] = ldexpf(fraction, exponent-m);
  }
}

