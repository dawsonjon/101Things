#ifndef EFFECTS_H
#define EFFECTS_H
#include <cstdint>
#include "eq.h"

enum e_modulator_effect {MODULATOR_OFF, CHORUS, FLANGER, TREMOLO, VIBRATO, PITCH_SHIFT}; 
enum e_delay_effect {DELAY_OFF, DELAY, ECHO}; 
enum e_distortion_effect {DISTORTION_OFF, CUBIC, QUADRATIC, FULL_WAVE, HALF_WAVE, FOLDBACK, FUZZ1, FUZZ2};

struct s_effect
{
  int32_t pre_gain = 1.0f;
  float eq_gains[5] = {1.0, 1.0, 1.0, 1.0, 1.0};

  e_distortion_effect distortion_effect = DISTORTION_OFF;
  float distortion_offset = 0.0f;
  float distortion_gain = 1.0f;

  e_delay_effect delay_effect = DELAY_OFF;
  float delay_delay_ms = 400.0f;
  float delay_feedback = 0.8;
  float echo_delay_ms = 100.0f;
  float echo_feedback = 0.8;

  e_modulator_effect modulator_effect = CHORUS;
  float pitch = 1.0f;
  float flanger_depth_ms = 1.0f;  // e.g 1-2 ms
  float flanger_rate_Hz = 1.0f;   //rate in Hz e.g. 0.1 - 4
  float flanger_delay_ms = 1.0f;  //delay in ms e.g. 1 - 5
  float flanger_feedback = 0.5f;  //e.g. 0.0 to 1.0
  float chorus1_depth_ms = 2.0f;  // e.g 1-2 ms
  float chorus1_rate_Hz = 1.0f;   //rate in Hz e.g. 0.1 - 5
  float chorus1_delay_ms = 20.0f; //delay in ms e.g. 20 - 50
  float chorus1_feedback = 0.5f;  //e.g. 0.0 to 1.0
  float chorus2_depth_ms = 2.0f;  // e.g 1-2 ms
  float chorus2_rate_Hz = 2.0f;   //rate in Hz e.g. 0.1 - 5
  float chorus2_delay_ms = 30.0f; //delay in ms e.g. 20 - 50
  float chorus2_feedback = 0.5f;  //e.g. 0.0 to 1.0
  float chorus3_depth_ms = 2.0f;  // e.g 1-2 ms
  float chorus3_rate_Hz = 4.0f;   //rate in Hz e.g. 0.1 - 5
  float chorus3_delay_ms = 40.0f; //delay in ms e.g. 20 - 50
  float chorus3_feedback = 0.8f;  //e.g. 0.0 to 1.0
  float tremolo_rate_Hz = 20.0f;  //rate in Hz e.g. 0.1 - 100
  float vibrato_depth_ms = 4.0f;  // e.g 1-2 ms
  float vibrato_rate_Hz = 2.0f;   //rate in Hz e.g. 0.1 - 5
};

class lfo
{
  uint32_t p = 0;
  public:
  float get_sample(float frequency_Hz, float amplitude, float audio_sample_rate);
};

class delay_line
{
  float buffer[4096] = {0};
  int16_t input_pointer = 0;
  public:
  void input_sample(float sample);
  float tap(uint16_t delay);
};


class effects
{
  int32_t dc = 0;
  lfo lfo1;
  lfo lfo2;
  lfo lfo3;
  delay_line delay_line1;
  delay_line delay_line2;
  eq eq1;
  float sweep = 0;
  s_effect m_settings;
  public:
  static void initialise();
  void update_settings(s_effect & effect_settings);
  void process_sample(int16_t & sample);
};



#endif
