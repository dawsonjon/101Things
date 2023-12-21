//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2023
// filename: transmitter.cpp
// description:
// License: MIT
//

#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"
#include <cmath>
#include <stdio.h>

#include "modulator.h"
#include "stream_bits.pio.h"

static int16_t sin_table[1024];
class signal_generator {
private:
  uint32_t phase = 0;

public:
  signal_generator() {
    // initialise lookup table
    for (uint16_t idx = 0; idx < 1024; idx++) {
      sin_table[idx] = 32767 * sin(2 * M_PI * idx / 1024);
    }
  }

  int16_t get_sample(uint32_t frequency_steps) {
    int16_t sample = sin_table[phase >> 22];
    phase += frequency_steps;
    return sample;
  }

  uint32_t frequency_steps(double normalized_frequency) {
    return pow(2.0, 32.0) * normalized_frequency;
  }
};

class adc {
private:
  int32_t dc = 0;

public:
  adc(const uint8_t mic_pin, const uint8_t adc_input) {
    // configure ADC
    adc_init();
    adc_gpio_init(mic_pin);
    adc_select_input(adc_input);
  }

  int16_t get_sample() {
    int32_t adc_audio = adc_read();
    dc = dc + ((adc_audio - dc) >> 1);
    return (adc_audio - dc);
  }
};

class nco {
private:
  PIO pio = pio0;
  uint32_t nco_dma, chain_dma, sm;
  dma_channel_config nco_dma_cfg;
  dma_channel_config chain_dma_cfg;
  uint32_t index_f24 = 0u;
  uint8_t ping_pong = 0u;
  uint32_t interrupts;
  bool dma_started = false;
  static const uint8_t waveforms_per_sample = 1u;
  static const uint8_t bits_per_word = 32u;
  static const uint16_t waveform_length_bits = 256u;
  static const uint16_t waveform_length_words =
      waveform_length_bits / bits_per_word;
  static const uint32_t fraction_bits = 24u;
  int32_t index_increment_f24;
  int32_t wrap_f24;
  int32_t phase_step_clocks_f24;
  uint32_t buffer[bits_per_word * waveform_length_words * 2]
      __attribute__((aligned(4)));
  const uint32_t *buffer_addresses[2][waveforms_per_sample + 1];

  void initialise_waveform_buffer(uint32_t buffer[],
                                  uint32_t waveform_length_words,
                                  double normalised_frequency) {
    // Uses floating-point arithmetic and trig functions.
    // Not very fast but doesn't matter because it only runs once.
    for (uint8_t advance = 0u; advance < bits_per_word; ++advance) {
      uint32_t sample_number = advance;
      for (uint16_t word = 0; word < waveform_length_words * 2; ++word) {
        uint32_t bit_samples = 0;
        for (uint8_t bit = 0; bit < bits_per_word; ++bit) {
          double sample =
              sin(2 * M_PI * normalised_frequency * sample_number++);
          // could apply dithering here to remove harmonics
          // i.e. sample += (((double)rand()/(double)RANDMAX) - 0.5) * 2.0
          // //random number between -1 and +1
          if (sample > 0) {
            bit_samples |= (1 << bit);
          }
        }
        buffer[(advance * waveform_length_words * 2) + word] = bit_samples;
      }
    }
  }

public:
  nco(const uint8_t rf_pin, double frequency_Hz) {
    // calculate some constants
    const double normalised_frequency = frequency_Hz / 125e6;
    const double period_clocks = 125e6 / frequency_Hz;

    // Each 256 bit waveform will not necessarily be a whole number of cycles,
    // we will probably be partway through a cycle at the end.
    // This constant tells us how many clocks (and part clocks) we were through
    // the last cycle at the end of a 256-bit waveform. Next time, we choose a
    // waveform that has been advanced by this many clocks so that we pick up
    // where we left off (to the nearest clock). The fraction part is
    // accumulated so that the any rounding error we introduce can be corrected
    // for later. 8 integer bits allow up to 256 whole clocks of advancement,
    // the 24 remaining bits are fraction bits.
    index_increment_f24 = round(fmod(waveform_length_bits, period_clocks) *
                                pow(2.0, fraction_bits));
    wrap_f24 = period_clocks * pow(2.0, fraction_bits);

    // we may want to adjust the phase on-the fly.
    // A 16-bit phase adjustment will have steps of period/32768 i.e.
    // -period/32768 +period/32767
    phase_step_clocks_f24 = (period_clocks / 32768.0) * pow(2.0, fraction_bits);

    // store 32 waveforms
    // 32 copies of the waveform are stored, each advanced by 1 clock more than
    // the last advancements of up to 31 bits are achieved by selecting the
    // appropriately advanced waveform. advancements of 32 bits or more can be
    // achieved by advancing whole words through each waveform.
    initialise_waveform_buffer(buffer, waveform_length_words,
                               normalised_frequency);

    // The PIO contains a very simple program that reads a 32-bit word
    // from the FIFO and sends 1 bit per clock to an IO pin
    uint offset = pio_add_program(pio, &stream_bits_program);
    sm = pio_claim_unused_sm(pio, true);
    stream_bits_program_init(pio, sm, offset, rf_pin); // GPIO0

    // 2 DMA channels are used.
    // 1 transfers the pre-generated blocks of 256-bits to PIO
    // The second DMA configures the first from a table of start addresses
    nco_dma = dma_claim_unused_channel(true);
    chain_dma = dma_claim_unused_channel(true);

    // configure DMA from memory to PIO TX FIFO
    nco_dma_cfg = dma_channel_get_default_config(nco_dma);
    channel_config_set_transfer_data_size(&nco_dma_cfg, DMA_SIZE_32);
    channel_config_set_read_increment(&nco_dma_cfg, true);
    channel_config_set_write_increment(&nco_dma_cfg, false);
    channel_config_set_dreq(&nco_dma_cfg, pio_get_dreq(pio, sm, true));
    channel_config_set_chain_to(&nco_dma_cfg, chain_dma);
    channel_config_set_irq_quiet(&nco_dma_cfg,
                                 true); // don't generate interrupt for each
                                        // transfer, only on NULL transfer
    dma_channel_configure(nco_dma, &nco_dma_cfg, &pio->txf[sm], NULL, 8,
                          false); // 8x32bit transfers

    // use a second DMA to reconfigure the first
    chain_dma_cfg = dma_channel_get_default_config(chain_dma);
    channel_config_set_transfer_data_size(&chain_dma_cfg, DMA_SIZE_32);
    channel_config_set_read_increment(
        &chain_dma_cfg, true); // works through a list of transfers
    channel_config_set_write_increment(
        &chain_dma_cfg, false); // always writes to data DMA read address
    dma_channel_start(nco_dma);

    buffer_addresses[0][waveforms_per_sample] =
        NULL; // null transfer at the end of each 32 address block
    buffer_addresses[1][waveforms_per_sample] = NULL;
  }

