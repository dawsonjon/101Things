#include "fixed.h"
#include <cmath>
#include <cstdint>

int32_t product(int32_t a, int32_t b)
{
  int64_t product = (int64_t)a * (int64_t)b;
  return (product + 16384) >> 15;
}

int32_t float2fixed(float a)
{
  return roundf(a*32768);
}

float fixed2float(int32_t a)
{
  return (float)a/32768;
}
