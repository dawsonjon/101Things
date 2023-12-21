//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2023
// filename: eq.cpp
// description:
// License: MIT
//

#include "eq.h"
#include "fft_64.h"
#include <cmath>
#include <cstdio>

int32_t eq_scale(int32_t a, int32_t b) {
  int32_t product = a * b;
  return (product + 2048) >> 12;
}

int32_t eq_const(float a) { return roundf(a * 4096); }

void eq::set_eq(int32_t band[]) {
  frequency_response[0] = band[0]; // dc
  frequency_response[1] = band[1];
  frequency_response[2] = band[1];
  frequency_response[3] = band[2];
  frequency_response[4] = band[2];
  frequency_response[5] = band[2];
  frequency_response[6] = band[2];
  frequency_response[7] = band[3];
  frequency_response[8] = band[3];
  frequency_response[9] = band[3];
  frequency_response[10] = band[3];
  frequency_response[11] = band[3];
  frequency_response[12] = band[3];
  frequency_response[13] = band[3];
  frequency_response[14] = band[3];
  frequency_response[15] = band[4];
  frequency_response[16] = band[4];
  frequency_response[17] = band[4];
  frequency_response[18] = band[4];
  frequency_response[19] = band[4];
  frequency_response[20] = band[4];
  frequency_response[21] = band[4];
  frequency_response[22] = band[4];
  frequency_response[23] = band[4];
  frequency_response[24] = band[4];
  frequency_response[25] = band[4];
  frequency_response[26] = band[4];
  frequency_response[27] = band[4];
  frequency_response[28] = band[4];
  frequency_response[29] = band[4];
  frequency_response[30] = band[4];
  frequency_response[31] = band[4];
  frequency_response[32] = band[4]; // fs/2
}

void eq::process_sample(int32_t &sample) {

  buffer[ping_pong][pointer + 32] = sample;
  buffer[ping_pong ^ 1][pointer] = sample;
  sample = output_buffer[pointer];
  ++pointer;

  if (pointer == 32u) {

    // zero imaginary part
    for (uint8_t i = 0; i < 64; ++i) {
      buffer_imag[i] = 0;
    }

    // input has 16 bits.
    // ADC has enob of 9.5 bits and 16x oversampling adds 2 bits giving about 12
    // bits containing useful information lower 4 bits are just noise, and can
    // be discarded without degrading SNR. This is achieved by scaling the
    // window by 1/16 and avoids the need for another multiplication.

    // window
    for (uint8_t i = 0; i < 64; i++) {
      buffer[ping_pong][i] = eq_scale(buffer[ping_pong][i], window[i]);
    }

    // The data now uses the 12 lower bits.
    // The 64 point FFT has a growth of 6 bits, the least significant 3 bits
    // contain only noise and are removed inside the FFT This leaves us with 18
    // bits of data, 15 of which are useful.

    // FFT
    fft_64(buffer[ping_pong], buffer_imag);

    // apply frequency response
    // has a maximum gain of 1/4 (-12dB) and a minimum of 1/64 (-24dB)
    // this removes 2 bits of growth and avoid the need for any scaling
    // after the FFT and the output will fit back into the original 16 bits.
    for (uint16_t i = 0; i < 33; i++) {
      buffer[ping_pong][i] =
          eq_scale(buffer[ping_pong][i], frequency_response[i]);
      buffer_imag[i] = eq_scale(buffer_imag[i], frequency_response[i]);
    }
    for (uint16_t i = 33; i < 64; i++) {
      buffer[ping_pong][i] =
          eq_scale(buffer[ping_pong][i], frequency_response[64 - i]);
      buffer_imag[i] = eq_scale(buffer_imag[i], frequency_response[64 - i]);
    }

    // inverse FFT (swap real and imaginary)
    fft_64(buffer_imag, buffer[ping_pong]);

    for (uint16_t i = 0; i < 32; ++i) {
      output_buffer[i] = (buffer[ping_pong][i] + overlap[i] + 8);
      overlap[i] = buffer[ping_pong][i + 32];
    }

    pointer = 0u;
    ping_pong ^= 1;
  }
}
