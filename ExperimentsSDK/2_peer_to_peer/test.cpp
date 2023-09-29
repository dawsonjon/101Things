#include <stdio.h>

#include "secrets.h"
#include "pico/stdlib.h"
#include "TCPServer.h"
#include "PWMAudio.h"
#include <math.h>


int main() {
    stdio_init_all();
    char ssid[] = "PLUSNET-P82J";
    char pass[] = "6f67de2e59";

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Connecting to WiFi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect to WIFI\n");
        return 1;
    } else {
        printf("Connected to WIFI.\n");
    }


    uint16_t sin_lookup[1024];
    for(int i = 0; i<1024; ++i)
    {
        sin_lookup[i] = round(2047.0 * (sin((2.0 * M_PI * i)/1024.0) + 1));
        printf("%u\n", sin_lookup[i]);
    }
    

    TCPServer server;
    PWMAudio audioOutput(0, 10000);
    while(true)
    {
        printf("Starting server at %s on port %u\n", ip4addr_ntoa(netif_ip4_addr(netif_list)), 4242);
        if(!server.listen(4242))
        {
            printf("couldn't start server\n");
            continue;
        }
        printf("server waiting for connections\n");
        if(server.accept())
        {
            printf("server accepted connection\n");
            uint8_t bytes[1024];
            uint16_t samples[3][1024];
            sleep_ms(100);
            uint32_t phase = 0;
            while(server.isConnected()){

                for(uint16_t block=0; block < 3; ++block)
                {
                    if(!server.isConnected()) break;
                    uint16_t got = server.receive(bytes, 1024);
                    for(uint16_t i = 0; i<got; i++)
                    {
                        samples[block][i] = bytes[i]<<4; //form 12 bit sample from 8 bits bit sample
                        //samples[block][i] = sin_lookup[(phase >> 6) & 0x3FF];
                        phase += 1966;
                    }
                    if(got<1024) printf("%u \n", got);
                    audioOutput.OutputSamples(samples[block], got);
                }

               

            }
            printf("server connection closed\n");
        }
    }

    cyw43_arch_deinit();
    return 0;
}
