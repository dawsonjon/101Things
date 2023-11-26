#include "guitar_effects_ui.h"
#include <hardware/flash.h>
#include "pico/multicore.h"
#include "string.h"

static const uint8_t __in_flash() __attribute__((aligned(FLASH_SECTOR_SIZE)))
flash_sector[FLASH_SECTOR_SIZE] = {0xff};
// The compiler "knows" that flash sector[0] contains 0xff, so replaces
// accesses to flash_sector[0] with that value. To avoid this, cast to a
// volatile pointer, then the compiler actually reads from the appropriate
// location
volatile uint8_t *flash_sector_address = (uint8_t *)&flash_sector[0];

float dB2lin(float x)
{
  return powf(10.0f, x/20.0f);
}

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

const FixedPoint gains[] = {
  FixedPoint::from_float(dB2lin(0)),
  FixedPoint::from_float(dB2lin(3)),
  FixedPoint::from_float(dB2lin(6)),
  FixedPoint::from_float(dB2lin(9)),
  FixedPoint::from_float(dB2lin(12)),
  FixedPoint::from_float(dB2lin(15)),
  FixedPoint::from_float(dB2lin(18)),
  FixedPoint::from_float(dB2lin(21)),
  FixedPoint::from_float(dB2lin(24))
};

//patch selection
number_entry patch(0, 0, 3, 1);

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
enum_entry delay_effect("Off#Delay#Echo#Reverb", 0, 3);
float_entry delay_ms(100.0f, 100.0f, 400.0f, 1.0f);
float_entry delay_feedback(0.8f, 0.0f, 1.0f, 0.1f);
float_entry delay_mix(0.1f, 0.0f, 1.0f, 0.1f);
menu_item *delay_menu_items[] = {&delay_effect, &delay_ms, &delay_feedback, &delay_mix};
menu delay_menu("Delay", "Effect#Delay (ms)#Feedback#Mix", delay_menu_items, 3);

//Modulator Menus
enum_entry modulator_effect("Off#Chorus#Flanger#Tremolo#Vibrato#Phaser", 0, 5);

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
float_entry tremolo_depth(0.5f, 0.0f, 1.0f, 0.1f);
menu_item *tremolo_menu_items[] = {&tremolo_rate_Hz, &tremolo_depth};
menu tremolo_menu("Tremolo", "Rate (Hz)#Depth", tremolo_menu_items, 1);

float_entry vibrato_depth_ms(4.0f, 1.0f, 5.0f, 0.1f);
float_entry vibrato_rate_Hz(2.0f, 0.1f, 5.0f, 0.1f);
menu_item *vibrato_menu_items[] = {&vibrato_depth_ms, &vibrato_rate_Hz};
menu vibrato_menu("Vibrato", "Depth (ms)#Rate (Hz)", vibrato_menu_items, 1);

float_entry phaser_depth(0.8f, 1.0f, 5.0f, 0.1f);
float_entry phaser_rate_Hz(1.0f, 0.1f, 5.0f, 0.1f);
menu_item *phaser_menu_items[] = {&phaser_depth, &phaser_rate_Hz};
menu phaser_menu("Phaser", "Depth (ms)#Rate (Hz)", phaser_menu_items, 1);

menu_item *modulator_items[] = {&modulator_effect, &flanger_menu, &chorus_menu, &tremolo_menu, &vibrato_menu, &phaser_menu};
menu modulator_menu("Modulator", "Effect#Flanger Settings#Chorus Settings#Tremolo Settings#Vibrato Settings#Phaser Settings", modulator_items, 5);

//main menu
menu_item *menu_items[] = {&patch, &preamp_menu, &eq_menu, &distortion_menu, &delay_menu, &modulator_menu};
menu main_menu("Menu", "Patch#Preamp#EQ#Distorion#Delay#Modulator", menu_items, 5);

