#include "fft.h"
#include "eq.h"
#include <cmath>
#include <cstdio>

void eq::set_eq(float band[])
{
  frequency_response[0]  = band[0];//dc
  frequency_response[1]  = band[1];
  frequency_response[2]  = band[1];
  frequency_response[3]  = band[2];
  frequency_response[4]  = band[2];
  frequency_response[5]  = band[2];
  frequency_response[6]  = band[2];
  frequency_response[7]  = band[3];
  frequency_response[8]  = band[3];
  frequency_response[9]  = band[3];
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
  frequency_response[32] = band[4];//fs/2
}

void window(float samples[])
{
  for (int i = 0; i < 64; i++) {
    float multiplier = 0.5 * (1 - cosf(2*M_PI*i/63));
    samples[i] = multiplier * samples[i];
  }
}

void eq::process_sample(float & sample)
{

  buffer[ping_pong][pointer+32] = sample;
  buffer[ping_pong^1][pointer] = sample;
  sample = output_buffer[pointer]; 
  ++pointer;

  if(pointer == 32u)
  {

    //zero imaginary part
    for(uint16_t i=0; i<64; ++i){
       buffer_imag[i] = 0.0f;
    }

    //window
    for (int i = 0; i < 64; i++) {
      buffer[ping_pong][i] *= window[i];
    }
    
    //FFT
    fft(buffer[ping_pong], buffer_imag, 6);

    //apply response
    for(uint16_t i=0; i<33; i++){
      buffer[ping_pong][i] *= frequency_response[i];
      buffer_imag[i] *= frequency_response[i];
    }
    for(uint16_t i=33; i<64; i++){
      buffer[ping_pong][i] *= frequency_response[64-i];
      buffer_imag[i] *= frequency_response[64-i];
    }

    //inverse FFT
    ifft(buffer[ping_pong], buffer_imag, 6);

    for(uint16_t i=0; i<32; ++i)
    {
       output_buffer[i] = buffer[ping_pong][i] + overlap[i];
       overlap[i] = buffer[ping_pong][i+32];
    }

    pointer = 0u;
    ping_pong ^= 1;

  }
}
