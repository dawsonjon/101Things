#include "effects.h"
#include <cstdio>
#include <cstdint>

int main()
{

  FILE *inf = fopen("testfile", "r");
  FILE *outf = fopen("outfile", "w");
  effects e; 
  e.initialise();
  s_effect settings;

  settings.pre_gain = 4.0;
  settings.eq_gains[0] = 1.0;
  settings.eq_gains[1] = 2.0;
  settings.eq_gains[2] = 4.0;
  settings.eq_gains[3] = 2.0;
  settings.eq_gains[4] = 1.0;
  settings.distortion_effect = DISTORTION_OFF;
  settings.distortion_offset = 0.0;
  settings.distortion_gain = 4.0;
  settings.delay_effect = DELAY_OFF;
  settings.delay_delay_ms = 200.0;
  settings.delay_feedback = 0.8;
  settings.echo_delay_ms = 200.0;
  settings.echo_feedback = 0.9;
  settings.modulator_effect = MODULATOR_OFF;
  settings.pitch = 1.0f;
  settings.flanger_depth_ms = 5.0;
  settings.flanger_rate_Hz = 1.0;
  settings.flanger_delay_ms = 200.0;
  settings.flanger_feedback = 0.9;
  settings.chorus1_depth_ms = 5.0;
  settings.chorus1_rate_Hz = 1.1;
  settings.chorus1_delay_ms = 20.0;
  settings.chorus1_feedback = 0.9;
  settings.chorus2_depth_ms = 5.0;
  settings.chorus2_rate_Hz = 1.2;
  settings.chorus2_delay_ms = 21.0;
  settings.chorus2_feedback = 0.9;
  settings.chorus3_depth_ms = 5.0;
  settings.chorus3_rate_Hz = 1.3;
  settings.chorus3_delay_ms = 22.0;
  settings.chorus3_feedback = 0.9;
  settings.tremolo_rate_Hz = 10.0;
  settings.vibrato_depth_ms = 5.0;
  settings.vibrato_rate_Hz = 1.0;

  while(!feof(inf))
  {
    int16_t sample = fgetc(inf);
    sample |= fgetc(inf) << 8;
    sample -= 32768;

    printf("%i \n", sample);
    e.process_sample(sample, settings);

    fputc((sample & 0xff), outf);
    fputc((sample >> 8), outf);
  }  
  fclose(inf);
  fclose(outf);

  return 0;
}
