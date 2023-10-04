#include <stdio.h>

#include "PWMAudio.h"
#include "TCPServer.h"
#include "get_wifi.h"
#include "pico/stdlib.h"
#include <math.h>

int main() {
  stdio_init_all();

  if(!get_wifi()) return 1;

  TCPServer server;
  PWMAudio audio_output(0, 10000);
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
      uint16_t samples[2][1024];
      sleep_ms(100);
      while (server.is_connected()) {
        for (uint16_t block = 0; block < 2; ++block) {
          if (!server.is_connected())
            break;
          uint16_t got = server.receive(bytes, 1024);
          for (uint16_t i = 0; i < got; i++) {
            samples[block][i] =
                bytes[i] << 4; // form 12 bit sample from 8 bits bit sample
          }
          audio_output.output_samples(samples[block], got);
        }
      }
      printf("server connection closed\n");
    }
  }

  cyw43_arch_deinit();
  return 0;
}
