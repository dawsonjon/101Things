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
};

class number_entry : public menu_item
{
  private:
  const int32_t m_min;
  const int32_t m_max;
  const int32_t m_step;
  int32_t temp_value;
  enum e_state {idle, working};
  e_state state = idle;
  const char *m_title;

  public:
  int32_t m_value;

  number_entry(const char *title, const int32_t value, const int32_t min, const int32_t max, const int32_t step) 
  : m_title(title), m_min(min), m_max(max), m_step(step), m_value(value){}
  virtual bool poll();

};

class enum_entry : public menu_item
{
  private:
  const int32_t m_max;
  int32_t temp_value;
  enum e_state {idle, working};
  e_state state = idle;
  const char *m_title;
  const char *m_names;

  public:
  int32_t m_value;

  enum_entry(const char *title, const char *names, const int32_t value, const int32_t max) 
  : m_title(title), m_names(names), m_max(max), m_value(value){}
  virtual bool poll();

};

class menu : public menu_item
{
  private:
  const int32_t m_max;
  int32_t temp_value;
  enum e_state {idle, working, item_active};
  e_state state = idle;
  const char *m_title;
  const char *m_names;
  menu_item **m_menu_items;

  public:
  int32_t m_value;

  menu(const char *title, const char *names, menu_item **menu_items, const int32_t value, const int32_t max) 
  : m_title(title), m_names(names), m_menu_items(menu_items), m_max(max), m_value(value){}
  virtual bool poll();

};

#endif
