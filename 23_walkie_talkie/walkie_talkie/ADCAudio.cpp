#include "ADCAudio.h"
#include <stdio.h>

#include "Arduino.h"

//extern "C" void dma_irq_handler_wrapper();

extern "C" void dma_irq_handler_wrapper() {
  ADCAudio::dma_irq_handler();
}

ADCAudio *ADCAudio::instance = nullptr;

void ADCAudio::begin(const uint8_t audio_pin,
                     const uint32_t audio_sample_rate) {

  instance = this;

  adc_init();
  adc_gpio_init(audio_pin);
  const uint32_t usb_clock_frequency = 48000000;
  adc_set_clkdiv((usb_clock_frequency / audio_sample_rate) - 1);

  adc_dma = dma_claim_unused_channel(true);
  cfg = dma_channel_get_default_config(adc_dma);

  channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
  channel_config_set_read_increment(&cfg, false);
  channel_config_set_write_increment(&cfg, true);
  channel_config_set_dreq(&cfg, DREQ_ADC);

  adc_select_input(0);
  hw_clear_bits(&adc_hw->fcs, ADC_FCS_UNDER_BITS);
  hw_clear_bits(&adc_hw->fcs, ADC_FCS_OVER_BITS);
  adc_fifo_setup(true, true, 1, false, false);
  adc_run(true);

  // DMA IRQ setup
  dma_channel_set_irq1_enabled(adc_dma, true);
  irq_set_exclusive_handler(DMA_IRQ_1, dma_irq_handler_wrapper);
  irq_set_enabled(DMA_IRQ_1, true);

  // Start first transfer into ping
  ping_running = true;
  buffer_ready = false;
  last_completed = nullptr;

  dma_channel_configure(
      adc_dma, &cfg,
      ping,
      &adc_hw->fifo,
      ADC_BUFFER_SIZE,
      true);
}

void ADCAudio::dma_irq_handler() {

  dma_hw->ints0 = 1u << instance->adc_dma;
  gpio_put(0, instance->ping_running);

  instance->overflow = instance->buffer_ready;

  if (instance->ping_running) {
    // Ping just finished
    instance->last_completed = instance->ping;
    instance->ping_running = false;

    // Start pong
    dma_channel_configure(
        instance->adc_dma,
        &instance->cfg,
        instance->pong,
        &adc_hw->fifo,
        ADC_BUFFER_SIZE,
        true);
  } else {
    // Pong just finished
    instance->last_completed = instance->pong;
    instance->ping_running = true;

    // Start ping
    dma_channel_configure(
        instance->adc_dma,
        &instance->cfg,
        instance->ping,
        &adc_hw->fifo,
        ADC_BUFFER_SIZE,
        true);
  }

  instance->buffer_ready = true;
}

void ADCAudio::input_samples(uint16_t *&samples) {
  if (buffer_ready) {
    samples = (uint16_t *)last_completed;
    buffer_ready = false;   // consume it
  } else {
    samples = nullptr;      // nothing new yet
  }
}