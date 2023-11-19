#ifndef FIXED_H_
#define FIXED_H_
#include <cstdint>

int32_t product(int32_t a, int32_t b);
int32_t float2fixed(float a);
float fixed2float(int32_t a);
int32_t clamp(int32_t a);
int32_t sum(int32_t a, int32_t b);

#endif
