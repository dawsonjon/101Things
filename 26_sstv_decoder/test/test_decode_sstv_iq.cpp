#include <cstdint>
#include <iostream>

#include "../decode_sstv.h"


int main()
{
  c_sstv_decoder decoder;

  while(1)
  {

    int16_t i, q;
    std::cin.clear();
    std::cin >> i;
    std::cin >> q;
    //std::cerr << "cpp: " << i << " " << q << (int)std::cin.fail() << std::endl;

    bool pixel_complete;
    uint16_t line, col;
    uint8_t colour, pixel;
    int16_t frequency;

    pixel_complete = decoder.decode_iq(i, q, line, col, colour, pixel, frequency);

    std::cout << pixel_complete << " " << line << " " << col << " " << (int)colour << " " << (int)pixel << " " << frequency << std::endl;

  }
}
