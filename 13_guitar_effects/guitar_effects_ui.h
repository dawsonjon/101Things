#ifndef GUITAR_EFFECTS_UI_H
#define GUITAR_EFFECTS_UI_H

#include "effects.h"
#include "eq.h"
#include "ui.h"

struct s_patch
{
  uint8_t flash_in_use;
  uint8_t preamp_gain;
  uint8_t eq_band1;
  uint8_t eq_band2;
  uint8_t eq_band3;
  uint8_t eq_band4;
  uint8_t eq_band5;
  uint8_t distortion_effect;
  uint8_t distortion_gain;
  float distortion_offset;
  uint8_t delay_effect;
  float delay_ms;
  float delay_feedback;
  float delay_mix;
  uint8_t modulator_effect;
  float flanger_depth_ms;
  float flanger_rate_Hz;
  float flanger_delay_ms;
  float flanger_feedback;
  float chorus1_depth_ms;
  float chorus1_rate_Hz;
  float chorus1_delay_ms;
  float chorus1_feedback;
  float chorus2_depth_ms;
  float chorus2_rate_Hz;
  float chorus2_delay_ms;
  float chorus2_feedback;
  float chorus3_depth_ms;
  float chorus3_rate_Hz;
  float chorus3_delay_ms;
  float chorus3_feedback;
  float tremolo_rate_Hz;
  float tremolo_depth;
  float vibrato_depth_ms;
  float vibrato_rate_Hz;
  float phaser_depth;
  float phaser_rate_Hz;
};

const s_patch default_patch = 
{
  0, //0 means flash is in use
  4, //uint8_t preamp_gain;

  4, //uint8_t eq_band1;
  4, //uint8_t eq_band2;
  4, //uint8_t eq_band3;
  4, //uint8_t eq_band4;
  4, //uint8_t eq_band5;

  0, //uint8_t distortion_effect;
  4, //uint8_t distortion_gain;
  0.0f, //float distortion_offset;

  0, //uint8_t delay_effect;
  100.0f,//float delay_ms;
  0.8f, //float delay_feedback;
  0.1f, //float delay_mix;

  0, //uint8_t modulator_effect;
  1.0f, //float flanger_depth_ms;
  1.0f, //float flanger_rate_Hz;
  1.0f, //float flanger_delay_ms;
  0.5f, //float flanger_feedback;

  2.0f, //float chorus1_depth_ms;
  1.2f, //float chorus1_rate_Hz;
  21.0f, //float chorus1_delay_ms;
  0.5f, //float chorus1_feedback;

  2.0f, //float chorus2_depth_ms;
  1.1f, //float chorus2_rate_Hz;
  20.5f, //float chorus2_delay_ms;
  0.5f, //float chorus2_feedback;

  2.0f, //float chorus3_depth_ms;
  1.0f, //float chorus3_rate_Hz;
  20.0f, //float chorus3_delay_ms;
  0.5f, //float chorus3_feedback;

  20.0f, //float tremolo_rate_Hz;
  0.5f, //float tremolo_depth;

  4.0f, //float vibrato_depth_ms;
  2.0f, //float vibrato_rate_Hz;

  8.0f, //float phaser_depth_ms;
  1.0f, //float phaser_rate_Hz;
};

class guitar_effects_ui
{
  bool show_menu = true;
  uint8_t patch_number=0;
  s_patch patches[4] = {default_patch, default_patch, default_patch, default_patch};
  void patch_2_ui(s_patch & patch);
  void ui_2_patch(s_patch & patch);
  void restore_from_flash();
  void save_to_flash();

  public:
  void init();
  void do_ui(effects & e, s_effect & settings);
};

#endif
