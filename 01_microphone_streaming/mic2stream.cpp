//  _  ___  _   _____ _     _
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/
//
// Copyright (c) Jonathan P Dawson 2023
// filename: mic2stream.cpp
// description:
// License: MIT
//

#include <stdio.h>

#include "ADCAudio.h"
#include "TCPSocket.h"
#include "get_wifi.h"
#include "pico/stdlib.h"
#include "psu_mode.h"
#include <math.h>

int main() {
  stdio_init_all();
  if (!get_wifi())
    return 1;

  // reduces noise in ADC measurements
  disable_power_save();

  TCPSocket socket;
  ADCAudio audio_input(16, 10000);
  while (true) {
    printf("Starting server at %s on port %u\n",
           ip4addr_ntoa(netif_ip4_addr(netif_list)), 4242);
    if (!socket.listen(4242)) {
      printf("couldn't start server\n");
      continue;
    }
    printf("server accepted connection\n");
    uint8_t bytes[1024];
    uint16_t *samples;
    sleep_ms(100);
    while (socket.is_connected()) {

      audio_input.input_samples(samples);
      for (uint16_t i = 0; i < 1024; i++) {
        // form 8 bit sample from 12 bit sample
        bytes[i] = samples[i] >> 4;
      }

      uint16_t sent = 0;
      while (sent < 1024)
        sent +=
            socket.send(bytes + sent,
                        1024 - sent); // persevere until whole buffer is sent
    }
    printf("server connection closed\n");
  }

  cyw43_arch_deinit();
  return 0;
}
