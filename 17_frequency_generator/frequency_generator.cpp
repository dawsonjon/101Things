//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: frequency_generator.cpp
// description:
// License: MIT
//

#include <stdio.h>
#include <cmath>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/sync.h"

// Our assembled program:
#include "stream_bits.pio.h"

void nco_start(double frequency_Hz) {
    // calculate some constants
    const double normalised_frequency = frequency_Hz / 125e6;
    const double period_clocks = 125e6 / frequency_Hz;
    printf("normalised_frequency %f period_clocks %0.15f\n", normalised_frequency, period_clocks);

    // this gives a maximum period of 256 clocks and minimum of 2, that 488.28125kHz to 62.5MHz
    static const uint16_t waveform_length_bits = 256u;
    static const uint16_t waveform_length_words = waveform_length_bits / 32;
    const uint32_t fraction_bits = 24u;
    printf("waveform_length_bits %u waveform_length_words %u\n", waveform_length_bits, waveform_length_words);

    // Each 256 bit waveform will not necessarily be a whole number of cycles,
    // we will probably be partway through a cycle at the end.
    // This constant tells us how many clocks (and part clocks) we were through the last cycle
    // at the end of a 256-bit waveform.
    // Next time, we choose a waveform that has been advanced by this many clocks
    // so that we pick up where we left off (to the nearest clock).
    // The fraction part is accumulated so that the any rounding error we introduce can be corrected for later.
    // 8 integer bits allow up to 256 whole clocks of advancement, the 24 remaining bits are fraction bits.
    const int32_t index_increment_f24 = round(fmod(waveform_length_bits, period_clocks) * pow(2.0, fraction_bits));
    printf("index_increment: %0.15f\n", fmod(waveform_length_bits, period_clocks));

    //we may want to adjust the phase on-the fly.
    //An 8-bit phase adjustment will have steps of period/256
    //const int32_t phase_step_clocks_f24 = (period_clocks/256.0) * pow(2.0, fraction_bits);

    // store 32 waveforms
    // 32 copies of the waveform are stored, each advanced by 1 clock more than the last
    // advancements of up to 31 bits are achieved by selecting the appropriately advanced waveform.
    // advancements of 32 bits or more can be achieved by advancing whole words through each waveform.
    static uint32_t buffer[32 * waveform_length_words * 2] __attribute__((aligned(4)));

    // Uses floating-point arithmetic and trig functions.
    // Not very fast but doesn't matter because it only runs once.
    for (uint8_t advance = 0u; advance < 32; ++advance) {
        uint32_t sample_number = advance;
        for (uint16_t word = 0; word < waveform_length_words * 2; ++word) {
            uint32_t bit_samples = 0;
            for (uint8_t bit = 0; bit < 32; ++bit) {
                double sample = sin(2 * M_PI * normalised_frequency * sample_number++);
                // could apply dithering here to remove harmonics
                // i.e. sample += (((double)rand()/(double)RANDMAX) - 0.5) * 2.0 //random number between -1 and +1
                if (sample > 0) {
                    bit_samples |= (1 << bit);
                }
            }
            buffer[(advance * waveform_length_words * 2) + word] = bit_samples;
        }
    }

    // The PIO contains a very simple program that reads a 32-bit word
    // from the FIFO and sends 1 bit per clock to an IO pin
    PIO pio = pio0;
    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &stream_bits_program);
    sm = pio_claim_unused_sm(pio, true);
    stream_bits_program_init(pio, sm, offset, 0); // GPIO0

    // 2 DMA channels are used.
    // 1 transfers the pre-generated blocks of 256-bits to PIO
    // The second DMA configures the first from a table of start addresses
    int nco_dma;
    dma_channel_config nco_dma_cfg;
    int chain_dma;
    dma_channel_config chain_dma_cfg;
    nco_dma = dma_claim_unused_channel(true);
    chain_dma = dma_claim_unused_channel(true);

    // configure DMA from memory to PIO TX FIFO
    nco_dma_cfg = dma_channel_get_default_config(nco_dma);
    channel_config_set_transfer_data_size(&nco_dma_cfg, DMA_SIZE_32);
    channel_config_set_read_increment(&nco_dma_cfg, true);
    channel_config_set_write_increment(&nco_dma_cfg, false);
    channel_config_set_dreq(&nco_dma_cfg, pio_get_dreq(pio, sm, true));
    channel_config_set_chain_to(&nco_dma_cfg, chain_dma);
    channel_config_set_irq_quiet(&nco_dma_cfg, true); // don't generate interrupt for each transfer, only on NULL transfer
    dma_channel_configure(nco_dma, &nco_dma_cfg, &pio->txf[sm], NULL, 8, false); // 8x32bit transfers

    // use a second DMA to reconfigure the first
    chain_dma_cfg = dma_channel_get_default_config(chain_dma);
    channel_config_set_transfer_data_size(&chain_dma_cfg, DMA_SIZE_32);
    channel_config_set_read_increment(&chain_dma_cfg, true); // works through a list of transfers
    channel_config_set_write_increment(&chain_dma_cfg, false); // always writes to data DMA read address
    dma_channel_start(nco_dma);

    // Create a table of addresses to send
    const uint32_t* buffer_addresses[2][33];
    uint32_t index_f24 = 0u;
    buffer_addresses[0][32] = NULL; // null transfer at the end of each 32 address block
    buffer_addresses[1][32] = NULL;
    uint8_t ping_pong = 0u;

    // plan first 32 DMA transfers
    for (uint8_t transfer = 0u; transfer < 32u; ++transfer) {
        //index_f24 += (phase_step_clocks_f24 * phase);
        uint16_t index = index_f24 >> fraction_bits;
        // The lower 5 bits represent up to 31 bits of advance
        // The upper 3 bits represent the number of whole words of advance
        // we need to swap these because the words of each waveform are contiguous
        index = ((index << 4) | (index >> 5)) & 0x1ff;
        buffer_addresses[ping_pong][transfer] = &buffer[index];
        index_f24 += index_increment_f24;
    }

    uint8_t pin = 1;
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    uint32_t interrupts = save_and_disable_interrupts();

    // Play back 32, 256-bit chunks of the pre-generated waveform at a time.
    // The CPU calculates the correct delay for each chunk.
    while (1) // Loop iterates at 125e6/(256*32) = 15259 Hz
    {
        // don't put new code here!!
        // second DMA overwrite read address and triggers nco DMA
        dma_channel_configure(
            chain_dma,
            &chain_dma_cfg,
            &dma_hw->ch[nco_dma].al3_read_addr_trig, // overwrite nco_dma read address and trigger new transfer
            &buffer_addresses[ping_pong][0],         // increments each time
            1,                                       // Halt after 1 transfer
            true                                     // start now
        );
        restore_interrupts(interrupts);
        ping_pong ^= 1;

        gpio_put(pin, 1);
        // plan next 32 transfers while the last 32 are sending
        for (uint8_t transfer = 0u; transfer < 32u; ++transfer) {
            //index_f24 += (phase_step_clocks_f24 * phase);
            uint32_t phase_dither = rand() >> 8; //up to 1 clock
            uint16_t index = (index_f24 + phase_dither) >> fraction_bits;
            // The lower 5 bits represent up to 31 bits of advance
            // The upper 3 bits represent the number of whole words of advance
            // we need to swap these because the words of each waveform are contiguous
            index = ((index << 4) | (index >> 5)) & 0x1ff;
            buffer_addresses[ping_pong][transfer] = &buffer[index];
            index_f24 += index_increment_f24;
        }
        gpio_put(pin, 0);

        if(pio->fdebug)
        {
          //check that the PIO never stalls
          printf("debug: %u\n", pio->fdebug);
          pio->fdebug = 0xffffffff;//clear all
        }

        // don't plan any more until a block of 32 completes
        interrupts = save_and_disable_interrupts();
        while (!(dma_hw->intr & 1u << nco_dma))
        {
            tight_loop_contents();
        }
        dma_hw->ints0 = 1u << nco_dma;
        // PIO FIFO has 8 entries, so need to kick off next DMA within 32 x 8 = 256
        // clocks of the last one finishing
        // don't put anything between here and next DMA
    }
}

int main() {
    stdio_init_all();
    stdio_set_translate_crlf(&stdio_usb, false);

    printf("starting\n");
    nco_start(3.5e6);
}

