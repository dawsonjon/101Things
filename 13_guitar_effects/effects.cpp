#include "effects.h"
#include <cstdio>
#include <cmath>

const float audio_sample_rate_Hz = 20000;
const float samples_per_ms = audio_sample_rate_Hz/1000.0f;
float sin_table[1024];

//Low Frequency Oscillator Class
float lfo :: get_sample(float frequency_Hz, float amplitude, float audio_sample_rate)
{
    const uint32_t f = static_cast<uint32_t>(4294967296.0 * frequency_Hz/audio_sample_rate);
    const float sample = sin_table[p>>22] * amplitude; //10 MSBs (32-10 = 22)
    p += f;
    return sample;
}

//Delay Line Class
void delay_line :: input_sample(float sample)
{
    buffer[input_pointer] = sample;
    input_pointer = (input_pointer + 1) & 0xfff;
}

float delay_line :: tap(uint16_t delay)
{
    return buffer[(input_pointer - delay + 1) & 0xfff];
}

//Effects Class
void effects :: process_sample(int16_t & sample, s_effect & settings)
{
    //pre gain
    float temp = sample/32767.0f;
    dc += (temp - dc)/2.0f;
    temp -= dc;

    temp *= settings.pre_gain;
    //temp = eq1.applyfilter(temp, eq_gains);

    //Distortion
    switch(settings.distortion_effect)
    {

      case CUBIC:
        temp *= settings.distortion_gain;
        temp += settings.distortion_offset;
        if(temp > 1.0f)
        {
          temp = 2.0f/3.0f;
        }
        else if(temp < -1.0f)   
        {
          temp = -2.0f/3.0f;
        }
        else
        {
          temp = temp - ((temp * temp * temp) / 3.0f);
        }
        temp *= 1.5;
        break;

      case QUADRATIC:
        temp *= settings.distortion_gain;
        temp += settings.distortion_offset;
        if(temp > 2.0f/3.0f)
        {
          temp = 1.0f;
        }
        else if(temp < -2.0f/3.0f)   
        {
          temp = -1.0f;
        }
        else if(temp > 1.0f/3.0f)
        {
          //-3x^2 + 4x - 1/3
          temp = (-3.0f * temp * temp) + (4.0f * temp) - (1.0f/3.0f);
        }
        else if(temp < -1.0f/3.0f)   
        {
          //3x^2 - 4x + 1/3
          temp = (3.0f * temp * temp) + (4.0f * temp) + (1.0f/3.0f);
        }
        else
        {
          temp = 2.0f * temp;
        }
        break;

      case FULL_WAVE:
        temp *= settings.distortion_gain;
        temp += settings.distortion_offset;
        if(temp < 0)
        {
          temp = -temp;
        }
        break;

      case HALF_WAVE:
        temp *= settings.distortion_gain;
        temp += settings.distortion_offset;
        if(temp < 0)
        {
          temp = 0;
        }
        break;

      case FUZZ:
        temp *= settings.distortion_gain;
        temp += settings.distortion_offset;
        if(temp > 0)
        {
          temp = 1.0f;
        }
        break;

    }

    //reverb
    switch(settings.delay_effect)
    {
      case DELAY:
        delay_line2.input_sample(temp);
        temp += delay_line2.tap(settings.delay_delay_ms * samples_per_ms) * settings.delay_feedback;
        break;

      case REVERB:
        temp += delay_line2.tap(settings.reverb_delay_ms * samples_per_ms) * settings.reverb_feedback;
        delay_line2.input_sample(temp);
        break;
    }

    //Modulation Effects
    const float vibrato_delay_ms = settings.vibrato_depth_ms * 2.0f;
    switch(settings.modulator_effect)
    {
      case PITCH_SHIFT:
        delay_line1.input_sample(temp);
        temp = delay_line1.tap(sweep);
        sweep -= settings.pitch;
        break;

      case VIBRATO:
        delay_line1.input_sample(temp);
        temp = delay_line1.tap(vibrato_delay_ms * samples_per_ms + lfo1.get_sample(settings.vibrato_rate_Hz, settings.vibrato_depth_ms * samples_per_ms, audio_sample_rate_Hz));
        break;

      case TREMOLO:
        temp *= lfo1.get_sample(settings.tremolo_rate_Hz, 0.5f, audio_sample_rate_Hz) + 0.5f;
        break;

      case CHORUS:
        delay_line1.input_sample(temp);
        temp += delay_line1.tap(settings.chorus1_delay_ms * samples_per_ms + lfo1.get_sample(settings.chorus1_rate_Hz, settings.chorus1_depth_ms * samples_per_ms, audio_sample_rate_Hz)) * settings.chorus1_feedback;
        temp += delay_line1.tap(settings.chorus2_delay_ms * samples_per_ms + lfo2.get_sample(settings.chorus2_rate_Hz, settings.chorus2_depth_ms * samples_per_ms, audio_sample_rate_Hz)) * settings.chorus2_feedback;
        temp += delay_line1.tap(settings.chorus3_delay_ms * samples_per_ms + lfo3.get_sample(settings.chorus3_rate_Hz, settings.chorus3_depth_ms * samples_per_ms, audio_sample_rate_Hz)) * settings.chorus3_feedback;
        break;

      case FLANGER:
        delay_line1.input_sample(temp);
        temp += delay_line1.tap(settings.flanger_delay_ms * samples_per_ms + lfo1.get_sample(settings.flanger_rate_Hz, settings.flanger_depth_ms * samples_per_ms, audio_sample_rate_Hz)) * settings.flanger_feedback;
        break;
    }

    //hard clipping
    if(temp > 1.0f) temp = 1.0f;
    if(temp < -1.0f) temp = -1.0f;
    sample = temp * 32767;
}

void effects :: initialise()
{
  for(uint16_t idx=0; idx<1024; idx++)
  {
    sin_table[idx] = sin(2.0*M_PI*idx/1024.0);
  }
}

