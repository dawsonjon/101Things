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

  const uint8_t oversample = 16;
  ADCAudio audio_input(16, oversample * 20000);
  PWMAudio audio_output(0, 20000);
  effects e; 
  e.initialise();

  uint16_t *input_samples;
  uint8_t ping = 0;
  uint16_t output_samples[2][64];

  while (true) {
    audio_input.input_samples(input_samples);

    uint16_t input_sample_number = 0;
    for(uint16_t output_sample_number = 0; output_sample_number<64; output_sample_number++)
    {
      //sum 16 samples to give 1
      int16_t sample = 0;
      for(uint8_t j = 0; j<oversample; j++) 
      {
          sample += input_samples[input_sample_number++];
      }

      //convert to signed
      sample -= 32768;

      //apply effects
      //e.process_sample(sample);

      //convert to unsigned 8-bit
      output_samples[ping][output_sample_number] = (sample+32768) >> 4;
    }
    audio_output.output_samples(output_samples[ping], 64);
    ping ^= 1;
  }
}
