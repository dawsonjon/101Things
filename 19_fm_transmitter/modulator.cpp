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

#include <stdio.h>
#include <cmath>

#include "modulator.h"

void modulator :: process_sample(int16_t audio, uint16_t &magnitude, int16_t &phase, uint32_t fm_deviation_f15)
{
    magnitude = 65535;
    phase = last_phase + ((audio * fm_deviation_f15)>>15);
    last_phase = phase;
}
