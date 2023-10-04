#include <stdio.h>

#include "PWMAudio.h"
#include "ADCAudio.h"
#include "TCPServer.h"
#include "get_wifi.h"
#include "pico/stdlib.h"
#include <math.h>

void transfer_data(PWMAudio &audio_output, ADCAudio &audio_input,
                   TCPServer &server) {
  uint8_t block = 0;
  uint16_t speaker_samples[2][1024]; // ping-pong buffer

  while (true) {
    uint8_t bytes[1024];

    // wifi -> speaker
    //*******************************************************************
    uint16_t samples_received = 0;
    while (samples_received < 1024) {
      if (!server.is_connected())
        return;
             samples_received += server.receive(bytes+samples_received, 1024-samples_received);
    }

    for (uint16_t i = 0; i < 1024; ++i) {
      // convert 8-bit to 12-bit samples
      speaker_samples[block][i] = bytes[i] << 4;
    }

    audio_output.output_samples(speaker_samples[block], 1024);
    block ^= 1;

    // microphone -> wifi
    //*******************************************************************
    uint16_t *microphone_samples;
    audio_input.input_samples(microphone_samples);

    for (uint16_t i = 0; i < 1024; ++i) {
      // convert 12-bit to 8-bit samples
      bytes[i] = microphone_samples[i] >> 4;
    }

    uint16_t samples_sent = 0;
    while (samples_sent < 1024) {
      if (!server.is_connected())
        return;
      samples_sent += server.send(bytes + samples_sent, 1024 - samples_sent);
    }
  }
}

int main() {
  stdio_init_all();
  if(!get_wifi()) return 1;

  TCPServer server;
  PWMAudio audio_output(0, 10000);
  ADCAudio audio_input(16, 10000);

  while (true) {
    printf("Starting server at %s on port %u\n",
           ip4addr_ntoa(netif_ip4_addr(netif_list)), 4242);
    if (!server.listen(4242)) {
      printf("couldn't start server\n");
      continue;
    }
    printf("server waiting for connections\n");
    if (server.accept()) {
      printf("server accepted connection\n");
      transfer_data(audio_output, audio_input, server);
          printf("server connection closed\n");
    }
  }

  cyw43_arch_deinit();
  return 0;
}
