//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2023
// filename: half_band_filter_test.cpp
// description:
// License: MIT
//

#include <cstdio>
#include <math.h>
#include <stdlib.h>

#include "half_band_filter.h"

int main() {

  half_band_filter filter;

  for (uint16_t idx = 0; idx < 1024; idx++) {
    //int16_t audio = idx == 0 ? 32767 : 0;
    int16_t audio = 32767;
    filter.filter(audio);
    printf("%i\n", audio);
  }

  return 0;
}