  double get_sample_frequency_Hz() {
    return 125e6 / (waveform_length_bits * waveforms_per_sample);
  }

  void output_sample(int16_t phase) {

    // plan next 32 transfers while the last 32 are sending
    gpio_put(1, 1);
    for (uint8_t transfer = 0u; transfer < waveforms_per_sample; ++transfer) {
      uint32_t phase_modulation_f24 = (phase_step_clocks_f24 * phase);
      // uint32_t phase_dither_f24 = rand() >> 8; //up to 1 clock
      uint16_t index = (index_f24 + phase_modulation_f24) >> fraction_bits;
      // The lower 5 bits represent up to 31 bits of advance
      // The upper 3 bits represent the number of whole words of advance
      // we need to swap these because the words of each waveform are contiguous
      index = ((index & 0x1f) << 4) | ((index & 0xe0) >> 5);
      buffer_addresses[ping_pong][transfer] = &buffer[index];
      index_f24 += index_increment_f24;
      if (index_f24 > wrap_f24) {
        index_f24 -= wrap_f24;
      }
    }
    gpio_put(1, 0);

    // check for PIO stalls
    // if(pio->fdebug)
    //{
    //   printf("pio stall, potential lost samples debug: %x\n", pio->fdebug);
    //   pio->fdebug = 0xffffffff;//clear all
    // }

    // if a dma is running, wait until it completes
    interrupts = save_and_disable_interrupts();
    if (dma_started) {
      while (!(dma_hw->intr & 1u << nco_dma)) {
        tight_loop_contents();
      }
      dma_hw->ints0 = 1u << nco_dma;
    }

    ///////////////////////////////////////////////////////////////////////
    // PIO FIFO has 8 entries, so need to kick off next DMA within 32 x 8 = 256
    // clocks of the last one finishing
    // don't put anything between here and next DMA
    // don't put new code here!!
    ///////////////////////////////////////////////////////////////////////

    // chain nco DMA
    dma_channel_configure(
        chain_dma, &chain_dma_cfg,
        &dma_hw->ch[nco_dma]
             .al3_read_addr_trig, // overwrite nco_dma read address and trigger
                                  // new transfer
        &buffer_addresses[ping_pong][0], // increments each time
        1,                               // Halt after 1 transfer
        true                             // start now
    );

    restore_interrupts(interrupts);
    ping_pong ^= 1;
    dma_started = true;
  }
};

void transmitter_start(double frequency_Hz) {
  const bool enable_test_tone = false;
  const uint8_t mic_pin = 28;
  const uint8_t magnitude_pin = 6;
  const uint8_t rf_pin = 8;

  // Use ADC to capture MIC input
  adc mic_adc(mic_pin, 2);

  // Use PIO to output phase/frequency controlled oscillator
  nco rf_nco(rf_pin, frequency_Hz);

  // test signal
  signal_generator test_tone1, test_tone2;
  uint32_t f1 =
      test_tone1.frequency_steps(0.7e3 / rf_nco.get_sample_frequency_Hz());
  uint32_t f2 =
      test_tone2.frequency_steps(1.9e3 / rf_nco.get_sample_frequency_Hz());

  // create modulator
  modulator audio_modulator;
  const double fm_deviation_Hz = 75e3;
  const uint32_t fm_deviation_f15 =
      round(32768.0 * fm_deviation_Hz / rf_nco.get_sample_frequency_Hz());
  int16_t audio;
  uint16_t magnitude;
  int16_t phase;
  int16_t i; // not used in this design
  int16_t q; // not used in this design

  // use gpio for debug
  uint8_t debug_pin = 1;
  gpio_init(debug_pin);
  gpio_set_dir(debug_pin, GPIO_OUT);

  while (1) // Loop iterates at 125e6/(256*32) = 15259 Hz
  {
    // get a sample to transmit
    if (enable_test_tone) {
      audio = test_tone1.get_sample(f1) / 2;
      audio += test_tone2.get_sample(f2) / 2;
    } else {
      audio = mic_adc.get_sample() * 96;
    }

    // demodulate
    audio_modulator.process_sample(audio, magnitude, phase, fm_deviation_f15);

    // output phase
    rf_nco.output_sample(phase);
  }
}

int main() {
  stdio_init_all();
  stdio_set_translate_crlf(&stdio_usb, false);

  printf("starting\n");
  transmitter_start(88e6 / 3);
}
