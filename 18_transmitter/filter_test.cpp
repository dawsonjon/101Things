#include "modulator.h"
#include <cmath>

int main()
{

  half_band_filter2 uut;
 
  double sample_rate = 125e6/(256*32);
  
  for(uint16_t idx = 0; idx<1024; idx++)
  {
      int16_t i = idx==0?127:0;
      int16_t q = 0;
      i <<= 8;
      q <<= 8;
      uut.filter(i, q);
      printf("%i %i\n", i, q);
  }


}

