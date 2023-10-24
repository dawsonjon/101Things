#include <stdio.h>

#include "ADCAudio.h"
#include "PWMAudio.h"
#include "pico/stdlib.h"
#ifdef PICO_W
#include "pico/cyw43_arch.h"
#endif

int main() {
  stdio_init_all();

  //configure SMPS into power save mode
  //const uint8_t PSU_PIN = 23;
  //gpio_init(PSU_PIN);
  //gpio_set_dir(PSU_PIN, GPIO_OUT);
  //gpio_put(PSU_PIN, 0);

#if PICO_W
  cyw43_arch_init();
  cyw43_arch_gpio_put(1, 1);
#endif


  ADCAudio audio_input(16, 10000);
  PWMAudio audio_output(0, 10000);
  while (true) {
    uint16_t *samples;
    audio_input.input_samples(samples);
    audio_output.output_samples(samples, 1024);
  }

  return 0;
}
