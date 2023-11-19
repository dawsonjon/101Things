#include "effects.h"
#include "fixed.h"
#include <cstdio>
#include <cmath>

const uint16_t audio_sample_rate_Hz = 20000;
const uint16_t samples_per_ms = audio_sample_rate_Hz/1000;
int16_t sin_table[1024];

uint16_t frequency_Hz_to_steps(float frequency_Hz)
{
    return static_cast<uint32_t>(65536 * frequency_Hz/audio_sample_rate_Hz);
}

//Low Frequency Oscillator Class
int32_t lfo :: get_sample(uint16_t frequency_steps, int32_t amplitude)
{
    const int32_t sample = product(sin_table[p>>6], amplitude); //10 MSBs (16-10 = 4)
    p += frequency_steps;
    return sample;
}

//Delay Line Class
void delay_line :: input_sample(int32_t sample)
{
    buffer[input_pointer] = sample;
    input_pointer = (input_pointer + 1) & 0xfff;
}

int32_t delay_line :: tap(uint16_t delay)
{
    return buffer[(input_pointer - delay + 1) & 0xfff];
}

void effects :: update_settings(s_effect & settings)
{
  m_settings = settings;
  eq1.set_eq(m_settings.eq_gains);
}

//Effects Class
void effects :: process_sample(int16_t & s)
{
    int32_t sample = s;

    //pre gain
    dc += (sample - dc)/2u;
    sample -= dc;
    sample = product(sample, m_settings.pre_gain);

    //graphic equalizer
    eq1.process_sample(sample);

    //Distortion
    int32_t magnitude;
    bool positive;
    if(m_settings.distortion_effect != DISTORTION_OFF)
    {
        sample = product(sample, m_settings.distortion_gain);
        sample += m_settings.distortion_offset;
        positive = sample >= 0;
        magnitude = positive?sample:-sample;
    }

    switch(m_settings.distortion_effect)
    {

      case CUBIC:
        if(sample > float2fixed(1.0f))
        {
          sample = float2fixed(2.0f/3.0f);
        }
        else if(sample < float2fixed(-1.0f))   
        {
          sample = float2fixed(-2.0f/3.0f);
        }
        else
        {
          //x - x^3/3
          sample = sample -product(product(product(sample, sample), sample), float2fixed(1.0f/3.0f));
        }
        sample = product(sample, float2fixed(1.5f));
        break;

      case QUADRATIC:
        if(sample > float2fixed(2.0f/3.0f))
        {
          sample = float2fixed(1.0f);
        }
        else if(sample < float2fixed(-2.0f/3.0f))   
        {
          sample = float2fixed(-1.0f);
        }
        else if(sample > float2fixed(1.0f/3.0f))
        {
          //-3x^2 + 4x - 1/3
          sample = product(float2fixed(-3.0f), product(sample, sample)) + product(float2fixed(4.0f), sample) - float2fixed(1.0f/3.0f);
        }
        else if(sample < -float2fixed(1.0f/3.0f))   
        {
          //3x^2 - 4x + 1/3
          sample = product(float2fixed(3.0f), product(sample, sample)) + product(float2fixed(4.0f), sample) + float2fixed(1.0f/3.0f);
        }
        else
        {
          sample = product(float2fixed(2.0f), sample);
        }
        break;

      case FULL_WAVE:
        sample = magnitude;
        break;

      case HALF_WAVE:
        sample = positive?magnitude:float2fixed(0.0f);
        break;

      case FOLDBACK:
        //-4x^2 + 4x
        magnitude = product(float2fixed(-4.0f), product(magnitude, magnitude)) + product(float2fixed(4.0f), magnitude);
        sample = positive?magnitude:-magnitude;
        break;

      case FUZZ1:
        if(sample > float2fixed(0.66f))
        {
          sample = float2fixed(1.0f);
        }
        if(sample < float2fixed(-0.66f))
        {
          sample = float2fixed(-1.0f);
        }
        break;

      case FUZZ2:
        if(sample > float2fixed(0.66f))
        {
          sample = float2fixed(1.0f);
        }
        break;

    }

    //delay
    switch(m_settings.delay_effect)
    {
      case DELAY:
        delay_line2.input_sample(sample);
        sample += product(delay_line2.tap(m_settings.delay_delay_ms * samples_per_ms), m_settings.delay_feedback);
        break;

      case ECHO:
        sample += product(delay_line2.tap(m_settings.echo_delay_ms * samples_per_ms), m_settings.echo_feedback);
        delay_line2.input_sample(sample);
        break;
    }


    //Modulation Effects
    int32_t modulation;
    switch(m_settings.modulator_effect)
    {
      case PITCH_SHIFT:
        delay_line1.input_sample(sample);
        sample = delay_line1.tap(sweep);
        sweep -= m_settings.pitch;
        break;

      case VIBRATO:
        delay_line1.input_sample(sample);
        modulation = lfo1.get_sample(m_settings.vibrato_rate_steps, product(m_settings.vibrato_depth_ms, samples_per_ms));
        modulation += product(m_settings.vibrato_depth_ms, samples_per_ms*2);
        sample = delay_line1.tap(modulation);
        break;

      case TREMOLO:
        modulation = lfo1.get_sample(m_settings.tremolo_rate_steps, float2fixed(0.5)) + float2fixed(0.5);
        sample = product(sample, modulation); 
        break;

      case CHORUS:
        delay_line1.input_sample(sample);
        modulation = lfo1.get_sample(m_settings.chorus1_rate_steps, product(m_settings.chorus1_depth_ms, samples_per_ms));
        sample += product(delay_line1.tap(m_settings.chorus1_delay_ms * samples_per_ms + modulation), m_settings.chorus1_feedback);
        modulation = lfo1.get_sample(m_settings.chorus2_rate_steps, product(m_settings.chorus2_depth_ms, samples_per_ms));
        sample += product(delay_line1.tap(m_settings.chorus2_delay_ms * samples_per_ms + modulation), m_settings.chorus2_feedback);
        modulation = lfo1.get_sample(m_settings.chorus3_rate_steps, product(m_settings.chorus3_depth_ms, samples_per_ms));
        sample += product(delay_line1.tap(m_settings.chorus3_delay_ms * samples_per_ms + modulation), m_settings.chorus3_feedback);
        break;

      case FLANGER:
        delay_line1.input_sample(sample);
        modulation = lfo1.get_sample(m_settings.flanger_rate_steps, product(m_settings.flanger_depth_ms, samples_per_ms));
        sample += product(delay_line1.tap(m_settings.flanger_delay_ms * samples_per_ms + modulation), m_settings.flanger_feedback);
        break;
    }

    //hard clipping
    s = clamp(sample);
}

void effects :: initialise()
{
  for(uint16_t idx=0; idx<1024; idx++)
  {
    sin_table[idx] = float2fixed(sin(2.0*M_PI*idx/1024.0));
  }
}

