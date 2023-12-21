//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: waveform_test.cpp
// description:
// License: MIT
//

#include <cstdio>
#include <cstdint>
#include <cmath>

int main()
{
    const double frequency_Hz = 1e6;

    // calculate some constants
    const double normalised_frequency = frequency_Hz / 125e6;
    const double period_clocks = 125e6 / frequency_Hz;

    // this gives a maximum period of 256 clocks and minimum of 2, that 488.28125kHz to 62.5MHz
    static const uint16_t waveform_length_bits = 256u;
    static const uint16_t waveform_length_words = waveform_length_bits / 32;
    const uint32_t fraction_bits = 24u;

    // Each 256 bit waveform will not necessarily be a whole number of cycles,
    // we will probably be partway through a cycle at the end.
    // This constant tells us how many clocks (and part clocks) we were through the last cycle
    // at the end of a 256-bit waveform.
    // Next time, we choose a waveform that has been advanced by this many clocks
    // so that we pick up where we left off (to the nearest clock).
    // The fraction part is accumulated so that the any rounding error we introduce can be corrected for later.
    // 8 integer bits allow up to 256 whole clocks of advancement, the 24 remaining bits are fraction bits.
    const int32_t index_increment_f24 = round(fmod(waveform_length_bits, period_clocks) * pow(2.0, fraction_bits));

    //we may want to adjust the phase on-the fly.
    //An 8-bit phase adjustment will have steps of period/128 i.e. -period/128 +period/127
    const int32_t phase_step_clocks_f24 = (period_clocks/128.0) * pow(2.0, fraction_bits);

    // store 32 waveforms
    // 32 copies of the waveform are stored, each advanced by 1 clock more than the last
    // advancements of up to 31 bits are achieved by selecting the appropriately advanced waveform.
    // advancements of 32 bits or more can be achieved by advancing whole words through each waveform.
    static uint32_t buffer[32 * waveform_length_words * 2];

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

    uint32_t index_f24 = 0u;

    for(uint8_t j=0; j<32; j++)
    {
      // plan next 32 transfers while the last 32 are sending 
      for (uint8_t transfer = 0u; transfer < 32u; ++transfer) { 
          uint32_t phase_dither_f24 = rand() >> 8; //up to 1 clock 
          uint16_t index = index_f24 >> fraction_bits; 
          // The lower 5 bits represent up to 31 bits of advance 
          // The upper 3 bits represent the number of whole words of advance 
          // we need to swap these because the words of each waveform are contiguous 
          index = ((index & 0x1f) << 4) | ((index 0xe0) >> 4); 

          printf("index: %u %u %u %u\n", j, index, index&0xf, (index>>4 & 0x1f));

          uint16_t num = 0; 
          for(uint8_t word=0; word<8; word++)
          {
              uint32_t lword = buffer[index+word]; 
              for(uint8_t bit=0; bit<32; bit++)
              {
                //printf("%u %u\n", num++, (lword >> bit) & 1);
              }

          }

          index_f24 += index_increment_f24; 
      } 
    }



}
