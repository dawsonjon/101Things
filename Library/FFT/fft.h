#ifndef FFT_H_
#define FFT_H_
#include <cstdint>

void fft_initialise();
void fixed_fft(int32_t reals[], int32_t imaginaries[], unsigned m);
void fixed_ifft(int32_t reals[], int32_t imaginaries[], unsigned m);

#endif
