#include "effects.h"
#include <cstdio>
#include <cmath>

const float audio_sample_rate_Hz = 10000;
const float samples_per_ms = audio_sample_rate_Hz/1000.0f;
enum e_modulator_effect {MODULATOR_OFF, CHORUS, FLANGER, TREMOLO, VIBRATO, PITCH_SHIFT};
enum e_delay_effect {DELAY_OFF, DELAY, REVERB};
enum e_distortion_effect {DISTORTION_OFF, CUBIC, QUADRATIC, FULL_WAVE, HALF_WAVE};

float sin_table[1024];

//settings
const float pre_gain = 16.0f;
float eq_gains[5] = {1.0, 1.0, 1.0, 1.0, 1.0};

const e_distortion_effect distortion_effect = HALF_WAVE;
const float distortion_offset = 0.0f;
const float distortion_gain = 4.0f;

const e_delay_effect delay_effect = REVERB;
const float delay_delay_ms = 400.0f;
const float delay_feedback = 0.8;
const float reverb_delay_ms = 100.0f;
const float reverb_feedback = 0.8;

const e_modulator_effect modulator_effect = CHORUS;
const float pitch = 1.0f;
const float flanger_depth_ms = 1.0f; // e.g 1-2 ms
const float flanger_rate_Hz = 1.0f; //rate in Hz e.g. 0.1 - 4
const float flanger_delay_ms = 1.0f; //delay in ms e.g. 1 - 5
const float flanger_feedback = 0.5f; //e.g. 0.0 to 1.0
const float chorus1_depth_ms = 2.0f; // e.g 1-2 ms
const float chorus1_rate_Hz = 1.0f; //rate in Hz e.g. 0.1 - 5
const float chorus1_delay_ms = 20.0f; //delay in ms e.g. 20 - 50
const float chorus1_feedback = 0.5f; //e.g. 0.0 to 1.0
const float chorus2_depth_ms = 2.0f; // e.g 1-2 ms
const float chorus2_rate_Hz = 2.0f; //rate in Hz e.g. 0.1 - 5
const float chorus2_delay_ms = 30.0f; //delay in ms e.g. 20 - 50
const float chorus2_feedback = 0.5f; //e.g. 0.0 to 1.0
const float chorus3_depth_ms = 2.0f; // e.g 1-2 ms
const float chorus3_rate_Hz = 4.0f; //rate in Hz e.g. 0.1 - 5
const float chorus3_delay_ms = 40.0f; //delay in ms e.g. 20 - 50
const float chorus3_feedback = 0.8f; //e.g. 0.0 to 1.0
const float tremolo_rate_Hz = 20.0f; //rate in Hz e.g. 0.1 - 100
const float vibrato_depth_ms = 4.0f; // e.g 1-2 ms
const float vibrato_rate_Hz = 2.0f; //rate in Hz e.g. 0.1 - 5

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
void effects :: process_sample(int16_t & sample)
{
    //pre gain
    float temp = sample/32767.0f;
    dc += (temp - dc)/2.0f;
    temp -= dc;

    temp *= pre_gain;

    //temp = eq1.applyfilter(temp, eq_gains);

    //Distortion
    switch(distortion_effect)
    {

      case CUBIC:
        temp *= distortion_gain;
        temp += distortion_offset;
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
        temp *= distortion_gain;
        temp += distortion_offset;
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
        temp *= distortion_gain;
        temp += distortion_offset;
        if(temp < 0)
        {
          temp = -temp;
        }
        break;

      case HALF_WAVE:
        temp *= distortion_gain;
        temp += distortion_offset;
        if(temp < 0)
        {
          temp = 0;
        }
        break;

    }

    //reverb
    switch(delay_effect)
    {
      case DELAY:
        delay_line2.input_sample(temp);
        temp += delay_line2.tap(delay_delay_ms * samples_per_ms) * delay_feedback;
        break;

      case REVERB:
        temp += delay_line2.tap(reverb_delay_ms * samples_per_ms) * reverb_feedback;
        delay_line2.input_sample(temp);
        break;
    }

    //Modulation Effects
    const float vibrato_delay_ms = vibrato_depth_ms * 2.0f;
    switch(modulator_effect)
    {
      case PITCH_SHIFT:
        delay_line1.input_sample(temp);
        temp = delay_line1.tap(sweep);
        sweep -= pitch;
        break;

      case VIBRATO:
        delay_line1.input_sample(temp);
        temp = delay_line1.tap(vibrato_delay_ms * samples_per_ms + lfo1.get_sample(vibrato_rate_Hz, vibrato_depth_ms * samples_per_ms, audio_sample_rate_Hz));
        break;

      case TREMOLO:
        temp *= lfo1.get_sample(tremolo_rate_Hz, 0.5f, audio_sample_rate_Hz) + 0.5f;
        break;

      case CHORUS:
        delay_line1.input_sample(temp);
        temp += delay_line1.tap(chorus1_delay_ms * samples_per_ms + lfo1.get_sample(chorus1_rate_Hz, chorus1_depth_ms * samples_per_ms, audio_sample_rate_Hz)) * chorus1_feedback;
        temp += delay_line1.tap(chorus2_delay_ms * samples_per_ms + lfo2.get_sample(chorus2_rate_Hz, chorus2_depth_ms * samples_per_ms, audio_sample_rate_Hz)) * chorus2_feedback;
        temp += delay_line1.tap(chorus3_delay_ms * samples_per_ms + lfo3.get_sample(chorus3_rate_Hz, chorus3_depth_ms * samples_per_ms, audio_sample_rate_Hz)) * chorus3_feedback;
        break;

      case FLANGER:
        delay_line1.input_sample(temp);
        temp += delay_line1.tap(flanger_delay_ms * samples_per_ms + lfo1.get_sample(flanger_rate_Hz, flanger_depth_ms * samples_per_ms, audio_sample_rate_Hz)) * flanger_feedback;
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

