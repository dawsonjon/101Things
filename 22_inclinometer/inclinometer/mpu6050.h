//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2024
// filename: mpu6050.h
// description: Interface to mpu6050 IMU
// License: MIT
//

#ifndef MPU6050_H_
#define MPU6050_H_

#include <cstdint>
#include "hardware/i2c.h"

class mpu6050
{
  public:
  mpu6050(i2c_inst_t *i2c, uint8_t sda_pin, uint8_t scl_pin);
  void get_pitch_roll(float &pitch, float &roll);
  void get_pitch_roll_rate(float &pitch_rate, float &roll_rate);
  void zero_gyro();

  private:
  i2c_inst_t *m_i2c;
  float x_gyro_zero = 0.0f;
  float y_gyro_zero = 0.0f;
  float z_gyro_zero = 0.0f;
};

#endif
