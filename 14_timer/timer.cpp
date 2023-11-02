#include <stdio.h>
#include "pico/stdlib.h"
#include "ui.h"
#include "PWMAudio.h"
#include <cmath>


class tone
{
  uint32_t p = 0;
  float sin_table[1024];
  public:
  tone()
  {
    for(uint16_t idx=0; idx<1024; idx++)
    {
      sin_table[idx] = sin(2.0*M_PI*idx/1024.0);
    }
  }
  float get_sample(float frequency_Hz, float amplitude, float audio_sample_rate)
  {
      const uint32_t f = static_cast<uint32_t>(4294967296.0 * frequency_Hz/audio_sample_rate);
      const float sample = sin_table[p>>22] * amplitude; //10 MSBs (32-10 = 22)
      p += f;
      return sample;
  }
};

void silence(PWMAudio &audio_output) {
  
  uint16_t samples[1024];
  for(uint16_t idx = 0; idx < 1024; idx++)
  {
    samples[idx] = 0;
  }
  audio_output.output_samples(samples, 1024);

}

void beep(PWMAudio &audio_output) {
  
  uint16_t samples[2][1024];
  tone mytone;
  uint64_t end_time = time_us_64() + 1000000u;

  while(1)
  {
    for(uint8_t ping = 0; ping < 1; ping++)
    {
      for(uint16_t idx = 0; idx < 1024; idx++)
      {
        samples[ping][idx] = mytone.get_sample(500, 2047, 10000)+2048;
      }
      audio_output.output_samples(samples[ping], 1024);
    }
    if(time_us_64() >= end_time){
      silence(audio_output);
      return;
    }
  }

}

int main() {
  stdio_init_all();
  init_ui();
  PWMAudio audio_output(0, 10000);

  number_entry minutes_entry("Minutes", 0, 0, 59, 1);
  number_entry hours_entry("Hours", 0, 0, 23, 1);
  menu_item *menu_items[] = {&minutes_entry, &hours_entry};
  menu my_menu("Menu", "Minutes#Hours", menu_items, 1);

  uint8_t hours = 0;
  uint8_t minutes = 0;
  uint8_t seconds = 0;
  uint8_t stored_hours = 0;
  uint8_t stored_minutes = 0;
  bool running = false;
  bool needs_reset = false;
  uint64_t next_run = 0;
  while(1)
  {
    //launch menu
    //if(button_back.poll())
    //{
      //while(!my_menu.poll())
      //{
        //sleep_ms(100);
      //}
      //hours = hours_entry.m_value;
      //minutes = minutes_entry.m_value;
      //seconds = 0;
    //}

    //increment minutes
    if(button_back.poll())
    {
      running = !running;
      if(running)
      {
        stored_minutes = minutes; 
        stored_hours = hours; 
        next_run = time_us_64()+1000000u; //update timer 1 second from now
      }
      else
      {
        needs_reset = true;
      }
    }

    //fwd
    if(button_fwd.poll())
    {
      if(needs_reset)
      {
        //reset to last value
        minutes = stored_minutes; 
        hours = stored_hours; 
        seconds = 0;
        needs_reset = false;
      }
      else
      {
        //clear
        stored_minutes = minutes = 0; 
        stored_hours = hours = 0; 
        seconds = 0; 
      }
    }

    //increment minutes
    if(button_up.poll())
    {
      if(minutes < 59)
      {
        ++minutes;
      }
      else if(hours < 23)
      {
        ++hours;
        minutes = 0;
      }
    }

    //decrement minutes
    if(button_down.poll())
    {
      if(minutes > 0)
      {
        --minutes;
      }
      else if(hours > 0)
      {
        --hours;
        minutes = 59;
      }
    }

    //draw time
    ssd1306_clear(&disp);

    char line[22];
    snprintf(line, 22, " %02u:%02u:%02u", hours, minutes, seconds);
    ssd1306_draw_string(&disp, 0, 16, 2, line);

    ssd1306_draw_line(&disp, 0, 53, 127, 53);
    if(running)
    {
      ssd1306_draw_string(&disp, 0, 56, 1, "stop |               ");
    }
    else
    {
      if(needs_reset)
      {
        ssd1306_draw_string(&disp, 0, 56, 1, "start|reset|  + |  - ");
      }
      else
      {
        ssd1306_draw_string(&disp, 0, 56, 1, "start|clear|  + |  - ");
      }
    }
    ssd1306_show(&disp);

    if(running){
      if(time_us_64() >= next_run)
      {
        next_run += 1000000u;
        if(seconds == 0)
        {
          if(minutes == 0)
          {
            if(hours == 0)
            {
              //timer expired
              running = false;
              hours = stored_hours;
              minutes = stored_minutes;
              seconds = 0;
              needs_reset = false;
              beep(audio_output);
            }
            else
            {
              minutes = 59;
              --hours;
            }
          }
          else
          {
            --minutes;
            seconds = 59;
          }
        }
        else
        {
          --seconds;
        }
      }
    }

    sleep_ms(50);

  }

}
