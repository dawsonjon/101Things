#ifndef ADC_AUDIO_H
#define ADC_AUDIO_H

#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/dma.h"

static const uint16_t ADC_BUFFER_SIZE = 256;

class ADCAudio {
public:
  void begin(uint8_t audio_pin, uint32_t audio_sample_rate);
  void input_samples(uint16_t *&samples);
  static void dma_irq_handler();
  bool get_overflow() { return overflow;}

private:
  int adc_dma;
  dma_channel_config cfg;

  uint16_t ping[ADC_BUFFER_SIZE];
  uint16_t pong[ADC_BUFFER_SIZE];

  volatile bool ping_running = true;
  volatile bool buffer_ready = false;
  volatile bool overflow = false;
  volatile uint16_t *last_completed = nullptr;

  static ADCAudio *instance;
  
};

#endif
