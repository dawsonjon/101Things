#include "ui.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "string.h"

button button_back(PIN_A);
button button_fwd(PIN_B);
button button_up(PIN_C);
button button_down(PIN_D);
ssd1306_t disp;

char line[257];
char* centre(const char string_to_centre[], uint8_t length)
{
  
  for(uint8_t idx = 0; idx<length; ++idx)
  {
    line[idx] = ' ';
  }
  line[length] = 0;
  uint8_t pos = (length - strlen(string_to_centre))/2;
  for(uint8_t idx = 0; idx<strlen(string_to_centre); ++idx)
  {
    line[pos++] = string_to_centre[idx];
  }
  return line;
}

void init_ui()
{
  //configure display
  i2c_init(i2c1, 400000);
  const uint8_t PIN_DISPLAY_SDA = 18;
  const uint8_t PIN_DISPLAY_SCL = 19;
  gpio_set_function(PIN_DISPLAY_SDA, GPIO_FUNC_I2C);
  gpio_set_function(PIN_DISPLAY_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(PIN_DISPLAY_SDA);
  gpio_pull_up(PIN_DISPLAY_SCL);
  disp.external_vcc=false;
  ssd1306_init(&disp, 128, 64, 0x3C, i2c1);

  gpio_init(PIN_A);
  gpio_set_dir(PIN_A, GPIO_IN);
  gpio_pull_up(PIN_A);
  gpio_init(PIN_B);
  gpio_set_dir(PIN_B, GPIO_IN);
  gpio_pull_up(PIN_B);
  gpio_init(PIN_C);
  gpio_set_dir(PIN_C, GPIO_IN);
  gpio_pull_up(PIN_C);
  gpio_init(PIN_D);
  gpio_set_dir(PIN_D, GPIO_IN);
  gpio_pull_up(PIN_D);

}

bool button :: poll()
{
  switch(state)
  {
    case up:
      if(!gpio_get(pin))
      {
        state = down;
        delay = 5;
        return true;
      }
      else
      {
        return false;
      }
      break;

    case down:
      if(!gpio_get(pin))
      {
        state = down;
        if(!delay)
        {
          state = repeat;
          return true;
        } 
        else
        {
          delay--;
          return false;
        }
      }
      else
      {
        state = up;
        return false;
      }
      break;

    case repeat:
      if(!gpio_get(pin))
      {
        state = down;
        return true;
      }
      else
      {
        state = up;
        return false;
      }

    default:
      return false;
  }
}

void draw_number(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, int32_t value, uint8_t length)
{
  char string[length+1]; 
  snprintf(string, length+1, "%.*d", length, value);
  ssd1306_draw_string(p, x, y, scale, centre(string, 21));
}

uint8_t num_digits(int32_t x)
{
  uint8_t digits = 0;
  if(x < 0) x = -x;
  while(x)
  {
    digits += 1;
    x /= 10;
  }
  return digits;
}

bool number_entry :: poll()
{
  switch(state)
  {
    case idle:
      state = working;
      temp_value = m_value;
      break;
  }

  if(button_up.poll())
  {
    temp_value+=m_step;
    if(temp_value > m_max) temp_value = m_max;
  }

  if(button_down.poll())
  {
    temp_value-=m_step;
    if(temp_value < m_min) temp_value = m_min;
  }

  ssd1306_clear(&disp);
  ssd1306_draw_string(&disp, 0, 0, 1, centre(m_title, 21));
  ssd1306_draw_line(&disp, 0, 9, 127, 9);
  draw_number(&disp, 0, 16, 1, temp_value, num_digits(m_max));
  ssd1306_draw_line(&disp, 0, 53, 127, 53);
  ssd1306_draw_string(&disp, 0, 56, 1, "back | ok | up | down");
  ssd1306_show(&disp);

  if(button_fwd.poll())
  {
    m_value = temp_value;
    state = idle;
    return true;
  }

  if(button_back.poll())
  {
    state = idle;
    return true;
  }

  return false;
}

void extract_option(char string[], const char options[], uint8_t option){
    char x;
    uint8_t i, idx=0;
    uint8_t string_pos=0;

    //find nth substring
    for(i=0; i<option; i++){ 
      while(options[idx++]!='#'){}
    }

    //print substring
    while(1){
      x = options[idx];
      if(x==0 || x=='#')
      {
         string[string_pos] = 0;
         return;
      }
      string[string_pos++] = x;
      idx++;
    }
}


bool enum_entry :: poll()
{
  switch(state)
  {
    case idle:
      state = working;
      temp_value = m_value;
      break;
  }

  if(button_up.poll())
  {
    temp_value++;
    if(temp_value > m_max) temp_value = m_max;
  }

  if(button_down.poll())
  {
    temp_value--;
    if(temp_value < 0) temp_value = 0;
  }

  ssd1306_clear(&disp);
  ssd1306_draw_string(&disp, 0, 0, 1, centre(m_title, 21));
  ssd1306_draw_line(&disp, 0, 9, 127, 9);
  snprintf(line, 22, "    ( %2u of %2u )     ", temp_value+1, m_max+1);
  ssd1306_draw_string(&disp, 0, 11, 1, line);
  char option[17];
  extract_option(option, m_names, temp_value); 
  ssd1306_draw_string(&disp, 0, 30, 1, centre(option, 21));
  ssd1306_draw_line(&disp, 0, 53, 127, 53);
  ssd1306_draw_string(&disp, 0, 56, 1, "back | ok | up | down");
  ssd1306_show(&disp);

  if(button_fwd.poll())
  {
    m_value = temp_value;
    state = idle;
    return true;
  }

  if(button_back.poll())
  {
    state = idle;
    return true;
  }

  return false;
}


bool menu :: poll()
{
  switch(state)
  {
    case idle:
      state = working;
      temp_value = m_value;
      return false;

    case working:
      if(button_up.poll())
      {
        temp_value++;
        if(temp_value > m_max) temp_value = m_max;
      }

      if(button_down.poll())
      {
        temp_value--;
        if(temp_value < 0) temp_value = 0;
      }

      ssd1306_clear(&disp);
      ssd1306_draw_string(&disp, 0, 0, 1, centre(m_title, 21));
      ssd1306_draw_line(&disp, 0, 9, 127, 9);
      snprintf(line, 22, "    ( %2u of %2u )     ", temp_value+1, m_max+1);
      ssd1306_draw_string(&disp, 0, 11, 1, line);
      char option[17];
      extract_option(option, m_names, temp_value); 
      ssd1306_draw_string(&disp, 0, 30, 1, centre(option, 21));
      ssd1306_draw_line(&disp, 0, 53, 127, 53);
      ssd1306_draw_string(&disp, 0, 56, 1, "back | ok | up | down");
      ssd1306_show(&disp);

      if(button_fwd.poll())
      {
        m_value = temp_value;
        state = item_active;
      }

      if(button_back.poll())
      {
        state = idle;
        return true;
      }
      return false;

    case item_active:
      if(m_menu_items[temp_value]->poll())
      {
         state = idle;
         return true;
      }
      return false;

    default:
      return false;
  }

}
