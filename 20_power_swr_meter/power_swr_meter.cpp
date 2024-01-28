//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2023
// filename: transmitter.cpp
// description: RF Power and SWR Meter
// License: MIT
//

#include "font.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "psu_mode.h"
#include "ssd1306.h"
#include <cmath>
#include <stdio.h>

// measured value of ADC voltage reference
const float vref = 3.252f;

// o---|>|-------+
//               |
//              [r1]
//               |
//               +-------o ADC
//               |
//              [r2]
//               |
//               o gnd

// voltage divider
const float r1 = 10e3;
const float r2 = 10e3;
const float adc_scale = (r1 + r2) / r2;
const float diode_drop_volts = 0.33;
const float turns_ratio = 10.0;

float read_adc_volts(uint8_t channel) {
  adc_select_input(channel);
  return (vref * adc_read()) / 4096.0f;
}

int main() {
  stdio_init_all();
  stdio_set_translate_crlf(&stdio_usb, false);

  // reduces noise in ADC measurements
  disable_power_save();

  // configure ADC
  uint8_t fwd_voltage_pin = 26;
  uint8_t rev_voltage_pin = 27;
  uint8_t fwd_adc = 0;
  uint8_t rev_adc = 1;
  uint8_t batt_adc = 3;
  adc_init();
  adc_gpio_init(fwd_voltage_pin);
  adc_gpio_init(rev_voltage_pin);

  // configure display
  i2c_init(i2c1, 400000);
  const uint8_t PIN_DISPLAY_SDA = 18;
  const uint8_t PIN_DISPLAY_SCL = 19;
  gpio_set_function(PIN_DISPLAY_SDA, GPIO_FUNC_I2C);
  gpio_set_function(PIN_DISPLAY_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(PIN_DISPLAY_SDA);
  gpio_pull_up(PIN_DISPLAY_SCL);
  ssd1306_t disp;
  disp.external_vcc = false;
  ssd1306_init(&disp, 128, 64, 0x3C, i2c1);

  float fwd_adc_volts = 0.0f;
  float rev_adc_volts = 0.0f;
  float batt_adc_volts = 0.0f;

  // Measure fwd power using DMM at 2 power levels, compared with reported value
  // calculate gradient and intercept
  //  y      x
  // Vfwd  Vfwd_adc
  // 1.583 1.572
  // 0.831 0.838

  const float y1 = 0.831; // measured values
  const float y2 = 1.583;
  const float x1 = 0.838; // reported values
  const float x2 = 1.572;

  const float m = (y2 - y1) / (x2 - x1);
  const float c = y1 - (m * x1);

  while (1) {
    // smoothing filter
    fwd_adc_volts = (0.9 * fwd_adc_volts) + (0.1 * read_adc_volts(fwd_adc));
    rev_adc_volts = (0.9 * rev_adc_volts) + (0.1 * read_adc_volts(rev_adc));
    batt_adc_volts = (0.9 * batt_adc_volts) + (0.1 * read_adc_volts(batt_adc));

    // apply calibration correction and scaling
    const float fwd_corrected_peak_voltage =
        m * (fwd_adc_volts * adc_scale) + c;
    const float rev_corrected_peak_voltage =
        m * (rev_adc_volts * adc_scale) + c;

    // convert ADC voltage to RF pk-pk voltage
    const float fwd_pk_pk_volts =
        (fwd_corrected_peak_voltage + diode_drop_volts) * turns_ratio * 2.0f;
    const float rev_pk_pk_volts =
        (rev_corrected_peak_voltage + diode_drop_volts) * turns_ratio * 2.0f;
    const float batt_volts = 12.0 * batt_adc_volts;

    // convert RF pk-pk voltage into power assuming 50ohms
    const float fwd_power_watts =
        (fwd_adc_volts * adc_scale) > 0.1
            ? (fwd_pk_pk_volts * fwd_pk_pk_volts) / 400.0f
            : 0.0f;
    const float rev_power_watts =
        (rev_adc_volts * adc_scale) > 0.1
            ? (rev_pk_pk_volts * rev_pk_pk_volts) / 400.0f
            : 0.0f;

    // calculate VSWR
    const float reflection_ratio = sqrt(rev_power_watts / fwd_power_watts);
    const float vswr = (1.0f + reflection_ratio) / (1.0f - reflection_ratio);

    ssd1306_clear(&disp);
    char line[23];
    snprintf(line, 22, "FWD  %3.1fW", fwd_power_watts);
    ssd1306_draw_string(&disp, 0, 0, 2, line);
    snprintf(line, 22, "REV  %3.1fW", rev_power_watts);
    ssd1306_draw_string(&disp, 0, 16, 2, line);
    if (fwd_power_watts == 0) {
      snprintf(line, 22, "VSWR ----", vswr);
    } else {
      snprintf(line, 22, "VSWR %3.1f", vswr);
    }
    ssd1306_draw_string(&disp, 0, 32, 2, line);
    snprintf(line, 22, "BATT %3.1fV", batt_volts);
    ssd1306_draw_string(&disp, 0, 48, 2, line);
    ssd1306_show(&disp);
    sleep_us(10000);

    printf("FWD (W): %f REV (W) %f VSWR (%f)\n", fwd_power_watts,
           rev_power_watts, vswr);
    printf("V1 %5.3f V2 %5.3f\n", fwd_adc_volts * adc_scale,
           rev_adc_volts * adc_scale);
  }
}