void guitar_effects_ui::init()
{
  init_ui();
  ssd1306_contrast(&disp, 0x01);

  //if stored data is available load from flash
  if(*flash_sector_address != 0xff)
  {
    printf("Restoring from flash\n");
    restore_from_flash();
    patch_2_ui(patches[0]);
  }
}

void guitar_effects_ui::do_ui(effects & e, s_effect & settings)
{

  //interact with user
  if(show_menu)
  {
    show_menu = main_menu.poll();
    if(!show_menu)
    {
      ssd1306_poweroff(&disp);
    }

    if(patch.m_value == patch_number)
    {
      //save patch settings
      ui_2_patch(patches[patch_number]);
    }
    else
    {
      //load patch settings
      patch_number = patch.m_value;
      patch_2_ui(patches[patch_number]);
    }

    //apply settings to effects unit
    settings.pre_gain = gains[preamp_gain.m_value];
    settings.eq_gains[0] = eq_gains[eq_band1.m_value];
    settings.eq_gains[1] = eq_gains[eq_band2.m_value];
    settings.eq_gains[2] = eq_gains[eq_band3.m_value];
    settings.eq_gains[3] = eq_gains[eq_band4.m_value];
    settings.eq_gains[4] = eq_gains[eq_band5.m_value];
    settings.distortion_effect = static_cast<e_distortion_effect>(distortion_effect.m_value);
    settings.distortion_offset = FixedPoint::from_float(distortion_offset.m_value);
    settings.distortion_gain = gains[distortion_gain.m_value];
    settings.delay_effect = static_cast<e_delay_effect>(delay_effect.m_value);
    settings.delay_delay_ms = delay_ms.m_value;
    settings.delay_feedback = FixedPoint::from_float(delay_feedback.m_value);
    settings.delay_mix = FixedPoint::from_float(delay_mix.m_value);
    settings.modulator_effect = static_cast<e_modulator_effect>(modulator_effect.m_value);
    settings.flanger_depth_ms = FixedPoint::from_float(flanger_depth_ms.m_value);
    settings.flanger_rate_steps = frequency_Hz_to_steps(flanger_rate_Hz.m_value);
    settings.flanger_delay_ms = flanger_delay_ms.m_value;
    settings.flanger_feedback = FixedPoint::from_float(flanger_feedback.m_value);
    settings.chorus1_depth_ms = FixedPoint::from_float(chorus1_depth_ms.m_value);
    settings.chorus1_rate_steps = frequency_Hz_to_steps(chorus1_rate_Hz.m_value);
    settings.chorus1_delay_ms = chorus1_delay_ms.m_value;
    settings.chorus1_feedback = FixedPoint::from_float(chorus1_feedback.m_value);
    settings.chorus2_depth_ms = FixedPoint::from_float(chorus2_depth_ms.m_value);
    settings.chorus2_rate_steps = frequency_Hz_to_steps(chorus2_rate_Hz.m_value);
    settings.chorus2_delay_ms = chorus2_delay_ms.m_value;
    settings.chorus2_feedback = FixedPoint::from_float(chorus2_feedback.m_value);
    settings.chorus3_depth_ms = FixedPoint::from_float(chorus3_depth_ms.m_value);
    settings.chorus3_rate_steps = frequency_Hz_to_steps(chorus3_rate_Hz.m_value);
    settings.chorus3_delay_ms = chorus3_delay_ms.m_value;
    settings.chorus3_feedback = FixedPoint::from_float(chorus3_feedback.m_value);
    settings.tremolo_rate_steps = frequency_Hz_to_steps(tremolo_rate_Hz.m_value);
    settings.tremolo_depth = FixedPoint::from_float(tremolo_depth.m_value);
    settings.vibrato_depth_ms = FixedPoint::from_float(vibrato_depth_ms.m_value);
    settings.vibrato_rate_steps = frequency_Hz_to_steps(vibrato_rate_Hz.m_value);
    settings.phaser_depth = FixedPoint::from_float(phaser_depth.m_value);
    settings.phaser_rate_steps = frequency_Hz_to_steps(phaser_rate_Hz.m_value);
    e.update_settings(settings);
  }
  else if(button_back.poll() || button_fwd.poll() || button_up.poll() || button_down.poll())
  {
    show_menu = true; 
    ssd1306_poweron(&disp);
  }

}

