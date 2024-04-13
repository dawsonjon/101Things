#ifndef ST7735_H
#define ST7735_H

#include "stdint.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

class st7735
{

  public:
    st7735(spi_inst_t *spi, uint8_t pin_miso, uint8_t pin_sck, uint8_t pin_mosi, uint8_t pin_dc, uint8_t pin_reset, uint8_t pin_cs);
    void reset();
    void initr();
    void set_rotation(uint8_t new_rotation);
    void set_rgb(bool x);
    void draw(uint8_t data[], uint32_t len);


  private:
    void write_command(const uint8_t command);
    void write_data(const uint8_t *	command, size_t len);
    void setMADCTL();
    void set_windowloc();

    bool rgb;
    uint8_t current_rotation;
    uint8_t width, height;
    uint8_t windowLocData[4] = {};
    uint8_t dc;
    uint8_t rst;
    uint8_t cs;
    spi_inst_t *spi_port;

};

#endif
