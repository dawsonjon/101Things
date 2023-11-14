#include "fft.h"
#include "eq.h"
#include <cmath>
#include <cstdio>

void eq::set_eq(float band[])
{
  frequency_response[0]  = float2fixed(band[0]);//dc
  frequency_response[1]  = float2fixed(band[1]);
  frequency_response[2]  = float2fixed(band[1]);
  frequency_response[3]  = float2fixed(band[2]);
  frequency_response[4]  = float2fixed(band[2]);
  frequency_response[5]  = float2fixed(band[2]);
  frequency_response[6]  = float2fixed(band[2]);
  frequency_response[7]  = float2fixed(band[3]);
  frequency_response[8]  = float2fixed(band[3]);
  frequency_response[9]  = float2fixed(band[3]);
  frequency_response[10] = float2fixed(band[3]);
  frequency_response[11] = float2fixed(band[3]);
  frequency_response[12] = float2fixed(band[3]);
  frequency_response[13] = float2fixed(band[3]);
  frequency_response[14] = float2fixed(band[3]);
  frequency_response[15] = float2fixed(band[4]);
  frequency_response[16] = float2fixed(band[4]);
  frequency_response[17] = float2fixed(band[4]);
  frequency_response[18] = float2fixed(band[4]);
  frequency_response[19] = float2fixed(band[4]);
  frequency_response[20] = float2fixed(band[4]);
  frequency_response[21] = float2fixed(band[4]);
  frequency_response[22] = float2fixed(band[4]);
  frequency_response[23] = float2fixed(band[4]);
  frequency_response[24] = float2fixed(band[4]);
  frequency_response[25] = float2fixed(band[4]);
  frequency_response[26] = float2fixed(band[4]);
  frequency_response[27] = float2fixed(band[4]);
  frequency_response[28] = float2fixed(band[4]);
  frequency_response[29] = float2fixed(band[4]);
  frequency_response[30] = float2fixed(band[4]);
  frequency_response[31] = float2fixed(band[4]);
  frequency_response[32] = float2fixed(band[4]);//fs/2
}

void eq::process_sample(int32_t & sample)
{

  buffer[ping_pong][pointer+32] = sample;
  buffer[ping_pong^1][pointer] = sample;
  int32_t output_sample = output_buffer[pointer]; 
  if(output_sample > float2fixed(1.0))
  {
    sample = float2fixed(1.0);
  } 
  else if(output_sample < -float2fixed(1.0))
  {
    sample = -float2fixed(1.0);
  }
  else
  {
    sample = output_sample; 
  }
  ++pointer;

  if(pointer == 32u)
  {

    //zero imaginary part
    for(uint16_t i=0; i<64; ++i){
       buffer_imag[i] = 0;
    }

    //window
    for (int i = 0; i < 64; i++) {
      buffer[ping_pong][i] = product(buffer[ping_pong][i], window[i]);
    }
    
    //FFT
    fixed_fft(buffer[ping_pong], buffer_imag, 6);

    //apply response
    for(uint16_t i=0; i<33; i++){
      buffer[ping_pong][i] = product(buffer[ping_pong][i], frequency_response[i]);
      buffer_imag[i] = product(buffer_imag[i], frequency_response[i]);
    }
    for(uint16_t i=33; i<64; i++){
      buffer[ping_pong][i] = product(buffer[ping_pong][i], frequency_response[64-i]);
      buffer_imag[i] = product(buffer_imag[i], frequency_response[64-i]);
    }

    //inverse FFT
    fixed_ifft(buffer[ping_pong], buffer_imag, 6);

    for(uint16_t i=0; i<32; ++i)
    {
       output_buffer[i] = buffer[ping_pong][i] + overlap[i];
       overlap[i] = buffer[ping_pong][i+32];
    }

    pointer = 0u;
    ping_pong ^= 1;

  }
}
