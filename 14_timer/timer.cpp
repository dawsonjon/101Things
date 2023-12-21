//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: timer.cpp
// description:
// License: MIT
//

#include "PWMAudio.h"
#include "pico/stdlib.h"
#include "ui.h"
#include <cmath>
#include <stdio.h>

#include "music.hpp"
#include "tunes.hpp"


int main() {
  stdio_init_all();
  init_ui();
  PWMAudio audio_output(0, 10000);
  music player(10000);

  number_entry minutes_entry(0, 0, 59, 1);
  number_entry hours_entry(0, 0, 23, 1);
  menu_item *menu_items[] = {&minutes_entry, &hours_entry};
  menu my_menu("Menu", "Minutes#Hours", menu_items, 1);

  uint64_t timeout = 15 * 1e6;

  uint8_t hours = 0;
  uint8_t minutes = 0;
  uint8_t seconds = 0;
  uint8_t stored_hours = 0;
  uint8_t stored_minutes = 0;
  bool running = false;
  bool needs_reset = false;
  uint64_t next_run = 0;
  uint16_t tune = 0;
  while (1) {

    // increment minutes
    if (button_back.poll()) {
      running = !running;
      if (running) {
        stored_minutes = minutes;
        stored_hours = hours;
        next_run = time_us_64() + 1000000u; // update timer 1 second from now
      } else {
        needs_reset = true;
      }
    }

    // fwd
    if (button_fwd.poll()) {
      if (needs_reset) {
        // reset to last value
        minutes = stored_minutes;
        hours = stored_hours;
        seconds = 0;
        needs_reset = false;
      } else {
        // clear
        stored_minutes = minutes = 0;
        stored_hours = hours = 0;
        seconds = 0;
      }
    }

    // increment minutes
    if (button_up.poll()) {
      if (minutes < 59) {
        ++minutes;
      } else if (hours < 23) {
        ++hours;
        minutes = 0;
      }
    }

    // decrement minutes
    if (button_down.poll()) {
      if (minutes > 0) {
        --minutes;
      } else if (hours > 0) {
        --hours;
        minutes = 59;
      }
    }

    // draw time
    ssd1306_clear(&disp);

    char line[22];
    snprintf(line, 22, " %02u:%02u:%02u", hours, minutes, seconds);
    ssd1306_draw_string(&disp, 0, 16, 2, line);

    ssd1306_draw_line(&disp, 0, 53, 127, 53);
    if (running) {
      ssd1306_draw_string(&disp, 0, 56, 1, "stop |               ");
    } else {
      if (needs_reset) {
        ssd1306_draw_string(&disp, 0, 56, 1, "start|reset|  + |  - ");
      } else {
        ssd1306_draw_string(&disp, 0, 56, 1, "start|clear|  + |  - ");
      }
    }
    ssd1306_show(&disp);

    if (running) {
      if (time_us_64() >= next_run) {
        next_run += 1000000u;
        if (seconds == 0) {
          if (minutes == 0) {
            if (hours == 0) {
              // timer expired
              running = false;
              hours = stored_hours;
              minutes = stored_minutes;
              seconds = 0;
              needs_reset = false;
              uint64_t start_time = time_us_64(); // update timer 1 second from now
              while((time_us_64() - start_time) < timeout)
              {
                player.play_tune(audio_output, tunes[tune], num_notes[tune]);
                ++tune;
              }
            } else {
              minutes = 59;
              --hours;
            }
          } else {
            --minutes;
            seconds = 59;
          }
        } else {
          --seconds;
        }
      }
    }

    sleep_ms(50);
  }
}
