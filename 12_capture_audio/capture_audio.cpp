#include <stdio.h>

#include "ADCAudio.h"
#include "pico/stdlib.h"
//#include "pico/cyw43_arch.h"
#include <math.h>

int main() {
  stdio_init_all();
  stdio_set_translate_crlf(&stdio_usb, false);

//configure SMPS into power save mode
#if PICO_W
  cyw43_arch_init();
  cyw43_arch_gpio_put(1, 1);
#else
  const uint8_t PSU_PIN = 23;
  gpio_init(23);
  gpio_set_dir(23, GPIO_OUT);
  gpio_put(23, 1);
#endif

  ADCAudio audio_input(16, 20000);
  while (true) {
    uint16_t *samples;
    audio_input.input_samples(samples);
    fwrite("AAAACCCC", 1, 8, stdout); 
    fwrite(samples, 2, 1024, stdout); 
  }
}
