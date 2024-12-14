#ifndef __SSTV_DECODER_H__
#define __SSTV_DECODER_H__

#include "half_band_filter2.h"

enum e_mode 
{
  martin, 
  scottie, 
  unsupported
};

enum e_state
{
  vsync_1,
  vsync_2,
  vsync_3,
  vis_start,
  vis_get_bit,
  wait_rising_edge,
  decode_line,
  wait,
};

struct s_sstv_mode 
{
  e_mode mode = unsupported;
  uint16_t width = 320;
  uint16_t height = 256;
  float colour_time_ms=0.0;
  float colour_gap_ms=0.0;
  float hsync_pulse_ms=0.0;
};

class c_sstv_decoder
{

  private:

  s_sstv_mode sstv_mode;
  uint32_t sync_counter = 0;
  uint16_t y_pixel = 0;
  uint16_t last_x = 0;
  uint32_t image_sample = 0;
  uint16_t last_sample = 0;
  uint32_t last_hsync_sample = 0;
  uint32_t confirmed_sync_sample = 0;
  e_state state = vsync_1;
  void decode_vis(uint16_t vis, s_sstv_mode & sstv_mode);
  void sample_to_pixel(uint16_t &x, uint16_t &y, uint8_t &colour, uint32_t image_sample);
  uint8_t frequency_to_brightness(uint16_t x);
  uint32_t samples_per_hsync;
  uint32_t samples_per_line;
  uint32_t mean_samples_per_line;
  uint32_t samples_per_colour_line;
  uint32_t samples_per_colour_gap;
  uint32_t samples_per_pixel;
  uint32_t timeout;
  uint32_t vis_sample;
  uint32_t vis_accumulator[10];
  uint32_t pixel_accumulator;
  uint16_t pixel_n;
  int16_t last_phase = 0;
  uint8_t ssb_phase = 0;
  half_band_filter2 ssb_filter;
  int16_t frequency;

  public:
  c_sstv_decoder();
  bool decode(uint16_t sample, uint16_t &line, uint16_t &col, uint8_t &colour, uint8_t &pixel, e_state &debug_state);
  bool decode_iq(int16_t sample_i, int16_t sample_q, uint16_t &pixel_y, uint16_t &pixel_x, uint8_t &pixel_colour, uint8_t &pixel, int16_t &frequency);
  bool decode_audio(int16_t audio, uint16_t &pixel_y, uint16_t &pixel_x, uint8_t &pixel_colour, uint8_t &pixel, int16_t &frequency);
  
};

#endif
