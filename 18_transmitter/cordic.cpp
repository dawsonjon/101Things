#include <cstdint>
#include <cstdio>
#include <stdlib.h>
#include <math.h>

const uint8_t cordic_iterations = 8;
int16_t thetas[cordic_iterations+1];
const int16_t half_pi = 16384;
int16_t recip_gain;

void cordic_init()
{
  double k;

  //calculate theta lookup table
  k = 1.0f;
  for (uint8_t idx = 0; idx <= cordic_iterations; idx++) {
    thetas[idx] = round(atan(k) * 32768/M_PI);
    k *= 0.5f;
  }

  //calculate cordic gain
  double gain = 1.0;
  for(uint8_t idx; idx < cordic_iterations; idx++)
  {
    const double d = pow(2.0, idx);
    const double magnitude = sqrt(1+(1.0/d)*(1.0/d));
    gain *= magnitude;
  }

  recip_gain = 32768/gain;

}

void cordic_rectangular_to_polar(int16_t i, int16_t q, uint16_t &magnitude, int16_t &phase)
{
  int32_t temp_i;
  int32_t i_32 = i;
  int32_t q_32 = q;

  if (i_32 < 0) {
    //rotate by an initial +/- 90 degrees
    temp_i = i_32;
    if (q > 0.0f) {
       i_32 = q_32;		//subtract 90 degrees
       q_32 = -temp_i;
       phase = -half_pi;
    } else {
       i_32 = -q_32;		//add 90 degrees
       q_32 = temp_i;
       phase = half_pi;
    }
  } else {
    phase = 0.0;
  }


  // rotate using "1 + jK" factors
  for (uint8_t idx = 0; idx <= cordic_iterations; idx++) {
    temp_i = i_32;
    if (q_32 >= 0.0) {
      //phase is positive: do negative roation
      i_32 += q_32 >> idx;
      q_32 -= temp_i >> idx;
      phase -= thetas[idx];
    } else {
      // phase is negative: do positive rotation
      i_32 -= q_32 >> idx;
      q_32 += temp_i >> idx;
      phase += thetas[idx];
    }
  }

  magnitude = (i_32 * recip_gain) >> 14;
}
