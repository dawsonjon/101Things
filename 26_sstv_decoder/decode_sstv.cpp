#include <cstdint>
#include <algorithm>
#include <cmath>
#include "decode_sstv.h"
#include "cordic.h"

#include <iostream>

//from the sample number work out the colour and x/y coordinates
void c_sstv_decoder :: sample_to_pixel(uint16_t &x, uint16_t &y, uint8_t &colour, uint32_t image_sample)
{
  //martin and scottie colour order is g-b-r, map to r-g-b
  static const uint8_t colourmap[4] = {1, 2, 0, 3};

  if( sstv_mode.mode == martin )
  {

    y = image_sample/mean_samples_per_line;
    image_sample -= y*mean_samples_per_line;
    colour = image_sample/samples_per_colour_line;
    image_sample -= colour*samples_per_colour_line;
    colour = colourmap[colour];
    x = image_sample/samples_per_pixel;

  }

  else if( sstv_mode.mode == scottie )
  {

    y = image_sample/mean_samples_per_line;
    image_sample -= y*mean_samples_per_line;
    colour = image_sample/samples_per_colour_line;
    image_sample -= colour*samples_per_colour_line;

    //hsync is between blue and red component (not at end of line)
    //for red component, subtract the length of the scan-line
    if( colour == 2 )
    {
      if(image_sample > samples_per_hsync)
      {
        image_sample -= samples_per_hsync;
      }
      else
      {
        //return colour 3 for non-displayable pixels (e.g. during hsync)
        x = 0; y=0; colour=3;
        return;
      }
    }

    colour = colourmap[colour];
    x = image_sample/samples_per_pixel;

  }

}

void c_sstv_decoder :: decode_vis(uint16_t vis, s_sstv_mode & sstv_mode)
{

  if((vis & 0x30) == 0x20)
  {
    std::cerr << "martin";
    sstv_mode.mode = martin;
    sstv_mode.hsync_pulse_ms = 4.862;
    sstv_mode.colour_gap_ms = 0.572;
  }
  else if((vis & 0x30) == 0x30)
  {
    std::cerr << "scottie";
    sstv_mode.mode = scottie;
    sstv_mode.hsync_pulse_ms = 9;
    sstv_mode.colour_gap_ms = 1.5;
  }
  else
  {
    sstv_mode.mode = martin;
    sstv_mode.hsync_pulse_ms = 4.862;
    sstv_mode.colour_gap_ms = 0.572;
  }

  if (vis & 0x8)
  {
    sstv_mode.height = 256;
  }
  else
  {
    sstv_mode.height = 128;
  }

  if (vis & 0x4)
  {
    sstv_mode.width = 320;
    if(sstv_mode.mode == scottie)
    {
      sstv_mode.colour_time_ms = 138.240;
    }
    else
    {
      sstv_mode.colour_time_ms = 146.342;
    }
  }
  else
  {
    sstv_mode.width = 160;
    if(sstv_mode.mode == scottie)
    {
      sstv_mode.colour_time_ms = 88.064;
    }
    else
    {
      sstv_mode.colour_time_ms = 73.216;
    }
  }

  std::cerr << " " << sstv_mode.width << " " << sstv_mode.height << std::endl;

}

uint8_t c_sstv_decoder :: frequency_to_brightness(uint16_t x)
{
  int16_t brightness = (256*(x-1500))/(2300-1500);
  return std::min(std::max(brightness, (int16_t)0), (int16_t)255);
}

bool parity_check(uint8_t x)
{
  x ^= x >> 4;
  x ^= x >> 2;
  x ^= x >> 1;
  return (~x) & 1;
}

c_sstv_decoder :: c_sstv_decoder()
{
  cordic_init();
}

bool c_sstv_decoder :: decode_audio(int16_t audio, uint16_t &pixel_y, uint16_t &pixel_x, uint8_t &pixel_colour, uint8_t &pixel, int16_t &frequency)
{
    // shift frequency by +FS/4
    //       __|__
    //   ___/  |  \___
    //         |
    //   <-----+----->

    //        | ____
    //  ______|/    \
    //        |
    //  <-----+----->

    // filter -Fs/4 to +Fs/4

    //        | __
    //  ______|/  \__
    //        |
    //  <-----+----->

    ssb_phase = (ssb_phase + 1) & 3u;
    audio = audio >> 1;

    const int16_t audio_i[4] = {audio, 0, (int16_t)-audio, 0};
    const int16_t audio_q[4] = {0, (int16_t)-audio, 0, audio};
    int16_t ii = audio_i[ssb_phase];
    int16_t qq = audio_q[ssb_phase];
    ssb_filter.filter(ii, qq);

    // shift frequency by -FS/4
    //         | __
    //   ______|/  \__
    //         |
    //   <-----+----->

    //     __ |
    //  __/  \|______
    //        |
    //  <-----+----->

    const int16_t sample_i[4] = {(int16_t)-qq, (int16_t)-ii, qq, ii};
    const int16_t sample_q[4] = {ii, (int16_t)-qq, (int16_t)-ii, qq};
    int16_t i = sample_i[ssb_phase];
    int16_t q = sample_q[ssb_phase];

    return decode_iq(i, q, pixel_y, pixel_x, pixel_colour, pixel, frequency);
}


