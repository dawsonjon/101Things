//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2023
// filename: pwm.cpp
// description: PWM Magnitude for Ham Transmitter
// License: MIT
//

#include "pwm.h"

pwm::pwm(const uint8_t magnitude_pin) {
  m_magnitude_pin = magnitude_pin;
  gpio_set_function(magnitude_pin, GPIO_FUNC_PWM);
  gpio_set_drive_strength(magnitude_pin, GPIO_DRIVE_STRENGTH_12MA);
  gpio_set_function(magnitude_pin + 1, GPIO_FUNC_PWM);
  gpio_set_drive_strength(magnitude_pin + 1, GPIO_DRIVE_STRENGTH_12MA);
  const uint16_t pwm_max = 255; // 8 bit pwm
  const int magnitude_pwm_slice = pwm_gpio_to_slice_num(magnitude_pin); // GPIO1
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 2.f); // 125MHz
  pwm_config_set_wrap(&config, pwm_max);
  pwm_config_set_output_polarity(&config, false, false);
  pwm_init(magnitude_pwm_slice, &config, true);
}

pwm::~pwm() {
  // disable GPIO, pullup/pulldown resistors should be installed
  // to switch off transistors when pin is high impedance
  gpio_deinit(m_magnitude_pin);
  gpio_deinit(m_magnitude_pin + 1);
}

void pwm::output_sample(uint16_t magnitude) {
  const bool balanced_mode = true;
  if (balanced_mode) {
#ifdef BALANCED
    pwm_set_gpio_level(m_magnitude_pin, 128 + (magnitude >> 9));
    pwm_set_gpio_level(m_magnitude_pin + 1, 128 - (magnitude >> 9));
#else
    // remove 8 lsbs
    magnitude >>= 8;
    pwm_set_gpio_level(m_magnitude_pin, magnitude);
    pwm_set_gpio_level(m_magnitude_pin + 1, 255 - magnitude);
#endif
  } else {
    pwm_set_gpio_level(m_magnitude_pin, magnitude >> 8);
  }
}
