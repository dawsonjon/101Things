#ifndef UI_H
#define UI_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "ssd1306.h"

const uint8_t PIN_A = 22;
const uint8_t PIN_B = 21;
const uint8_t PIN_C = 20;
const uint8_t PIN_D = 17;

extern ssd1306_t disp;

void init_ui();
void draw_number(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, int32_t value, uint8_t length);

class button
{
  private:
  enum e_state {up, down, repeat};
  e_state state = up;
  const uint8_t pin;
  uint8_t delay;

  public:
  button(const uint8_t p) : pin(p), state(up), delay(5) {}
  bool poll();
};

extern button button_back;
extern button button_fwd;
extern button button_up;
extern button button_down;

class menu_item
{
  public:
  virtual bool poll()=0;
  virtual void draw()=0;
  virtual bool is_menu()=0;
};

class number_entry : public menu_item
{
  private:
  const int32_t m_min;
  const int32_t m_max;
  const int32_t m_step;

  public:
  bool m_redraw = true;
  int32_t m_value;

  number_entry(const char *title, const int32_t value, const int32_t min, const int32_t max, const int32_t step) 
  : m_min(min), m_max(max), m_step(step), m_value(value){}
  virtual bool poll();
  virtual void draw();
  virtual bool is_menu()
  {
    return false;
  }
};

class float_entry : public menu_item
{
  private:
  const float m_min;
  const float m_max;
  const float m_step;

  public:
  bool m_redraw = true;
  float m_value;

  float_entry(const float value, const float min, const float max, const float step) 
  : m_min(min), m_max(max), m_step(step), m_value(value){}
  virtual bool poll();
  virtual void draw();
  virtual bool is_menu()
  {
    return false;
  }
};

class enum_entry : public menu_item
{
  private:
  const int32_t m_max;
  const char *m_names;

  public:
  bool m_redraw = true;
  int32_t m_value;

  enum_entry(const char *names, int32_t value, const int32_t max) 
  : m_names(names), m_max(max), m_value(value){}
  virtual bool poll();
  virtual void draw();
  virtual bool is_menu()
  {
    return false;
  }
};

class menu : public menu_item
{
  private:
  const int32_t m_max;
  int32_t m_temp_value;
  const char *m_title;
  const char *m_names;
  menu_item **m_menu_items;
  bool submenu_active = false;

  public:
  bool m_redraw = true;
  menu(const char *title, const char *names, menu_item **menu_items, const int32_t max) 
  : m_title(title), m_names(names), m_menu_items(menu_items), m_max(max), m_temp_value(0){}
  virtual bool poll();
  virtual void draw();
  virtual bool is_menu()
  {
    return true;
  }
};
#endif
