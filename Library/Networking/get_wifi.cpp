#include <algorithm>
#include <stdlib.h>
#include <string.h>

#include "pico/cyw43_arch.h"
#include "pico/multicore.h"
#include <hardware/flash.h>
extern "C" {
#include "dhcpserver.h"
}
#include "get_wifi.h"

static const uint32_t MAX_NUM_SSIDS = 256;
struct s_wifi_access_points {
  char ssids[MAX_NUM_SSIDS][33];
  uint8_t num_ssids = 0;
};

static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
  s_wifi_access_points *access_points = (s_wifi_access_points *)env;
  if (result) {
    bool match = false;
    for (uint8_t idx = 0; idx < access_points->num_ssids; ++idx) {
      if (strncmp(access_points->ssids[idx], (char *)result->ssid, 32) == 0) {
        // overwrite existing
        match = true;
        break;
      }
    }
    if (!match) {
      if (access_points->num_ssids < MAX_NUM_SSIDS) {
        strncpy(access_points->ssids[access_points->num_ssids++],
                (char *)result->ssid, 32);
      } else {
        // no space to store discard
      }
    }
  }
  return 0;
}

bool get_wifi_ap(const char *ap_name, const char *password) {
  if (cyw43_arch_init())
    return false;

  cyw43_arch_enable_ap_mode(ap_name, password, CYW43_AUTH_WPA2_AES_PSK);
  ip4_addr_t gw, mask;
  IP4_ADDR(&gw, 192, 168, 4, 1);
  IP4_ADDR(&mask, 255, 255, 255, 0);

  // Start the dhcp server
  dhcp_server_t dhcp_server;
  dhcp_server_init(&dhcp_server, &gw, &mask);

  printf("WIFI Access Point Started\n");

  return true;
}

bool get_wifi() {
  if (cyw43_arch_init())
    return false;
  cyw43_arch_enable_sta_mode();

#ifdef WIFI_SSID
  char ssid[] = WIFI_SSID;
  char pass[] = WIFI_PASSWORD;
#else
  // map constant onto a sector of flash
  static const uint8_t __in_flash() __attribute__((aligned(FLASH_SECTOR_SIZE)))
  flash_sector[FLASH_SECTOR_SIZE] = {0xff};
  // The compiler "knows" that flash sector[0] contains 0xff, so replaces
  // accesses to flash_sector[0] with that value. To avoid this, cast to a
  // volatile pointer, then the compiler actually reads from the appropriate
  // location
  volatile uint8_t *flash_sector_address = (uint8_t *)&flash_sector[0];
  char ssid[33] = {0};
  char pass[64] = {0};

  // settings stored in flash
  printf("flash content %x\n", (int)flash_sector_address[0]);
  if (flash_sector_address[0] != 0xff) {
    printf("Retrieving stored WIFI settings...\n");
    strncpy(ssid, (char *)flash_sector_address, 32);
    strncpy(pass, (char *)flash_sector_address + 32, 63);
  }

  // settings not stored - scan wifi
  else {
    // delay to allow serial connection to be made
    sleep_ms(10000);

    printf("No stored wifi settings\n");
    printf("Scanning for access points...\n");

    cyw43_wifi_scan_options_t scan_options = {0};
    s_wifi_access_points wifi_access_points = {0};
    int err = cyw43_wifi_scan(&cyw43_state, &scan_options, &wifi_access_points,
                              scan_result);
    if (err == 0) {
      while (cyw43_wifi_scan_active(&cyw43_state)) {
#if PICO_CYW43_ARCH_POLL
        cyw43_arch_poll();
#endif
        sleep_ms(1);
      }
    }

    for (uint8_t idx = 0; idx < wifi_access_points.num_ssids; ++idx) {
      printf("%u %-32s\n", idx, wifi_access_points.ssids[idx]);
    }

    // prompt for ssid/password
    int idx;
    printf("Select ssid:\n");
    while (1) {
      char line[256];
      fgets(line, 256, stdin);
      if (sscanf(line, " %d", &idx) && idx < wifi_access_points.num_ssids &&
          idx >= 0)
        break;
    }
    strncpy(ssid, wifi_access_points.ssids[idx], 32);
    printf("Enter password for %.32s:\n", ssid);
    fgets(pass, 63, stdin);
    printf("%.63s\n", pass);
    pass[strcspn(pass, "\r\n")] = 0; // remove newline

    // store SSID for next time
    uint8_t sector_to_write[FLASH_SECTOR_SIZE] = {0};
    strncpy((char *)sector_to_write, ssid, 32);
    strncpy((char *)sector_to_write + 32, pass, 63);

    const uint32_t address = (uint32_t) & (flash_sector[0]);
    const uint32_t flash_address = address - XIP_BASE;
    //!!! PICO is **very** fussy about flash erasing, there must be no code
    //!running in flash.  !!!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    const uint32_t ints =
        save_and_disable_interrupts(); // disable all interrupts

    // safe to erase flash here
    //--------------------------------------------------------------------------------------------
    flash_range_erase(flash_address, FLASH_SECTOR_SIZE);
    flash_range_program(flash_address, sector_to_write, FLASH_SECTOR_SIZE);
    //--------------------------------------------------------------------------------------------

    restore_interrupts(ints); // restore interrupts
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //!!! Normal operation resumed
  }

#if PICO_CYW43_ARCH_POLL
  cyw43_arch_poll();
#endif

#endif

  printf("Connecting to wifi...");
  if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK,
                                         30000))
    return false;
  printf("connected\n");
  return true;
}
