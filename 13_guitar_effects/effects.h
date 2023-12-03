#ifndef EFFECTS_H
#define EFFECTS_H
#include <cstdint>
#include "eq.h"
#include "fixed.h"

typedef fixed_point<int32_t, 15> FixedPoint;

uint16_t frequency_Hz_to_steps(float frequency_Hz);

enum e_modulator_effect {MODULATOR_OFF, CHORUS, FLANGER, TREMOLO, VIBRATO, PHASER}; 
enum e_delay_effect {DELAY_OFF, DELAY, ECHO, REVERB}; 
enum e_distortion_effect {DISTORTION_OFF, CUBIC, QUADRATIC, FULL_WAVE, HALF_WAVE, FOLDBACK, FUZZ};

struct s_effect
{
  FixedPoint pre_gain = FixedPoint::from_float(1.0f);
  int32_t eq_gains[5] = {0};

  e_distortion_effect distortion_effect = DISTORTION_OFF;
  FixedPoint distortion_offset = FixedPoint::from_float(0.0f);
  FixedPoint distortion_gain = FixedPoint::from_float(1.0f);

  e_delay_effect delay_effect = DELAY_OFF;
  uint16_t delay_delay_ms = 400u;
  FixedPoint delay_feedback = FixedPoint::from_float(0.8);
  FixedPoint delay_mix = FixedPoint::from_float(0.1);

  e_modulator_effect modulator_effect = PHASER;
  FixedPoint flanger_depth_ms = FixedPoint::from_float(0.5f);  // e.g 1-2 ms
  uint16_t flanger_rate_steps = frequency_Hz_to_steps(1.0f);   //rate in Hz e.g. 0.1 - 4
  uint16_t flanger_delay_ms = 1u;  //delay in ms e.g. 1 - 5
  FixedPoint flanger_feedback = FixedPoint::from_float(0.5f);  //e.g. 0.0 to 1.0
  FixedPoint chorus1_depth_ms = FixedPoint::from_float(2u);  // e.g 1-2 ms
  uint16_t chorus1_rate_steps = frequency_Hz_to_steps(1.0f);   //rate in Hz e.g. 0.1 - 5
  uint16_t chorus1_delay_ms = 20u; //delay in ms e.g. 20 - 50
  FixedPoint chorus1_feedback = FixedPoint::from_float(0.5f);  //e.g. 0.0 to 1.0
  FixedPoint chorus2_depth_ms = FixedPoint::from_float(2.0f);  // e.g 1-2 ms
  uint16_t chorus2_rate_steps = frequency_Hz_to_steps(2.0f);   //rate in Hz e.g. 0.1 - 5
  uint16_t chorus2_delay_ms = 30u; //delay in ms e.g. 20 - 50
  FixedPoint chorus2_feedback = FixedPoint::from_float(0.5f);  //e.g. 0.0 to 1.0
  FixedPoint chorus3_depth_ms = FixedPoint::from_float(2.0f);  // e.g 1-2 ms
  uint16_t chorus3_rate_steps = frequency_Hz_to_steps(4.0f);   //rate in Hz e.g. 0.1 - 5
  uint16_t chorus3_delay_ms = 40u; //delay in ms e.g. 20 - 50
  FixedPoint chorus3_feedback = FixedPoint::from_float(0.8f);  //e.g. 0.0 to 1.0
  uint16_t tremolo_rate_steps = frequency_Hz_to_steps(20.0f);  //rate in Hz e.g. 0.1 - 100
  FixedPoint tremolo_depth = FixedPoint::from_float(0.8f);  //e.g. 0.0 to 1.0
  uint16_t vibrato_rate_steps = frequency_Hz_to_steps(2.0f);   //rate in Hz e.g. 0.1 - 5
  FixedPoint vibrato_depth_ms = FixedPoint::from_float(4u);  // e.g 1-2 ms
  uint16_t phaser_rate_steps = frequency_Hz_to_steps(1.0f);  //rate in Hz e.g. 0.1 - 100
  FixedPoint phaser_depth = FixedPoint::from_float(0.8f);  //e.g. 0.0 to 1.0
};

class lfo
{
  uint16_t p = 0;
  public:
  FixedPoint get_sample(uint16_t frequency_steps, FixedPoint amplitude);
};

class delay_line
{
  FixedPoint buffer[4096] = {0};
  uint16_t input_pointer = 0;
  public:
  void input_sample(FixedPoint sample);
  FixedPoint tap(uint16_t delay);
};

class all_pass
{
  FixedPoint z1 = 0;

  public:
  FixedPoint process_sample(FixedPoint sample, FixedPoint k)
  {
    FixedPoint z0 = sample + (z1 * k);
    sample = z1 - (z0 * k);
    z1 = z0; //single element delay
    return sample;
  }

};


class effects
{
  FixedPoint dc = 0;
  all_pass all_pass1;
  all_pass all_pass2;
  all_pass all_pass3;
  lfo lfo1;
  lfo lfo2;
  lfo lfo3;
  delay_line delay_line1;
  delay_line delay_line2;
  delay_line delay_line3;
  delay_line delay_line4;
  delay_line modulator_delay_line;
  eq eq1;
  s_effect m_settings;
  public:
  static void initialise();
  void update_settings(s_effect & effect_settings);
  void process_sample(int16_t & sample);
};



#endif
