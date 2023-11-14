#ifndef EQ_H
#define EQ_H
#include <stdint.h>
#include "fft.h"
#include "fixed.h"
#include <cmath>

class eq
{
  uint16_t pointer = 0u;
  uint8_t ping_pong = 0u;

  int32_t frequency_response[33];
  int32_t buffer[2][64];
  int32_t buffer_imag[64];
  int32_t output_buffer[32];
  int32_t overlap[32];
  int32_t window[64];

  public:
  eq()
  {
    fft_initialise();
    for (int i = 0; i < 64; i++) {
      float multiplier = 0.5 * (1 - cosf(2*M_PI*i/63));
      window[i] = float2fixed(multiplier);
    }
    for (int i = 0; i < 32; i++) {
      output_buffer[i] = 0;
      overlap[i] = 0;
    }
  }
  void set_eq(float band[]);
  void process_sample(int32_t & sample);

};

#endif
