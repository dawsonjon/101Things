//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: cordic_test.cpp
// description:
// License: MIT
//

#include <cstdio>
#include <stdlib.h>
#include <math.h>

#include "cordic.h"

int main() {

    cordic_init();

    for(uint16_t idx=0; idx<256; idx++)
    {
      int16_t i = cos(2*M_PI*idx/256) * 32767;
      int16_t q = sin(2*M_PI*idx/256) * 32767;
      uint16_t r;
      int16_t theta;
      cordic_rectangular_to_polar(i, q, r, theta);
      printf("%i %i %u %i\n", i, q, r, theta);
    }

    return 0;
}

