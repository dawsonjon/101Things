#include <EEPROM.h>
#include <WiFi.h>
#include <Arduino.h>

static const uint16_t ssid_address = 1u;
static const uint16_t password_address = ssid_address + 33u;

void loadWiFi(char *ssid, char *password)
{
  for(uint16_t i=0; i<33; i++)
  {
    unsigned char val = EEPROM.read(i+ssid_address);
    ssid[i] = val;
    if(val == 0) break;
  }
  for(uint16_t i=0; i<64; i++)
  {
    unsigned char val = EEPROM.read(i+password_address);
    password[i] = val;
    if(val == 0) break;
  }
  return;
}

void saveWiFi(char *ssid, char *password)
{
  //save wifi settings
  EEPROM.write(0, 0xaa); //indicates stored value
  for(uint16_t i=0; i<33; i++)
  {
    unsigned char val = ssid[i];
    EEPROM.write(i+ssid_address, val);
    if(val == 0) break;
  }
  for(uint16_t i=0; i<64; i++)
  {
    unsigned char val = password[i];
    EEPROM.write(i+password_address, val);
    if(val == 0) break;
  }
  EEPROM.commit();
}

void scanWiFi(char *ssid, char *password)
{
  while(1)
  {
    auto cnt = WiFi.scanNetworks();
    if (!cnt) {
      Serial.printf("No networks found\n");
    } else {
      Serial.printf("Found %d networks\n", cnt);
      for (auto i = 0; i < cnt; i++) {
        Serial.print(i);
        Serial.print(" ");
        Serial.println(WiFi.SSID(i));
      }
    }
    Serial.println("Select WiFi Network:");
    Serial.setTimeout(100000);
    int network = Serial.parseInt();
    Serial.println(network);
    if(network > cnt) continue;
    if(network <= 0) continue;
    strcpy(ssid, WiFi.SSID(network));
    while(Serial.available()) Serial.read();
    Serial.println("Enter Password:");
    Serial.setTimeout(100000);
    Serial.readBytesUntil('\n', password, 64);
    return;
  }
}

void getWiFiConnection(char *ssid, char *password)
{
  delay(1000); 
  EEPROM.begin(512);
  if(EEPROM.read(0) == 0xaa)
  {
    Serial.println("Reading WIFI connection from EEPROM");
    loadWiFi(ssid, password);
  }
  else
  {
    Serial.println("Scanning for WiFi...\n");
    scanWiFi(ssid, password);
    saveWiFi(ssid, password);
  }
  EEPROM.end();
}