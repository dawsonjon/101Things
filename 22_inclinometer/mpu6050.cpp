#include "mpu6050.h"
#include <cmath>
#include "pico/stdlib.h"

#include <cstdio>

static const int addr = 0x68;

mpu6050::mpu6050(i2c_inst_t *i2c, uint8_t sda_pin, uint8_t scl_pin) 
{
    i2c_init(i2c, 400 * 1000);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    m_i2c = i2c;
    

    //reset/wake
    uint8_t reset_command[] = {107, 128};
    i2c_write_blocking(i2c, addr, reset_command, 2, false);
    sleep_us(100000);

    uint8_t path_reset_command[] = {104, 7};
    i2c_write_blocking(i2c, addr, path_reset_command, 2, false);
    sleep_us(100000);

    uint8_t clock_select_command[] = {107, 1};
    i2c_write_blocking(i2c, addr, clock_select_command, 2, false);

    //read whoami
    //uint8_t val = 0x75;
    //uint8_t whoami[1];
    //i2c_write_blocking(i2c0, addr, &val, 1, true);
    //i2c_read_blocking(i2c0, addr, whoami, 1, false);

    //set acceleration scale
    uint8_t accel_scale_command[] = {0x1C, 0x00};
    i2c_write_blocking(i2c, addr, accel_scale_command, 2, false);
}

void mpu6050::get_pitch_roll(float &pitch, float &roll) 
{
    uint8_t buffer[6];
    uint8_t val = 0x3B;
    i2c_write_blocking(m_i2c, addr, &val, 1, true);
    i2c_read_blocking(m_i2c, addr, buffer, 6, false);

    int16_t x_raw = (buffer[0] << 8 | buffer[1]);
    int16_t y_raw = (buffer[2] << 8 | buffer[3]);
    int16_t z_raw = (buffer[4] << 8 | buffer[5]);

    float x = x_raw/16384.0f;
    float y = y_raw/16384.0f;
    float z = z_raw/16384.0f;

    //this depends on the orientation of the sensor
    roll = 180.0f * atan2f(x, sqrt(y*y + z*z))/M_PI;
    pitch = -180.0f * atan2f(z, sqrt(x*x + y*y))/M_PI;
}
