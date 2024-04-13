#ifndef MPU6050_H_
#define MPU6050_H_

#include <cstdint>
#include "hardware/i2c.h"

class mpu6050
{
  public:
  mpu6050(i2c_inst_t *i2c, uint8_t sda_pin, uint8_t scl_pin);
  void get_pitch_roll(float &pitch, float &roll);

  private:
  i2c_inst_t *m_i2c;
};

#endif
