#ifndef __MODULATOR_H__
#define __MODULATOR_H__

#include <stdio.h>
#include <cmath>
#include "half_band_filter2.h"

enum tx_mode_t {AM, FM, LSB, USB};

class modulator
{

  private:
  int8_t last_phase = 0;
  uint8_t ssb_phase = 0;
  half_band_filter2 ssb_filter;
  uint16_t rectangular_2_magnitude(int16_t i, int16_t q);
  int16_t rectangular_2_phase(int16_t i, int16_t q);

  public:
  void process_sample(tx_mode_t mode, int8_t audio, uint8_t &magnitude, int8_t &phase, uint32_t fm_deviation_f8);
};

#endif
