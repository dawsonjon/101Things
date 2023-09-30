#include <stdio.h>

#include "pico/stdlib.h"
#include "PWMAudio.h"
#include "ADCAudio.h"


int main() {
    stdio_init_all();
   
    ADCAudio audioInput(16, 10000);
    PWMAudio audioOutput(0, 10000);
    while(true)
    {
        uint16_t *samples;
        audioInput.InputSamples(samples);
        audioOutput.OutputSamples(samples, 1024);
   }

    return 0;
}
