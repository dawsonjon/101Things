#include <stdio.h>

#include "ADCAudio.h"
#include "PWMAudio.h"
#include "effects.h"
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
  gpio_init(23);
  gpio_set_dir(23, GPIO_OUT);
  gpio_put(23, 1);
#endif

  ADCAudio audio_input(16, 10000);
  PWMAudio audio_output(0, 10000);
  effects e; 
  e.initialise();
  while (true) {
    uint16_t *samples;
    audio_input.input_samples(samples);
    for(uint16_t idx = 0; idx<1024; idx++)
    {
      int16_t sample = samples[idx]-32768;
      e.process_sample(sample);
      samples[idx] = (sample+32768) >> 4;
    }
    audio_output.output_samples(samples, 1024);
  }
}