void guitar_effects_ui :: patch_2_ui(s_patch & patch)
{
  preamp_gain.m_value = patch.preamp_gain;
  eq_band1.m_value = patch.eq_band1;
  eq_band2.m_value = patch.eq_band2;
  eq_band3.m_value = patch.eq_band3;
  eq_band4.m_value = patch.eq_band4;
  eq_band5.m_value = patch.eq_band5;
  distortion_effect.m_value = patch.distortion_effect;
  distortion_offset.m_value = patch.distortion_offset;
  distortion_gain.m_value = patch.distortion_gain;
  delay_effect.m_value = patch.delay_effect;
  delay_ms.m_value = patch.delay_ms;
  delay_feedback.m_value = patch.delay_feedback;
  delay_mix.m_value = patch.delay_mix;
  modulator_effect.m_value = patch.modulator_effect;
  flanger_depth_ms.m_value = patch.flanger_depth_ms;
  flanger_rate_Hz.m_value = patch.flanger_rate_Hz;
  flanger_delay_ms.m_value = patch.flanger_delay_ms;
  flanger_feedback.m_value = patch.flanger_feedback;
  chorus1_depth_ms.m_value = patch.chorus1_depth_ms;
  chorus1_rate_Hz.m_value = patch.chorus1_rate_Hz;
  chorus1_delay_ms.m_value = patch.chorus1_delay_ms;
  chorus1_feedback.m_value = patch.chorus1_feedback;
  chorus2_depth_ms.m_value = patch.chorus2_depth_ms;
  chorus2_rate_Hz.m_value = patch.chorus2_rate_Hz;
  chorus2_delay_ms.m_value = patch.chorus2_delay_ms;
  chorus2_feedback.m_value = patch.chorus2_feedback;
  chorus3_depth_ms.m_value = patch.chorus3_depth_ms;
  chorus3_rate_Hz.m_value = patch.chorus3_rate_Hz;
  chorus3_delay_ms.m_value = patch.chorus3_delay_ms;
  chorus3_feedback.m_value = patch.chorus3_feedback;
  tremolo_rate_Hz.m_value = patch.tremolo_rate_Hz;
  tremolo_depth.m_value = patch.tremolo_depth;
  vibrato_depth_ms.m_value = patch.vibrato_depth_ms;
  vibrato_rate_Hz.m_value = patch.vibrato_rate_Hz;
  phaser_depth.m_value = patch.phaser_depth;
  phaser_rate_Hz.m_value = patch.phaser_rate_Hz;
}

bool assign_if_changed(float &a, float &b)
{
  if(a != b)
  {
    a = b;
    return true;
  }
  else
  {
    return false;
  }
}

bool assign_if_changed(uint8_t &a, int32_t &b)
{
  if(a != b)
  {
    a = b;
    return true;
  }
  else
  {
    return false;
  }
}

