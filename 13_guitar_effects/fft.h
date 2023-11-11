#ifndef FFT_H_
#define FFT_H_

void fft_initialise();
void fft(float reals[], float imaginaries[], unsigned m);
void ifft(float reals[], float imaginaries[], unsigned m);

#endif