bool c_sstv_decoder :: decode_iq(int16_t sample_i, int16_t sample_q, uint16_t &pixel_y, uint16_t &pixel_x, uint8_t &pixel_colour, uint8_t &pixel, int16_t &smoothed_sample_16)
{

  uint16_t magnitude;
  int16_t phase;

  cordic_rectangular_to_polar(sample_i, sample_q, magnitude, phase);
  frequency = last_phase-phase;
  last_phase = phase;

  int16_t sample = (int32_t)frequency*15000>>16;


  static uint32_t smoothed_sample = 0;
  smoothed_sample = ((smoothed_sample << 3) + sample - smoothed_sample) >> 3;
  smoothed_sample_16 = std::min(std::max(smoothed_sample, (uint32_t)1000u), (uint32_t)2500u);

  e_state debug_state;
  return decode(smoothed_sample_16, pixel_y, pixel_x, pixel_colour, pixel, debug_state);

}

bool c_sstv_decoder :: decode(uint16_t sample, uint16_t &pixel_y, uint16_t &pixel_x, uint8_t &pixel_colour, uint8_t &pixel, e_state &debug_state)
{

  static const uint8_t fraction_bits = 9;
  static const uint32_t scale = 1<<fraction_bits;
  static const float Fs = 15000.0;
  static const uint32_t vsync_samples = round(scale*Fs*300.0/1000.0);
  static const uint32_t vsync_gap_samples = round(scale*Fs*10/1000.0);
  static const uint32_t samples_per_bit = round(scale*Fs*30/1000.0);

  bool pixel_complete = false;
  switch(state)
  {

    //VSYNC/HEADER
    //1900Hz for 300ms
    case vsync_1:


      if(sample < 2200 && sample > 1600)
      {
        sync_counter += scale;
      }
      else if(sync_counter > scale)
      {
        sync_counter = sync_counter - scale;
      }
      else
      {
        sync_counter = 0;
      }

      if(sync_counter > 3*vsync_samples/4)
      {
        std::cerr << "vsync_2" << std::endl;
        sync_counter = 0;
        state = vsync_2;
        timeout = (vsync_samples/4+vsync_gap_samples)>>fraction_bits;
      }

      break;

    //1200Hz for 10ms
    case vsync_2:

      if(sample < 1400)
      {
        sync_counter += scale;
      }
      else if(sync_counter > scale)
      {
        sync_counter = sync_counter - scale;
      }
      else
      {
        sync_counter = 0;
      }

      if(sync_counter > vsync_gap_samples/4 || timeout==0)
      {
        std::cerr << "vsync_3" << std::endl;
        timeout = vsync_samples>>fraction_bits;
        sync_counter = 0;
        state = vsync_3;
      }

      timeout--;
      if(timeout == 0)
      {
        std::cerr << "vsync_0" << "timeout" << std::endl;
        sync_counter = 0;
        state = vsync_1;
      }

      break;

    //1900Hz for 300ms
    case vsync_3:

      if( sample < 2100 && sample > 1700)
      {
        sync_counter+=scale;
      }
      else if(sync_counter > scale)
      {
        sync_counter = sync_counter - scale;
      }
      else
      {
        sync_counter = 0;
      }

      timeout--;
      if((sync_counter > 3*vsync_samples/4) || (timeout == 0))
      {
        std::cerr << "vis_start" << std::endl;
        timeout = (vsync_samples/4) >> fraction_bits;
        sync_counter = 0;
        state = vis_start;
      }

      break;

    //VIS identifier
    case vis_start:

      if(sample < 1550)
      {
        sync_counter+=scale;
      }
      else if(sync_counter > scale)
      {
        sync_counter = sync_counter - scale;
      }
      else
      {
        sync_counter = 0;
      }

      timeout--;
      if( (sync_counter > 2) || (timeout == 0))
      {
        vis_sample = 3;
        for(uint8_t idx=0; idx<10; ++idx) vis_accumulator[idx] = 0;
        state = vis_get_bit;
      }

      break;

    case vis_get_bit:
      {
        uint16_t bit = vis_sample/samples_per_bit;
        if( vis_sample >= (10*samples_per_bit))
        {
          //calculate vis from accumulation over whole bit period
          //start and stop bit contain mid way value, use this as threshold
          //so that we can tollerate slightly wrong frequency
          uint32_t vis_threshold = (vis_accumulator[0]+vis_accumulator[9])/2;
          uint16_t vis=0;
          for(uint8_t idx=1; idx<9; ++idx)
          {
            if(vis_accumulator[idx] < vis_threshold)
            {
              vis |= (1 << (idx-1));
            }
          }
          std::cerr << "parity" << (int)parity_check(vis) << std::endl;

          //configure decoder based on vis code
          decode_vis(vis, sstv_mode);
          samples_per_hsync = scale*Fs*sstv_mode.hsync_pulse_ms/1000.0;
          samples_per_colour_line = scale*Fs*(sstv_mode.colour_time_ms+sstv_mode.colour_gap_ms)/1000.0;
          samples_per_colour_gap = scale*Fs*sstv_mode.colour_gap_ms/1000.0;
          samples_per_pixel = samples_per_colour_line/sstv_mode.width;
          if( sstv_mode.mode == martin)
          {
            samples_per_line = scale*Fs*((sstv_mode.colour_time_ms*3)+(sstv_mode.colour_gap_ms*4)+sstv_mode.hsync_pulse_ms)/1000.0;
          }
          else
          {
            samples_per_line = scale*Fs*((sstv_mode.colour_time_ms*3)+(sstv_mode.colour_gap_ms*3)+sstv_mode.hsync_pulse_ms)/1000.0;
          }
          mean_samples_per_line = samples_per_line;

          sync_counter = 0;
          timeout = samples_per_hsync >> fraction_bits;
          state = wait_rising_edge;
        }
        else
        {
          vis_accumulator[bit] += sample;
          vis_sample += scale;
        }
      }

      break;

    case wait_rising_edge:

      //sync pulse rising edge
      if(sample > 1400)
      {
        sync_counter+=scale;
      }
      else if(sync_counter > scale)
      {
        sync_counter = sync_counter - scale;
      }
      else
      {
        sync_counter = 0;
      }

      //timeout--;
      if((sync_counter > (samples_per_colour_gap/2)) || timeout == 0)
      {
        state = decode_line;
        pixel_accumulator = 0;
        pixel_n = 0;
        last_x = 0;
        image_sample = 0;
        last_sample = 0;
        confirmed_sync_sample = UINT32_MAX;
      }

      break;

    case decode_line:
    {

      uint16_t x, y;
      uint8_t colour;
      sample_to_pixel(x, y, colour, image_sample);

      if(x != last_x && colour < 3)
      {
        //output pixel
        pixel_complete = true;
        pixel = pixel_accumulator/pixel_n;
        pixel_y = y;
        pixel_x = last_x;
        pixel_colour = colour;

        //reset accumulator for next pixel
        pixel_accumulator = 0;
        pixel_n = 0;
        last_x = x;
      }

      //end of image
      if(y == sstv_mode.height)
      {
        std::cerr << "vsync_1" << std::endl;
        state = vsync_1;
        sync_counter = 0;
        break;
      }

      //auto slant correction
      if(sample < 1300 && last_sample > 1300)
      {
        //confirm sync if close to expected time
        if( (image_sample-last_hsync_sample) > (samples_per_line * 99/100) && (image_sample-last_hsync_sample) < (samples_per_line * 101/100))
        {
          if(confirmed_sync_sample == UINT32_MAX) 
          {
            confirmed_sync_sample = image_sample;
          }
          else
          {
            const uint32_t samples_since_confirmed = image_sample-confirmed_sync_sample;
            const uint16_t num_lines = round(1.0*samples_since_confirmed/samples_per_line);
            //std::cerr << (image_sample - last_hsync_sample)/scale << " " << 1.0*samples_per_line/scale << " " << num_lines << " " << 1.0*samples_since_confirmed/(num_lines*scale) << std::endl;
            mean_samples_per_line = mean_samples_per_line - (mean_samples_per_line >> 2) + ((samples_since_confirmed/num_lines) >> 2);
          }
        }
        last_hsync_sample = image_sample;
      }

      //colour pixels
      pixel_accumulator += frequency_to_brightness(sample);
      pixel_n++;
      image_sample+=scale;

      break;

    }
  }

  last_sample = sample;
  debug_state = state;
  return pixel_complete;

}
