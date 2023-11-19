#include "fixed.h"
#include <cmath>
#include <cstdint>

//4 integer bits, 12 fraction bits range = -8 <= x < 8
int32_t product(int32_t a, int32_t b)
{
  int32_t product = a * b;
  return (product + 16384) >> 15;
}

int32_t clamp(int32_t a)
{
  if(a > 32767)
  {
     return 32767;
  }
  else if(a < -32768)
  {
    return -32768;
  }
  else
  {
    return a;
  }
}

int32_t sum(int32_t a, int32_t b)
{
  int32_t sum = a + b;
  return clamp(sum);
}

int32_t float2fixed(float a)
{
  return roundf(a*32768);
}

float fixed2float(int32_t a)
{
  return (float)a/32768;
}
