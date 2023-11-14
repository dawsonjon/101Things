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

void effects :: update_settings(s_effect & settings)
{
  m_settings = settings;
  eq1.set_eq(m_settings.eq_gains);
}

//Effects Class
void effects :: process_sample(int16_t & sample)
{
    //pre gain
    dc += (sample - dc)/2u;
    sample -= dc;
    int32_t extended_sample = sample;
    //extended_sample = product(extended_sample, m_settings.pre_gain);

    //graphic equalizer
    //eq1.process_sample(extended_sample);

    //Distortion
    /*
    int32_t magnitude;
    bool positive;
    if(m_settings.distortion_effect != DISTORTION_OFF)
    {
        extended_sample = product(extended_sample, m_settings.distortion_gain);
        extended_sample += m_settings.distortion_offset;
        bool positive = extended_sample >= 0;
        magnitude = positive?extended_sample:-extended_sample;
    }

    switch(m_settings.distortion_effect)
    {

      case CUBIC:
        if(extended_sample > float2fixed(1.0f))
        {
          extended_sample = float2fixed(2.0f/3.0f);
        }
        else if(extended_sample < float2fixed(-1.0f))   
        {
          extended_sample = float2fixed(-2.0f/3.0f);
        }
        else
        {
          //x - x^3/3
          extended_sample = extended_sample - product(product(product(extended_sample, extended_sample), extended_sample), float2fixed(1.0f/3.0f));
        }
        extended_sample = product(extended_sample, float2fixed(1.5f));
        break;

      case QUADRATIC:
        if(extended_sample > float2fixed(2.0f/3.0f))
        {
          extended_sample = float2fixed(1.0f);
        }
        else if(extended_sample < float2fixed(-2.0f/3.0f))   
        {
          extended_sample = float2fixed(-1.0f);
        }
        else if(extended_sample > float2fixed(1.0f/3.0f))
        {
          //-3x^2 + 4x - 1/3
          extended_sample = product(float2fixed(-3.0f), product(extended_sample, extended_sample)) + product(float2fixed(4.0f), extended_sample) - float2fixed(1.0f/3.0f);
        }
        else if(extended_sample < -float2fixed(1.0f/3.0f))   
        {
          //3x^2 - 4x + 1/3
          extended_sample = product(float2fixed(3.0f), product(extended_sample, extended_sample)) + product(float2fixed(4.0f), extended_sample) + float2fixed(1.0f/3.0f);
        }
        else
        {
          extended_sample = product(float2fixed(2.0f), extended_sample);
        }
        break;

      case FULL_WAVE:
        extended_sample = magnitude;
        break;

      case HALF_WAVE:
        extended_sample = positive?magnitude:float2fixed(0.0f);
        break;

      case FOLDBACK:
        //-4x^2 + 4x
        magnitude = product(float2fixed(-4.0f), product(magnitude, magnitude)) + product(float2fixed(4.0f), magnitude);
        extended_sample = positive?magnitude:-magnitude;
        break;

      case FUZZ1:
        if(extended_sample > float2fixed(0.66f))
        {
          extended_sample = float2fixed(1.0f);
        }
        if(extended_sample < float2fixed(-0.66f))
        {
          extended_sample = float2fixed(-1.0f);
        }
        break;

      case FUZZ2:
        if(extended_sample > float2fixed(0.66f))
        {
          extended_sample = float2fixed(1.0f);
        }
        break;

    }
    */

    float temp = extended_sample/32767.0f;

    printf("%f\n", temp);

    //delay
    switch(m_settings.delay_effect)
    {
      case DELAY:
        delay_line2.input_sample(temp);
        temp += delay_line2.tap(m_settings.delay_delay_ms * samples_per_ms) * m_settings.delay_feedback;
        break;

      case ECHO:
        temp += delay_line2.tap(m_settings.echo_delay_ms * samples_per_ms) * m_settings.echo_feedback;
        delay_line2.input_sample(temp);
        break;
    }

    //Modulation Effects
    const float vibrato_delay_ms = m_settings.vibrato_depth_ms * 2.0f;
    switch(m_settings.modulator_effect)
    {
      case PITCH_SHIFT:
        delay_line1.input_sample(temp);
        temp = delay_line1.tap(sweep);
        sweep -= m_settings.pitch;
        break;

      case VIBRATO:
        delay_line1.input_sample(temp);
        temp = delay_line1.tap(vibrato_delay_ms * samples_per_ms + lfo1.get_sample(m_settings.vibrato_rate_Hz, m_settings.vibrato_depth_ms * samples_per_ms, audio_sample_rate_Hz));
        break;

      case TREMOLO:
        temp *= lfo1.get_sample(m_settings.tremolo_rate_Hz, 0.5f, audio_sample_rate_Hz) + 0.5f;
        break;

      case CHORUS:
        delay_line1.input_sample(temp);
        temp += delay_line1.tap(m_settings.chorus1_delay_ms * samples_per_ms + lfo1.get_sample(m_settings.chorus1_rate_Hz, m_settings.chorus1_depth_ms * samples_per_ms, audio_sample_rate_Hz)) * m_settings.chorus1_feedback;
        temp += delay_line1.tap(m_settings.chorus2_delay_ms * samples_per_ms + lfo2.get_sample(m_settings.chorus2_rate_Hz, m_settings.chorus2_depth_ms * samples_per_ms, audio_sample_rate_Hz)) * m_settings.chorus2_feedback;
        temp += delay_line1.tap(m_settings.chorus3_delay_ms * samples_per_ms + lfo3.get_sample(m_settings.chorus3_rate_Hz, m_settings.chorus3_depth_ms * samples_per_ms, audio_sample_rate_Hz)) * m_settings.chorus3_feedback;
        break;

      case FLANGER:
        delay_line1.input_sample(temp);
        temp += delay_line1.tap(m_settings.flanger_delay_ms * samples_per_ms + lfo1.get_sample(m_settings.flanger_rate_Hz, m_settings.flanger_depth_ms * samples_per_ms, audio_sample_rate_Hz)) * m_settings.flanger_feedback;
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

