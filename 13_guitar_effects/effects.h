#ifndef EFFECTS_H
#define EFFECTS_H
#include <cstdint>
#include "eq.h"
#include "fixed.h"

uint16_t frequency_Hz_to_steps(float frequency_Hz);

enum e_modulator_effect {MODULATOR_OFF, CHORUS, FLANGER, TREMOLO, VIBRATO, PITCH_SHIFT}; 
enum e_delay_effect {DELAY_OFF, DELAY, ECHO}; 
enum e_distortion_effect {DISTORTION_OFF, CUBIC, QUADRATIC, FULL_WAVE, HALF_WAVE, FOLDBACK, FUZZ1, FUZZ2};

struct s_effect
{
  int32_t pre_gain = float2fixed(1.0f);
  int32_t eq_gains[5] = {0};

  e_distortion_effect distortion_effect = DISTORTION_OFF;
  int32_t distortion_offset = float2fixed(0.0f);
  int32_t distortion_gain = float2fixed(1.0f);

  e_delay_effect delay_effect = DELAY_OFF;
  uint16_t delay_delay_ms = 400u;
  int32_t delay_feedback = float2fixed(0.8);
  uint16_t echo_delay_ms = 100u;
  int32_t echo_feedback = float2fixed(0.8);

  e_modulator_effect modulator_effect = CHORUS;
  float pitch = 1.0f;
  int32_t flanger_depth_ms = 1u;  // e.g 1-2 ms
  uint16_t flanger_rate_steps = frequency_Hz_to_steps(1.0f);   //rate in Hz e.g. 0.1 - 4
  uint16_t flanger_delay_ms = 1.0f;  //delay in ms e.g. 1 - 5
  int32_t flanger_feedback = 0.5f;  //e.g. 0.0 to 1.0
  int32_t chorus1_depth_ms = 2u;  // e.g 1-2 ms
  uint16_t chorus1_rate_steps = frequency_Hz_to_steps(1.0f);   //rate in Hz e.g. 0.1 - 5
  uint16_t chorus1_delay_ms = 20.0f; //delay in ms e.g. 20 - 50
  int32_t chorus1_feedback = 0.5f;  //e.g. 0.0 to 1.0
  int32_t chorus2_depth_ms = 2u;  // e.g 1-2 ms
  uint16_t chorus2_rate_steps = frequency_Hz_to_steps(2.0f);   //rate in Hz e.g. 0.1 - 5
  uint16_t chorus2_delay_ms = 30.0f; //delay in ms e.g. 20 - 50
  int32_t chorus2_feedback = 0.5f;  //e.g. 0.0 to 1.0
  int32_t chorus3_depth_ms = 2u;  // e.g 1-2 ms
  uint16_t chorus3_rate_steps = frequency_Hz_to_steps(4.0f);   //rate in Hz e.g. 0.1 - 5
  uint16_t chorus3_delay_ms = 40.0f; //delay in ms e.g. 20 - 50
  int32_t chorus3_feedback = 0.8f;  //e.g. 0.0 to 1.0
  uint16_t tremolo_rate_steps = frequency_Hz_to_steps(20.0f);  //rate in Hz e.g. 0.1 - 100
  int32_t vibrato_depth_ms = 4u;  // e.g 1-2 ms
  uint16_t vibrato_rate_steps = frequency_Hz_to_steps(2.0f);   //rate in Hz e.g. 0.1 - 5
};

class lfo
{
  uint16_t p = 0;
  public:
  int32_t get_sample(uint16_t frequency_steps, int32_t amplitude);
};

class delay_line
{
  int32_t buffer[4096] = {0};
  uint16_t input_pointer = 0;
  public:
  void input_sample(int32_t sample);
  int32_t tap(uint16_t delay);
};


class effects
{
  int16_t dc = 0;
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