void guitar_effects_ui :: ui_2_patch(s_patch & patch)
{
  bool changed = false;
  changed |= assign_if_changed(patch.preamp_gain, preamp_gain.m_value);
  changed |= assign_if_changed(patch.eq_band1, eq_band1.m_value);
  changed |= assign_if_changed(patch.eq_band2, eq_band2.m_value);
  changed |= assign_if_changed(patch.eq_band3, eq_band3.m_value);
  changed |= assign_if_changed(patch.eq_band4, eq_band4.m_value);
  changed |= assign_if_changed(patch.eq_band5, eq_band5.m_value);
  changed |= assign_if_changed(patch.distortion_effect, distortion_effect.m_value);
  changed |= assign_if_changed(patch.distortion_offset, distortion_offset.m_value);
  changed |= assign_if_changed(patch.distortion_gain, distortion_gain.m_value);
  changed |= assign_if_changed(patch.delay_effect, delay_effect.m_value);
  changed |= assign_if_changed(patch.delay_ms, delay_ms.m_value);
  changed |= assign_if_changed(patch.delay_feedback, delay_feedback.m_value);
  changed |= assign_if_changed(patch.delay_mix, delay_mix.m_value);
  changed |= assign_if_changed(patch.modulator_effect, modulator_effect.m_value);
  changed |= assign_if_changed(patch.flanger_depth_ms, flanger_depth_ms.m_value);
  changed |= assign_if_changed(patch.flanger_rate_Hz, flanger_rate_Hz.m_value);
  changed |= assign_if_changed(patch.flanger_delay_ms, flanger_delay_ms.m_value);
  changed |= assign_if_changed(patch.flanger_feedback, flanger_feedback.m_value);
  changed |= assign_if_changed(patch.chorus1_depth_ms, chorus1_depth_ms.m_value);
  changed |= assign_if_changed(patch.chorus1_rate_Hz, chorus1_rate_Hz.m_value);
  changed |= assign_if_changed(patch.chorus1_delay_ms, chorus1_delay_ms.m_value);
  changed |= assign_if_changed(patch.chorus1_feedback, chorus1_feedback.m_value);
  changed |= assign_if_changed(patch.chorus2_depth_ms, chorus2_depth_ms.m_value);
  changed |= assign_if_changed(patch.chorus2_rate_Hz, chorus2_rate_Hz.m_value);
  changed |= assign_if_changed(patch.chorus2_delay_ms, chorus2_delay_ms.m_value);
  changed |= assign_if_changed(patch.chorus2_feedback, chorus2_feedback.m_value);
  changed |= assign_if_changed(patch.chorus3_depth_ms, chorus3_depth_ms.m_value);
  changed |= assign_if_changed(patch.chorus3_rate_Hz, chorus3_rate_Hz.m_value);
  changed |= assign_if_changed(patch.chorus3_delay_ms, chorus3_delay_ms.m_value);
  changed |= assign_if_changed(patch.chorus3_feedback, chorus3_feedback.m_value);
  changed |= assign_if_changed(patch.tremolo_rate_Hz, tremolo_rate_Hz.m_value);
  changed |= assign_if_changed(patch.tremolo_depth, tremolo_depth.m_value);
  changed |= assign_if_changed(patch.vibrato_depth_ms, vibrato_depth_ms.m_value);
  changed |= assign_if_changed(patch.vibrato_rate_Hz, vibrato_rate_Hz.m_value);
  changed |= assign_if_changed(patch.phaser_depth, phaser_depth.m_value);
  changed |= assign_if_changed(patch.phaser_rate_Hz, phaser_rate_Hz.m_value);
  if(changed)
  {
    printf("saving to flash\n");
    save_to_flash();
  }
}


void guitar_effects_ui :: restore_from_flash()
{
  memcpy(&patches, (void*)flash_sector_address, sizeof patches);
}

void guitar_effects_ui :: save_to_flash()
{
  // store SSID for next time
  uint8_t sector_to_write[FLASH_SECTOR_SIZE];
  memcpy(&sector_to_write, &patches, sizeof patches);

  const uint32_t address = (uint32_t) & (flash_sector[0]);
  const uint32_t flash_address = address - XIP_BASE;

  //!!! PICO is **very** fussy about flash erasing, there must be no code
  //!running in flash.  !!!
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  const uint32_t ints =
      save_and_disable_interrupts(); // disable all interrupts

  // safe to erase flash here
  //--------------------------------------------------------------------------------------------
  flash_range_erase(flash_address, FLASH_SECTOR_SIZE);
  flash_range_program(flash_address, sector_to_write, FLASH_SECTOR_SIZE);
  //--------------------------------------------------------------------------------------------

  restore_interrupts(ints); // restore interrupts
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //!!! Normal operation resumed

}
