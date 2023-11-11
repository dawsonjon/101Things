#ifndef EQ_H
#define EQ_H
#include <stdint.h>
#include "fft.h"
#include <cmath>

class eq
{
  uint16_t pointer = 0u;
  uint8_t ping_pong = 0u;

  float frequency_response[33];
  float buffer[2][64];
  float buffer_imag[64];
  float output_buffer[32];
  float overlap[32];
  float window[64];

  public:
  eq()
  {
    fft_initialise();
    for (int i = 0; i < 64; i++) {
      float multiplier = 0.5 * (1 - cosf(2*M_PI*i/63));
      window[i] = multiplier;
    }
  }
  void set_eq(float band[]);
  void process_sample(float & sample);

};

#endif
