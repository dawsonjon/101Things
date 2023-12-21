//  _  ___  _   _____ _     _                 
// / |/ _ \/ | |_   _| |__ (_)_ __   __ _ ___ 
// | | | | | |   | | | '_ \| | '_ \ / _` / __|
// | | |_| | |   | | | | | | | | | | (_| \__ \
// |_|\___/|_|   |_| |_| |_|_|_| |_|\__, |___/
//                                  |___/    
//
// Copyright (c) Jonathan P Dawson 2023
// filename: duplex.cpp
// description:
// License: MIT
//

#include <stdio.h>

#include "ADCAudio.h"
#include "PWMAudio.h"
#include "TCPSocket.h"
#include "get_wifi.h"
#include "pico/stdlib.h"
#include "psu_mode.h"
#include <math.h>

void transfer_data(PWMAudio &audio_output, ADCAudio &audio_input,
                   TCPSocket &socket) {
  uint8_t block = 0;
  uint16_t speaker_samples[2][1024]; // ping-pong buffer

  while (true) {
    uint8_t bytes[1024];

    // wifi -> speaker
    //*******************************************************************
    if (!socket.is_connected())
      return;
    uint16_t samples_received = socket.receive(bytes, 1024);

    for (uint16_t i = 0; i < samples_received; ++i) {
      // convert 8-bit to 12-bit samples
      speaker_samples[block][i] = bytes[i] << 4;
    }

    audio_output.output_samples(speaker_samples[block], samples_received);
    block ^= 1;

    // microphone -> wifi
    //*******************************************************************
    uint16_t *microphone_samples;
    audio_input.input_samples(microphone_samples);

    for (uint16_t i = 0; i < 1024; ++i) {
      // convert 12-bit to 8-bit samples
      bytes[i] = microphone_samples[i] >> 4;
    }

    if (!socket.is_connected())
      return;
    // at this point it is possible that fewer than 1024
    uint16_t samples_sent = socket.send(bytes, 1024);
  }
}

int main() {
  stdio_init_all();
#if defined ACCESS_POINT_SERVER
#define IS_SERVER true
  if (!get_wifi_ap("AudioServer", "password"))
    return 1;
#elif defined ACCESS_POINT_CLIENT
#define IS_SERVER false
#elif defined SERVER
#define IS_SERVER true
  if (!get_wifi())
    return 1;
#elif defined CLIENT
#define IS_SERVER false
  if (!get_wifi())
    return 1;
#else
#error
#endif

  disable_power_save();

  TCPSocket socket;
  PWMAudio audio_output(0, 10000);
  ADCAudio audio_input(16, 10000);

  while (true) {
#if IS_SERVER
    printf("Starting server at %s on port %u\n",
           ip4addr_ntoa(netif_ip4_addr(netif_list)), 4242);
    if (!socket.listen(4242)) {
      printf("couldn't start server\n");
      continue;
    }
#else
    printf("Connecting to server at %s on port %u\n", "192.168.1.101", 4242);
    if (!socket.open("192.168.1.101", 4242)) {
      printf("couldn't connect to server\n");
      continue;
    }
#endif
    printf("connected\n");
    transfer_data(audio_output, audio_input, socket);
    printf("disconnected\n");

    sleep_ms(1000);
  }

  cyw43_arch_deinit();
  return 0;
}
