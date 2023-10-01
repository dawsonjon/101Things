#include <stdio.h>

#include "ADCAudio.h"
#include "TCPServer.h"
#include "pico/stdlib.h"
#include "secrets.h"
#include <math.h>

int main() {
  stdio_init_all();

  if (cyw43_arch_init()) {
    printf("failed to initialise\n");
    return 1;
  }

  cyw43_arch_enable_sta_mode();

  printf("Connecting to WiFi...\n");
  if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK,
                                         30000)) {
    printf("failed to connect to WIFI\n");
    return 1;
  } else {
    printf("Connected to WIFI.\n");
  }

  TCPServer server;
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
      uint8_t bytes[1024];
      uint16_t *samples;
      sleep_ms(100);
      while (server.is_connected()) {

        audio_input.input_samples(samples);
        for (uint16_t i = 0; i < 1024; i++) {
          bytes[i] = samples[i] >> 4; // form 8 bit sample from 12 bit sample
        }

        uint16_t sent = 0;
        while (sent < 1024)
          sent +=
              server.send(bytes + sent,
                          1024 - sent); // persevere until whole buffer is sent
      }
      printf("server connection closed\n");
    }
  }

  cyw43_arch_deinit();
  return 0;
}
