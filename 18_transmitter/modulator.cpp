#include <stdio.h>
#include <cmath>

#include "modulator.h"

void modulator :: process_sample(tx_mode_t mode, int8_t audio, uint8_t &magnitude, int8_t &phase, uint32_t fm_deviation_f8)
{

  if(mode == AM)
  {
    magnitude = audio + 128;
    phase = 0;
  }
  else if(mode == FM)
  {
    magnitude = 255;
    phase = last_phase + ((audio * fm_deviation_f8)>>8);
    last_phase = phase;
  }
  else if(mode == LSB || mode == USB)
  {
      //shift frequency by +FS/4
      //      __|__
      //  ___/  |  \___
      //        |
      //  <-----+----->

      //        | ____
      //  ______|/    \
      //        |
      //  <-----+----->

      //filter -Fs/4 to +Fs/4

      //        | __  
      //  ______|/  \__
      //        |
      //  <-----+----->


      if(mode == USB)
      {
        ssb_phase = (ssb_phase + 1) & 3u;
      }
      else
      {
        ssb_phase = (ssb_phase - 1) & 3u;
      }


      if(audio == -128) audio = -127;
      const int8_t audio_i[4] = {audio, 0, -audio, 0};
      const int8_t audio_q[4] = {0, -audio, 0, audio};
      int16_t ii = audio_i[ssb_phase];
      int16_t qq = audio_q[ssb_phase];
      ssb_filter.filter(ii,  qq);

      //shift frequency by -FS/4
      //        | __  
      //  ______|/  \__
      //        |
      //  <-----+----->

      //     __ |     
      //  __/  \|______
      //        |
      //  <-----+----->

      if(ii < -127) ii = -127;
      if(qq < -127) qq = -127;
      if(ii > 127) ii = 127;
      if(qq > 127) qq = 127;
      const int8_t sample_i[4] = {-qq, -ii, qq, ii};
      const int8_t sample_q[4] = {ii, -qq, -ii, qq};
      magnitude = rectangular_2_magnitude(sample_i[ssb_phase], sample_q[ssb_phase]);
      phase = rectangular_2_phase(sample_i[ssb_phase], sample_q[ssb_phase]);

  }

}

//from: http://dspguru.com/dsp/tricks/magnitude-estimator/
uint8_t modulator :: rectangular_2_magnitude(int8_t i, int8_t q)
{
  //Measure magnitude
  const int8_t absi = i>0?i:-i;
  const int8_t absq = q>0?q:-q;
  return absi > absq ? absi + absq / 4 : absq + absi / 4;
}

//from: https://dspguru.com/dsp/tricks/fixed-point-atan2-with-self-normalization/
//converted to fixed point
int8_t modulator :: rectangular_2_phase(int8_t i, int8_t q)
{
   //handle condition where phase is unknown
   if(i==0 && q==0) return 0;

   const int8_t absi=i>0?i:-i;
   int8_t angle=0;
   if (q>=0)
   {
      //scale r so that it lies in range -32 to 32
      const int16_t r = ((int16_t)(q - absi) << 5) / (q + absi);
      angle = 32 - r;
   }
   else
   {
      //scale r so that it lies in range -32 to 32
      const int16_t r = ((int16_t)(q + absi) << 5) / (absi - q);
      angle = (3 * 32) - r;
   }

   //angle lies in range -128 to 127
   if (i < 0) return(-angle);     // negate if in quad III or IV
   else return(angle);
}

