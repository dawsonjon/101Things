#include "psu_mode.h"
#include "pico/stdlib.h"


void disable_power_save()
{
//configure SMPS into power save mode 
#if PICO_W 
  cyw43_arch_init(); 
  cyw43_arch_gpio_put(1, 1); 
#else 
  gpio_init(23); 
  gpio_set_dir(23, GPIO_OUT); 
  gpio_put(23, 1); 
#endif 
}

void enable_power_save()
{
//configure SMPS into power save mode 
#if PICO_W 
  cyw43_arch_init(); 
  cyw43_arch_gpio_put(1, 0); 
#else 
  gpio_init(23); 
  gpio_set_dir(23, GPIO_OUT); 
  gpio_put(23, 0); 
#endif 
}
