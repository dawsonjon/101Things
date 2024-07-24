#ifndef PWM_AUDIO_H
#define PWM_AUDIO_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/dma.h"



class PWMAudio
{

    public:
    void begin(const int audio_pin, const int audio_sample_rate);
    void output_samples(const uint16_t samples[], const uint16_t len);

    private:
    int pwm_dma;
    dma_channel_config audio_cfg;
    int audio_pwm_slice_num;

};

#endif
