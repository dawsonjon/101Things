#include <stdio.h>

#include "PWMAudio.h"
#include "TCPSocket.h"
#include "get_wifi.h"
#include "pico/stdlib.h"
#include <math.h>

int main() {
  stdio_init_all();
  TCPSocket socket;
  PWMAudio audio_output(0, 10000);

  while (true) {

    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP) {
      printf("no wifi connection, trying to connect\n");
      get_wifi();
    }

    printf("Starting server at %s on port %u\n",
           ip4addr_ntoa(netif_ip4_addr(netif_list)), 4242);

    if (!socket.listen(4242)) {
      printf("couldn't start server\n");
      continue;
    }

    printf("server accepted connection\n");
    uint8_t bytes[1024];
    uint16_t samples[2][1024];

    while (socket.is_connected()) {
      for (uint16_t block = 0; block < 2; ++block) {
        if (!socket.is_connected())
          break;
        uint16_t got = socket.receive(bytes, 1024);
        for (uint16_t i = 0; i < got; i++) {
          samples[block][i] = bytes[i]
                              << 4; // form 12 bit sample from 8 bits bit sample
        }
        audio_output.output_samples(samples[block], got);
      }
    }

    printf("server connection closed\n");
  }

  cyw43_arch_deinit();
  return 0;
}
