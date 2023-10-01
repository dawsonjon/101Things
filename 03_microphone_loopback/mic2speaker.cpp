#include <stdio.h>

#include "ADCAudio.h"
#include "PWMAudio.h"
#include "pico/stdlib.h"

int main() {
  stdio_init_all();

  ADCAudio audio_input(16, 10000);
  PWMAudio audio_output(0, 10000);
  while (true) {
    uint16_t *samples;
    audio_input.input_samples(samples);
    audio_output.output_samples(samples, 1024);
  }

  return 0;
}
