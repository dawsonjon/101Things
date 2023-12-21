#ifndef CORDIC_H__
#define CORDIC_H__

#include <cstdint>

void cordic_init();
void cordic_rectangular_to_polar(int16_t i, int16_t q, uint16_t &magnitude, int16_t &phase);

#endif
