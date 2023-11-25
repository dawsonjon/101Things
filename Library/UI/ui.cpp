#include "ui.h"

#include "pico/stdlib.h"
#include "ssd1306.h"
#include "string.h"
#include <stdio.h>

button button_back(PIN_A);
button button_fwd(PIN_B);
button button_up(PIN_C);
button button_down(PIN_D);
ssd1306_t disp;

char line[257];
char *centre(const char string_to_centre[], uint8_t length) {

  for (uint8_t idx = 0; idx < length; ++idx) {
    line[idx] = ' ';
  }
  line[length] = 0;
  uint8_t pos = (length - strlen(string_to_centre)) / 2;
  for (uint8_t idx = 0; idx < strlen(string_to_centre); ++idx) {
    line[pos++] = string_to_centre[idx];
  }
  return line;
}

void init_ui() {
  // configure display
  i2c_init(i2c1, 400000);
  const uint8_t PIN_DISPLAY_SDA = 18;
  const uint8_t PIN_DISPLAY_SCL = 19;
  gpio_set_function(PIN_DISPLAY_SDA, GPIO_FUNC_I2C);
  gpio_set_function(PIN_DISPLAY_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(PIN_DISPLAY_SDA);
  gpio_pull_up(PIN_DISPLAY_SCL);
  disp.external_vcc = false;
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

bool button ::poll() {
  switch (state) {
  case up:
    if (!gpio_get(pin)) {
      state = down;
      delay = 5;
      return true;
    } else {
      return false;
    }
    break;

  case down:
    if (!gpio_get(pin)) {
      state = down;
      if (!delay) {
        state = repeat;
        return true;
      } else {
        delay--;
        return false;
      }
    } else {
      state = up;
      return false;
    }
    break;

  case repeat:
    if (!gpio_get(pin)) {
      state = down;
      return true;
    } else {
      state = up;
      return false;
    }

  default:
    return false;
  }
}

void draw_number(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale,
                 int32_t value, uint8_t length) {
  char string[length + 1];
  snprintf(string, length + 1, "%.*d", length, value);
  ssd1306_draw_string(p, x, y, scale, centre(string, 21));
}

uint8_t num_digits(int32_t x) {
  uint8_t digits = 0;
  if (x < 0)
    x = -x;
  while (x) {
    digits += 1;
    x /= 10;
  }
  return digits;
}

bool redraw = true;

void number_entry ::draw() {
  char line[22];
  if (m_value == m_max) {
    snprintf(line, 22, "< %d  ", m_value);
  } else if (m_value == m_min) {
    snprintf(line, 22, "  %d >", m_value);
  } else {
    snprintf(line, 22, "< %d >", m_value);
  }
  ssd1306_draw_string(&disp, 0, 56, 1, centre(line, 21));
}

bool number_entry ::poll() {
  if (button_fwd.poll()) {
    m_value += m_step;
    if (m_value > m_max)
      m_value = m_max;
    redraw = true;
  }
  if (button_back.poll()) {
    m_value -= m_step;
    if (m_value < m_min)
      m_value = m_min;
    redraw = true;
  }
  return true;
}

void float_entry ::draw() {
  char line[22];
  if (m_max - m_value < 0.01f) {
    snprintf(line, 22, "< %.2f  ", m_value);
  } else if (m_value - m_min < 0.01f) {
    snprintf(line, 22, "  %.2f >", m_value);
  } else {
    snprintf(line, 22, "< %.2f >", m_value);
  }
  ssd1306_draw_string(&disp, 0, 56, 1, centre(line, 21));
}

bool float_entry ::poll() {
  if (button_fwd.poll()) {
    m_value += m_step;
    if (m_value > m_max)
      m_value = m_max;
    redraw = true;
  }
  if (button_back.poll()) {
    m_value -= m_step;
    if (m_value < m_min)
      m_value = m_min;
    redraw = true;
  }
  return true;
}

void extract_option(char string[], const char options[], uint8_t option) {
  char x;
  uint8_t i, idx = 0;
  uint8_t string_pos = 0;

  // find nth substring
  for (i = 0; i < option; i++) {
    while (options[idx++] != '#') {
    }
  }

  // print substring
  while (1) {
    x = options[idx];
    if (x == 0 || x == '#') {
      string[string_pos] = 0;
      return;
    }
    string[string_pos++] = x;
    idx++;
  }
}

void enum_entry ::draw() {
  char line[22];
  char option[22];
  extract_option(option, m_names, m_value);
  if (m_value == m_max) {
    snprintf(line, 22, "< %s  ", option);
  } else if (m_value == 0) {
    snprintf(line, 22, "  %s >", option);
  } else {
    snprintf(line, 22, "< %s >", option);
  }
  ssd1306_draw_string(&disp, 0, 56, 1, centre(line, 21));
}

bool enum_entry ::poll() {
  if (button_fwd.poll()) {
    ++m_value;
    if (m_value > m_max)
      m_value = m_max;
    redraw = true;
  }
  if (button_back.poll()) {
    --m_value;
    if (m_value < 0)
      m_value = 0;
    redraw = true;
  }
  return true;
}

void menu ::draw() {
  char line[22];
  char option[19];

  // line 0 title
  // line 1 horizontal line
  // line 2   entry -2
  // line 3   entry -1
  // line 4 > entry 0
  // line 5   entry +1
  // line 6 horitontal line
  // line 7 value

  ssd1306_clear(&disp);
  // line 0
  snprintf(line, 22, "%s (%u/%u)", m_title, m_temp_value + 1, m_max + 1);
  ssd1306_draw_string(&disp, 0, 0, 1, centre(line, 21));
  // line 1
  ssd1306_draw_line(&disp, 0, 12, 127, 12);
  // line 2
  if (m_temp_value >= 2) {
    extract_option(option, m_names, m_temp_value - 2);
    snprintf(line, 22, "  %s", option);
    ssd1306_draw_string(&disp, 0, 16, 1, line);
  } else {
    ssd1306_draw_string(&disp, 0, 16, 1, "                     ");
  }
  // line 3
  if (m_temp_value >= 1) {
    extract_option(option, m_names, m_temp_value - 1);
    snprintf(line, 22, "  %s", option);
    ssd1306_draw_string(&disp, 0, 24, 1, line);
  } else {
    ssd1306_draw_string(&disp, 0, 24, 1, "                     ");
  }
  // line 4
  if (m_temp_value <= m_max) {
    extract_option(option, m_names, m_temp_value);
    snprintf(line, 22, "> %s", option);
    ssd1306_draw_string(&disp, 0, 32, 1, line);
  } else {
    ssd1306_draw_string(&disp, 0, 32, 1, "> [ RETURN ]         ");
  }
  // line 5
  if ((m_temp_value + 1) <= m_max) {
    extract_option(option, m_names, m_temp_value + 1);
    snprintf(line, 22, "  %s", option);
    ssd1306_draw_string(&disp, 0, 40, 1, line);
  } else if (m_temp_value == m_max)
    ssd1306_draw_string(&disp, 0, 40, 1, "  [ RETURN ]         ");
  else
    ssd1306_draw_string(&disp, 0, 40, 1, "                     ");
  // line 6
  ssd1306_draw_line(&disp, 0, 52, 127, 52);

  // line 7
  if (m_temp_value <= m_max) {
    if (m_menu_items[m_temp_value]->is_menu())
      ssd1306_draw_string(&disp, 0, 56, 1, centre("< SELECT SUBMENU >", 21));
  } else
    ssd1306_draw_string(&disp, 0, 56, 1, centre("< RETURN >", 21));

  // draw children
  if (m_temp_value <= m_max) {
    if (!m_menu_items[m_temp_value]->is_menu()) {
      m_menu_items[m_temp_value]->draw();
    }
  }

  ssd1306_show(&disp);
}

bool menu ::poll() {

  // hand responsibility to sub menu
  if (submenu_active) {
    if (!m_menu_items[m_temp_value]->poll()) {
      submenu_active = false;
    }
    return true;
  }

  if (button_down.poll()) {
    if (m_temp_value <= m_max)
      ++m_temp_value;
    redraw = true;
  }

  if (button_up.poll()) {
    if (m_temp_value > 0)
      --m_temp_value;
    redraw = true;
  }

  bool done = false;
  if (m_temp_value <= m_max) {
    if (m_menu_items[m_temp_value]->is_menu()) {
      if (button_fwd.poll() || button_back.poll()) {
        submenu_active = true;
        redraw = true;
      }
    } else {
      m_menu_items[m_temp_value]->poll();
    }
  } else {
    if (button_fwd.poll() || button_back.poll())
      done = true;
  }

  if (redraw) {
    draw();
    redraw = false;
  }

  if (submenu_active)
    redraw = true;
  if (done)
    redraw = true;

  return !done;
}
