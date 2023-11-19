#include <stdio.h>

#include "ADCAudio.h"
#include "PWMAudio.h"
#include "psu_mode.h"
#include "effects.h"
#include "fixed.h"
#include "eq.h"
#include "ui.h"
#include "pico/stdlib.h"
#include <math.h>

float dB2lin(float x)
{
  return powf(10.0f, x/20.0f);
}

int main() {
  stdio_init_all();
  stdio_set_translate_crlf(&stdio_usb, false);
  init_ui();
  ssd1306_contrast(&disp, 0x01);

  const int32_t eq_gains[] = {
    eq_const(dB2lin(-36)),
    eq_const(dB2lin(-33)),
    eq_const(dB2lin(-30)),
    eq_const(dB2lin(-27)),
    eq_const(dB2lin(-24)),
    eq_const(dB2lin(-21)),
    eq_const(dB2lin(-18)),
    eq_const(dB2lin(-15)),
    eq_const(dB2lin(-12))
  };

  const int32_t gains[] = {
    float2fixed(dB2lin(0)),
    float2fixed(dB2lin(3)),
    float2fixed(dB2lin(6)),
    float2fixed(dB2lin(9)),
    float2fixed(dB2lin(12)),
    float2fixed(dB2lin(15)),
    float2fixed(dB2lin(18)),
    float2fixed(dB2lin(21)),
    float2fixed(dB2lin(24))
  };

  //reduces noise in ADC measurements
  disable_power_save();

  //preamp
  enum_entry preamp_gain("0dB#3dB#6dB#9dB#12dB#15dB#18dB#21dB#24dB", 4, 8);
  menu_item *preamp_menu_items[] = {&preamp_gain};
  menu preamp_menu("Preamp", "Preamp Gain", preamp_menu_items, 0);

  //EQ Menus
  enum_entry eq_band1("-12dB#-9dB#-6dB#-3dB#0dB#3dB#6dB#9dB#12dB", 4, 8);
  enum_entry eq_band2("-12dB#-9dB#-6dB#-3dB#0dB#3dB#6dB#9dB#12dB", 4, 8);
  enum_entry eq_band3("-12dB#-9dB#-6dB#-3dB#0dB#3dB#6dB#9dB#12dB", 4, 8);
  enum_entry eq_band4("-12dB#-9dB#-6dB#-3dB#0dB#3dB#6dB#9dB#12dB", 4, 8);
  enum_entry eq_band5("-12dB#-9dB#-6dB#-3dB#0dB#3dB#6dB#9dB#12dB", 4, 8);
  menu_item *eq_menu_items[] = {&eq_band1, &eq_band2, &eq_band3, &eq_band4, &eq_band5};
  menu eq_menu("EQ", "Band 1#Band 2#Band 3#Band 4#Band 5", eq_menu_items, 4);

  //Distorion Menus
  enum_entry distortion_effect("Off#Cubic#Quadratic#Full Wave#Half Wave#Foldback#Fuzz1#Fuzz2", 0, 7);
  enum_entry distortion_gain("0dB#3dB#6dB#9dB#12dB#15dB#18dB#21dB#24dB", 4, 8);
  float_entry distortion_offset(0.0f, -1.0f, 1.0f, 0.01f);
  menu_item *distortion_menu_items[] = {&distortion_effect, &distortion_gain, &distortion_offset};
  menu distortion_menu("Distortion", "Effect#Gain#Offset", distortion_menu_items, 2);

  //Delay Menus
  enum_entry delay_effect("Off#Delay#Echo", 0, 2);
  float_entry delay_ms(100.0f, 100.0f, 400.0f, 1.0f);
  float_entry delay_feedback(0.8f, 0.0f, 1.0f, 0.1f);
  menu_item *delay_menu_items[] = {&delay_effect, &delay_ms, &delay_feedback};
  menu delay_menu("Delay", "Effect#Delay (ms)#Feedback", delay_menu_items, 2);
  
  //Modulator Menus
  enum_entry modulator_effect("Off#Chorus#Flanger#Tremolo#Vibrato#Pitch Shift", 0, 5);

  float_entry flanger_depth_ms(1.0f, 1.0f, 2.0f, 0.1f);
  float_entry flanger_rate_Hz(1.0f, 0.1f, 5.0f, 0.1f);
  float_entry flanger_delay_ms(1.0f, 1.0f, 5.0f, 0.1f);
  float_entry flanger_feedback(0.5f, 0.0f, 1.0f, 0.1f);
  menu_item *flanger_menu_items[] = {&flanger_depth_ms, &flanger_rate_Hz, &flanger_delay_ms, &flanger_feedback};
  menu flanger_menu("Flanger", "Depth (ms)#Rate (Hz)#Delay (ms)#Feedback", flanger_menu_items, 3);

  float_entry chorus1_depth_ms(2.0f, 1.0f, 2.0f, 0.1f);
  float_entry chorus1_rate_Hz(1.0f, 0.1f, 5.0f, 0.1f);
  float_entry chorus1_delay_ms(20.0f, 20.0f, 50.0f, 0.5f);
  float_entry chorus1_feedback(0.5f, 0.0f, 1.0f, 0.1f);
  float_entry chorus2_depth_ms(2.0f, 1.0f, 2.0f, 0.1f);
  float_entry chorus2_rate_Hz(1.0f, 0.1f, 5.0f, 0.1f);
  float_entry chorus2_delay_ms(20.0f, 20.0f, 50.0f, 0.5f);
  float_entry chorus2_feedback(0.5f, 0.0f, 1.0f, 0.1f);
  float_entry chorus3_depth_ms(2.0f, 1.0f, 2.0f, 0.1f);
  float_entry chorus3_rate_Hz(1.0f, 0.1f, 5.0f, 0.1f);
  float_entry chorus3_delay_ms(20.0f, 20.0f, 50.0f, 0.5f);
  float_entry chorus3_feedback(0.5f, 0.0f, 1.0f, 0.1f);
  menu_item *chorus_menu_items[] = {&chorus1_depth_ms, &chorus1_rate_Hz, &chorus1_delay_ms, &chorus1_feedback, 
                                    &chorus2_depth_ms, &chorus2_rate_Hz, &chorus2_delay_ms, &chorus2_feedback,
                                    &chorus3_depth_ms, &chorus3_rate_Hz, &chorus3_delay_ms, &chorus3_feedback};

  menu chorus_menu("Chorus", "Depth 1 (ms)#Rate 1 (ms)#Delay 1 (ms)#Feedback 1#Depth 2 (ms)#Rate 2 (ms)#Delay 2 (ms)#Feedback 2#Depth 3 (ms)#Rate 3 (ms)#Delay 3 (ms)#Feedback 3", chorus_menu_items, 11);
  float_entry tremolo_rate_Hz(20.0f, 0.1f, 100.0f, 0.1f);
  menu_item *tremolo_menu_items[] = {&tremolo_rate_Hz};
  menu tremolo_menu("Tremolo", "Rate (Hz)", tremolo_menu_items, 0);

  float_entry vibrato_depth_ms(4.0f, 1.0f, 5.0f, 0.1f);
  float_entry vibrato_rate_Hz(2.0f, 0.1f, 5.0f, 0.1f);
  menu_item *vibrato_menu_items[] = {&vibrato_depth_ms, &vibrato_rate_Hz};
  menu vibrato_menu("Vibrato", "Depth (ms)#Rate (Hz)", vibrato_menu_items, 1);

  menu_item *modulator_items[] = {&modulator_effect, &flanger_menu, &chorus_menu, &tremolo_menu, &vibrato_menu};
  menu modulator_menu("Modulator", "Effect#Flanger Settings#Chorus Settings#Tremolo Settings#Vibrato Settings", modulator_items, 4);

  //main menu
  menu_item *menu_items[] = {&preamp_menu, &eq_menu, &distortion_menu, &delay_menu, &modulator_menu};
  menu main_menu("Menu", "Preamp#EQ#Distorion#Delay#Modulator", menu_items, 4);

  const uint8_t oversample = 16;
  ADCAudio audio_input(16, oversample * 20000);
  PWMAudio audio_output(0, 20000);
  effects e; 
  e.initialise();
  s_effect settings;

  uint16_t *input_samples;
  uint8_t ping = 0;
  uint16_t output_samples[2][64];
  bool show_menu = true;

  while (true) {


    //process a block of data
    for(uint8_t i=0; i<20; i++)
    {
      audio_input.input_samples(input_samples);

      uint16_t input_sample_number = 0;
      uint64_t start = time_us_64();
      for(uint16_t output_sample_number = 0; output_sample_number<64; output_sample_number++)
      {
        //sum 16 samples to give 1
        int16_t sample = 0;
        for(uint8_t j = 0; j<oversample; j++) 
        {
            sample += input_samples[input_sample_number++];
        }

        //convert to signed
        sample -= 32768;

        //apply effects
        e.process_sample(sample);

        //convert to unsigned 8-bit
        output_samples[ping][output_sample_number] = (sample+32768) >> 4;
      }
      float elapsed = time_us_64()-start;
      printf("time: %f us\n", elapsed/64);
      audio_output.output_samples(output_samples[ping], 64);
      ping ^= 1;
    }


    //interact with user
    if(show_menu)
    {
      show_menu = main_menu.poll();
      if(!show_menu)
      {
        ssd1306_poweroff(&disp);
      }

      //apply settings
      settings.pre_gain = gains[preamp_gain.m_value];
      settings.eq_gains[0] = eq_gains[eq_band1.m_value];
      settings.eq_gains[1] = eq_gains[eq_band2.m_value];
      settings.eq_gains[2] = eq_gains[eq_band3.m_value];
      settings.eq_gains[3] = eq_gains[eq_band4.m_value];
      settings.eq_gains[4] = eq_gains[eq_band5.m_value];
      settings.distortion_effect = static_cast<e_distortion_effect>(distortion_effect.m_value);
      settings.distortion_offset = float2fixed(distortion_offset.m_value);
      settings.distortion_gain = gains[distortion_gain.m_value];
      settings.delay_effect = static_cast<e_delay_effect>(delay_effect.m_value);
      settings.delay_delay_ms = delay_ms.m_value;
      settings.delay_feedback = float2fixed(delay_feedback.m_value);
      settings.echo_delay_ms = delay_ms.m_value;
      settings.echo_feedback = float2fixed(delay_feedback.m_value);
      settings.modulator_effect = static_cast<e_modulator_effect>(modulator_effect.m_value);
      settings.pitch = 1.0f;

      settings.flanger_depth_ms = float2fixed(flanger_depth_ms.m_value);
      settings.flanger_rate_steps = frequency_Hz_to_steps(flanger_rate_Hz.m_value);
      settings.flanger_delay_ms = flanger_delay_ms.m_value;
      settings.flanger_feedback = float2fixed(flanger_feedback.m_value);

      settings.chorus1_depth_ms = float2fixed(chorus1_depth_ms.m_value);
      settings.chorus1_rate_steps = frequency_Hz_to_steps(chorus1_rate_Hz.m_value);
      settings.chorus1_delay_ms = chorus1_delay_ms.m_value;
      settings.chorus1_feedback = float2fixed(chorus1_feedback.m_value);

      settings.chorus2_depth_ms = float2fixed(chorus2_depth_ms.m_value);
      settings.chorus2_rate_steps = frequency_Hz_to_steps(chorus2_rate_Hz.m_value);
      settings.chorus2_delay_ms = chorus2_delay_ms.m_value;
      settings.chorus2_feedback = float2fixed(chorus2_feedback.m_value);

      settings.chorus3_depth_ms = float2fixed(chorus3_depth_ms.m_value);
      settings.chorus3_rate_steps = frequency_Hz_to_steps(chorus3_rate_Hz.m_value);
      settings.chorus3_delay_ms = chorus3_delay_ms.m_value;
      settings.chorus3_feedback = float2fixed(chorus3_feedback.m_value);

      settings.tremolo_rate_steps = frequency_Hz_to_steps(tremolo_rate_Hz.m_value);
      settings.vibrato_depth_ms = float2fixed(vibrato_depth_ms.m_value);
      settings.vibrato_rate_steps = frequency_Hz_to_steps(vibrato_rate_Hz.m_value);
      e.update_settings(settings);
    }
    else if(button_back.poll() || button_fwd.poll() || button_up.poll() || button_down.poll())
    {
      show_menu = true; 
      ssd1306_poweron(&disp);
    }


  }
}
