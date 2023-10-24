#ifndef EFFECTS_H
#define EFFECTS_H
#include <cstdint>
#include "eq.h"

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
  float dc = 0;
  lfo lfo1;
  lfo lfo2;
  lfo lfo3;
  delay_line delay_line1;
  delay_line delay_line2;
  eq eq1;
  float sweep = 0;
  public:
  static void initialise();
  void process_sample(int16_t & sample);
};



#endif
