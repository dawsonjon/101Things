#ifndef ADC_AUDIO_H
#define ADC_AUDIO_H

#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/dma.h"


class ADCAudio
{

    public:
    ADCAudio(const uint8_t audio_pin, const uint16_t audio_sample_rate);
    void InputSamples(uint16_t*& samples);

    private:
    int adc_dma;
    dma_channel_config cfg;
    uint16_t ping[1024];
    uint16_t pong[1024];
    bool ping_running = true;
};

#endif
