#ifndef PWM_AUDIO_H
#define PWM_AUDIO_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/dma.h"



class PWMAudio
{

    public:
    PWMAudio(int audio_pin, int audio_sample_rate);
    void OutputSamples(uint16_t samples[], uint16_t len);

    private:
    int pwm_dma;
    dma_channel_config audio_cfg;
    int audio_pwm_slice_num;

};

#endif
