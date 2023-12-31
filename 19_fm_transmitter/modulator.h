//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: modulator.h
// description:
// License: MIT
//

#ifndef __MODULATOR_H__
#define __MODULATOR_H__

#include <stdio.h>
#include <cmath>


class modulator
{

  private:
  int16_t last_phase = 0;

  public:
  void process_sample(int16_t audio, int16_t &phase, uint32_t fm_deviation_f15);
};

#endif
