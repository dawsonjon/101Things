//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2023
// filename: transmitter.cpp
// description: Ham Transmitter for Pi Pico 
// License: MIT
//

// Transmitter example with serial interface
// Supports serial streaming of audio data using
// transmit.py utility

#include "pico/stdlib.h"
#include <cmath>
#include <stdio.h>
#include <ctype.h>

#include "transmit.h"
#include "psu_mode.h"

// example application
int main() {
  stdio_init_all();
  disable_power_save();

  // chose default values
  uint32_t frequency = 14.175e6;
  tx_mode_t mode = USB;

  printf("Pi Pico Transmitter>\r\n");
  while (1) {
    int command = getchar_timeout_us(0);
    char frequency_string[100];
    if (command != PICO_ERROR_TIMEOUT) {
      printf("command: %c\r\n", command);
      switch (command) {
      // set frequency
      case 'f':
        frequency = 0;
        while(1)
        {
          char c = getchar();
          if(!isdigit(c)) break;
          frequency *= 10;
          frequency += (c - '0');
        }
        printf("terminate: %c\r\n");
        printf("frequency: %u Hz\r\n", frequency);
        break;

      // set mode
      case 'm':
        while (1) {
          char command = getchar();
          if (command == 'a') {
            printf("MODE=AM\r\n");
            mode = AM;
            break;
          } else if (command == 'f') {
            printf("MODE=FM\r\n");
            mode = FM;
            break;
          } else if (command == 'l') {
            printf("MODE=LSB\r\n");
            mode = LSB;
            break;
          } else if (command == 'u') {
            printf("MODE=USB\r\n");
            mode = USB;
            break;
          }
        }
        break;

      // transmit serial data
      case 's':
        printf("Starting transmitter\r\n");
        stdio_set_translate_crlf(&stdio_usb, false);
        transmit(mode, frequency, true);
        stdio_set_translate_crlf(&stdio_usb, true);
        printf("Transmitter timed out\r\n");
        break;

      // help
      case '?':
        printf("\r\nSerial Interface Help\r\n");
        printf("=====================\r\n\r\n");
        printf("fxxxxxx, set frequency Hz\r\n");
        printf("mx, set mode, a=AM, f=FM, l=LSB, u=USB\r\n");
        printf("s, transmit serial data - (timeout terminates)\r\n");
        printf("?, Help (this message)\r\n");
        break;
      }
      printf("Pi Pico Transmitter>\r\n");
    }

    sleep_us(1000);
  }
}
