#include "modulator.h"
#include <cmath>

int main()
{

  modulator uut;
 
  double sample_rate = 125e6/(256*32);
  
  for(uint16_t i = 0; i<1024; i++)
  {
      //sin wave
      //int8_t audio = 127 * sin(2.0*M_PI*i*1e3/sample_rate);

      //two tone
      int8_t audio = 63 * sin(2.0*M_PI*i*0.8e3/sample_rate);
             audio += 63 * sin(2.0*M_PI*i*2.2e3/sample_rate);

      //impulse
      //int8_t audio = i==0?127:0;

      uint8_t magnitude;
      int8_t phase;
      uut.process_sample(LSB, audio, magnitude, phase, 0);
      printf("%i %i %i\n", audio, magnitude, phase);
  }


}

