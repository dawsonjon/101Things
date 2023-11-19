#include "effects.h"
#include "fixed.h"
#include <cstdio>
#include <cstdint>

int main()
{

  FILE *inf = fopen("testfile", "r");
  FILE *outf = fopen("outfile", "w");
  effects e; 
  e.initialise();
  s_effect settings;

  settings.pre_gain = float2fixed(4.0);
  settings.eq_gains[0] = eq_const(4.0)/16; 
  settings.eq_gains[1] = eq_const(4.0)/16;
  settings.eq_gains[2] = eq_const(4.0)/16;
  settings.eq_gains[3] = eq_const(4.0)/16;
  settings.eq_gains[4] = eq_const(4.0)/16;
  settings.distortion_effect = FOLDBACK;
  settings.distortion_offset = float2fixed(0.0);
  settings.distortion_gain = float2fixed(8.0);
  settings.delay_effect = DELAY_OFF;
  settings.delay_delay_ms = 200.0;
  settings.delay_feedback = float2fixed(0.8);
  settings.echo_delay_ms = 200.0;
  settings.echo_feedback = float2fixed(0.9);
  settings.modulator_effect = MODULATOR_OFF;
  settings.pitch = 1.0f;
  settings.flanger_depth_ms = float2fixed(1.0);
  settings.flanger_rate_steps = frequency_Hz_to_steps(1.0);
  settings.flanger_delay_ms = 200u;
  settings.flanger_feedback = float2fixed(0.9);
  settings.chorus1_depth_ms = float2fixed(1.0);
  settings.chorus1_rate_steps = frequency_Hz_to_steps(1.1);
  settings.chorus1_delay_ms = 20u;
  settings.chorus1_feedback = float2fixed(0.9);
  settings.chorus2_depth_ms = float2fixed(1.0);
  settings.chorus2_rate_steps = frequency_Hz_to_steps(1.2);
  settings.chorus2_delay_ms = 21u;
  settings.chorus2_feedback = float2fixed(0.9);
  settings.chorus3_depth_ms = float2fixed(1.0);
  settings.chorus3_rate_steps = frequency_Hz_to_steps(1.3);
  settings.chorus3_delay_ms = 22u;
  settings.chorus3_feedback = float2fixed(0.9);
  settings.tremolo_rate_steps = frequency_Hz_to_steps(10.0);
  settings.vibrato_depth_ms = float2fixed(1.0);
  settings.vibrato_rate_steps = frequency_Hz_to_steps(2.0);
  e.update_settings(settings);

  while(!feof(inf))
  {
    int16_t sample = fgetc(inf);
    sample |= fgetc(inf) << 8;
    sample -= 32768;

    e.process_sample(sample);

    fputc((sample & 0xff), outf);
    fputc((sample >> 8), outf);
  }  
  fclose(inf);
  fclose(outf);

  return 0;
}
