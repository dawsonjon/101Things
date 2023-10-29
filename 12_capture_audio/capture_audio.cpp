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

  ADCAudio audio_input(16, 20000*16);
  while (true) {
    uint16_t *samples;
    uint16_t new_samples[64];
    audio_input.input_samples(samples);
    for(uint16_t j=0; j<64; j++){
      int16_t acc = 0;
      for(uint16_t i=0; i<16; i++){
        acc += samples[j*16+i];
      } 
      new_samples[j] = acc;
    }
    fwrite("AAAACCCC", 1, 8, stdout); 
    fwrite(new_samples, 2, 64, stdout); 
  }
}
