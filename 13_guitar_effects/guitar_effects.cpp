#include <stdio.h>

#include "ADCAudio.h"
#include "PWMAudio.h"
#include "effects.h"
#include "ui.h"
#include "pico/stdlib.h"
//#include "pico/cyw43_arch.h"
#include <math.h>

int main() {
  stdio_init_all();
  stdio_set_translate_crlf(&stdio_usb, false);
  init_ui();
  //ssd1306_poweroff(&disp);
  ssd1306_contrast(&disp, 0x01);

//configure SMPS into power save mode
#if PICO_W
  cyw43_arch_init();
  cyw43_arch_gpio_put(1, 1);
#else
  gpio_init(23);
  gpio_set_dir(23, GPIO_OUT);
  gpio_put(23, 1);
#endif

  //preamp
  float_entry preamp_gain(1.0f, 1.0f, 100.0f, 1.0f);
  menu_item *preamp_menu_items[] = {&preamp_gain};
  menu preamp_menu("Preamp", "Preamp Gain", preamp_menu_items, 0);

  //Distorion Menus
  enum_entry distortion_effect("Off#Cubic#Quadratic#Full Wave#Half Wave#Fuzz", 5);
  float_entry distortion_gain(1.0f, 1.0f, 100.0f, 1.0f);
  float_entry distortion_offset(0.0f, -1.0f, 1.0f, 0.05f);
  menu_item *distortion_menu_items[] = {&distortion_effect, &distortion_gain, &distortion_offset};
  menu distortion_menu("Distortion", "Effect#Gain#Offset", distortion_menu_items, 2);

  //Delay Menus
  enum_entry delay_effect("Off#Delay#Reverb", 2);
  float_entry delay_ms(100.0f, 100.0f, 400.0f, 1.0f);
  float_entry delay_feedback(0.8f, 0.0f, 1.0f, 0.1f);
  menu_item *delay_menu_items[] = {&delay_effect, &delay_ms, &delay_feedback};
  menu delay_menu("Delay", "Effect#Delay (ms)#Feedback", delay_menu_items, 2);
  
  //Modulator Menus
  enum_entry modulator_effect("Off#Chorus#Flanger#Tremolo#Vibrato#Pitch Shift", 5);

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
  menu_item *menu_items[] = {&preamp_menu, &distortion_menu, &delay_menu, &modulator_menu};
  menu main_menu("Menu", "Preamp#Distorion#Delay#Modulator", menu_items, 3);

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
    for(uint8_t i=0; i<20; i++)
    {
      audio_input.input_samples(input_samples);

      uint16_t input_sample_number = 0;
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
        e.process_sample(sample, settings);

        //convert to unsigned 8-bit
        output_samples[ping][output_sample_number] = (sample+32768) >> 4;
      }
      audio_output.output_samples(output_samples[ping], 64);
      ping ^= 1;
    }

    if(show_menu)
    {
      show_menu = main_menu.poll();
      if(!show_menu)
      {
        ssd1306_poweroff(&disp);
      }
    }
    else if(button_back.poll() || button_fwd.poll() || button_up.poll() || button_down.poll())
    {
      show_menu = true; 
      //main_menu.focus();
      ssd1306_poweron(&disp);
    }

    //apply settings
    settings.pre_gain = preamp_gain.m_value;
    settings.eq_gains[0] = 1.0;
    settings.eq_gains[1] = 1.0;
    settings.eq_gains[2] = 1.0;
    settings.eq_gains[3] = 1.0;
    settings.eq_gains[4] = 1.0;
    settings.distortion_effect = static_cast<e_distortion_effect>(distortion_effect.m_value);
    settings.distortion_offset = distortion_offset.m_value;
    settings.distortion_gain = distortion_gain.m_value;
    settings.delay_effect = static_cast<e_delay_effect>(delay_effect.m_value);
    settings.delay_delay_ms = delay_ms.m_value;
    settings.delay_feedback = delay_feedback.m_value;
    settings.reverb_delay_ms = delay_ms.m_value;
    settings.reverb_feedback = delay_feedback.m_value;
    settings.modulator_effect = static_cast<e_modulator_effect>(modulator_effect.m_value);
    settings.pitch = 1.0f;
    settings.flanger_depth_ms = flanger_depth_ms.m_value;
    settings.flanger_rate_Hz = flanger_rate_Hz.m_value;
    settings.flanger_delay_ms = flanger_delay_ms.m_value;
    settings.flanger_feedback = flanger_feedback.m_value;
    settings.chorus1_depth_ms = chorus1_depth_ms.m_value;
    settings.chorus1_rate_Hz = chorus1_rate_Hz.m_value;
    settings.chorus1_delay_ms = chorus1_delay_ms.m_value;
    settings.chorus1_feedback = chorus1_feedback.m_value;
    settings.chorus2_depth_ms = chorus2_depth_ms.m_value;
    settings.chorus2_rate_Hz = chorus2_rate_Hz.m_value;
    settings.chorus2_delay_ms = chorus2_delay_ms.m_value;
    settings.chorus2_feedback = chorus2_feedback.m_value;
    settings.chorus3_depth_ms = chorus3_depth_ms.m_value;
    settings.chorus3_rate_Hz = chorus3_rate_Hz.m_value;
    settings.chorus3_delay_ms = chorus3_delay_ms.m_value;
    settings.chorus3_feedback = chorus3_feedback.m_value;
    settings.tremolo_rate_Hz = tremolo_rate_Hz.m_value;
    settings.vibrato_depth_ms = vibrato_depth_ms.m_value;
    settings.vibrato_rate_Hz = vibrato_rate_Hz.m_value;

  }
}
