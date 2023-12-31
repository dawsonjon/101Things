//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2023
// filename: modulator.cpp
// description:
// License: MIT
//

#include <cmath>
#include <stdio.h>

#include "modulator.h"

void modulator ::process_sample(int16_t audio, int16_t &phase, uint32_t fm_deviation_f15) {
  phase = last_phase + ((audio * fm_deviation_f15) >> 15);
  last_phase = phase;
}
