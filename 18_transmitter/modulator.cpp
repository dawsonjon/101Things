#include <stdio.h>
#include <cmath>

#include "modulator.h"
#include "cordic.h"

modulator :: modulator()
{
  cordic_init();
}

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


      if(mode == LSB)
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
      int16_t ii = audio_i[ssb_phase] << 8;
      int16_t qq = audio_q[ssb_phase] << 8;
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

      const int16_t sample_i[4] = {-qq, -ii, qq, ii};
      const int16_t sample_q[4] = {ii, -qq, -ii, qq};
      uint16_t magnitude_16;
      int16_t phase_16;
      cordic_rectangular_to_polar(sample_i[ssb_phase], sample_q[ssb_phase], magnitude_16, phase_16);

      magnitude_16 = (magnitude_16 + (rand() & 0xff)) >> 7;
      if(magnitude_16 < 0) magnitude_16 = 0;
      if(magnitude_16 > 255) magnitude_16 = 255;
      magnitude = magnitude_16;

      phase_16 = (phase_16 + (rand() & 0xff)) >> 8;
      if(phase_16 < -127) phase_16 = -127;
      if(phase_16 > 127) phase_16 = 127;
      phase = phase_16;

  }

}
