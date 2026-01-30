#include <WiFi.h>
#include <WiFiUdp.h>

#include <I2S.h>
#include "ADCAudio.h"
#include "GetWifiConnection.h"
#include "WiFiManager.h"
#include "pico/cyw43_arch.h" //needed to set regulator mode

#define pDOUT 0
#define pBCLK 1
#define pWS (pBCLK+1)


//Non-Volatile Storage
const int eeprom_version = 400;
const int eeprom_address = 256;

enum e_connection_method {
  WIFI_MANAGER,
  P2P
};

enum e_p2p_role {
  AP,
  STA
};

struct s_settings {
  uint32_t version;
  e_connection_method connection_method;
  e_p2p_role p2p_role;
  bool p2p_initialised;
  arduino::IPAddress remoteIP;
};

s_settings settings = {
  eeprom_version,
  WIFI_MANAGER,
  STA,
  false,
  0
};

//arduino::IPAddress remoteIP;
unsigned int dataPort = 5005;       // local port to listen on
unsigned int advertisePort = 5006;  // local port to listen on
unsigned int peer_last_seen = 0;
unsigned int peer_last_audio = 0;
WiFiUDP Udp;
WiFiUDP UdpData;
bool wifi_up = false;

//audio input and buffer
ADCAudio audioInput;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

// Create the I2S port using a PIO state machine
I2S audioOutput(OUTPUT, pBCLK, pDOUT);
uint16_t outputSamples[ADC_BUFFER_SIZE*2];
uint8_t pingPong = 0;

void blink(uint8_t blinks)
{
  for(uint8_t blink=0; blink<blinks; blink++)
  {
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(100);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
    delay(100);                       // wait for a second
  }
  delay(500);
}

void led_on()
{
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
}

void led_off()
{
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED on (HIGH is the voltage level)
}

//use alaw compression to fit 12-bit pcm sample into a single byte
uint8_t alaw_compress(int16_t pcm_val) {
   const uint16_t ALAW_MAX = 0xFFF;
   uint16_t mask = 0x800;
   uint8_t sign = 0;
   uint8_t position = 11;
   uint8_t lsb = 0;
   if (pcm_val < 0)
   {
      pcm_val = -pcm_val;
      sign = 0x80;
   }
   if (pcm_val > ALAW_MAX)
   {
      pcm_val = ALAW_MAX;
   }
   for (; ((pcm_val & mask) != mask && position >= 5); mask >>= 1, position--);
   lsb = (pcm_val >> ((position == 4) ? (1) : (position - 4))) & 0x0f;
   return (sign | ((position - 4) << 4) | lsb) ^ 0x55;
}

//use alaw expansion to convert single byte to 12-bit PCM
int16_t alaw_expand(uint8_t u_val) {
   uint8_t sign = 0x00;
   uint8_t position = 0;
   int16_t decoded = 0;
   u_val^=0x55;
   if(u_val&0x80)
   {
      u_val&=~(1<<7);
      sign = -1;
   }
   position = ((u_val & 0xF0) >>4) + 4;
   if(position!=4)
   {
      decoded = ((1<<position)|((u_val&0x0F)<<(position-4))
                |(1<<(position-5)));
   }
   else
   {
      decoded = (u_val<<1)|1;
   }
   return (sign==0)?(decoded):(-decoded);
}

void sendData()
{

  int16_t dc = 2048;
  float envelope = 0;
  bool gate_open = false;
  float gain = 0, smoothed_gain = 0;

  while (BOOTSEL)
  {
    uint16_t *inputSamples;
    if(audioInput.get_overflow()) Serial.println("overflow in audio input");
    audioInput.input_samples(inputSamples);
    
    if (inputSamples)
    {
      for (uint16_t i = 0; i < ADC_BUFFER_SIZE; ++i)
      {
        int16_t sample = inputSamples[i];
        dc += (sample - dc) >> 1;
        sample -= dc;

        envelope = 0.9f * envelope + 0.1f * abs(sample);

        if (gate_open)
        {
          if (envelope < 3) gate_open = false;
          gain = 1.0f;
          smoothed_gain = 0.95f * smoothed_gain + 0.05f * gain;
        }
        else
        {
          if (envelope > 30) gate_open = true;
          gain = 0.0f;
          smoothed_gain = 0.99f * smoothed_gain + 0.01f * gain;
        }
        packetBuffer[i] = alaw_compress(sample * smoothed_gain);
      }


      //UDP unicast audio packet
      UdpData.beginPacket(settings.remoteIP, dataPort);
      UdpData.write(packetBuffer, ADC_BUFFER_SIZE);
      UdpData.endPacket();


    }

  }
}

void recvData()
{
  uint32_t inp = 0, outp = 0, fill=0;
  bool play = false;
  static uint8_t jitter_buffer[4][ADC_BUFFER_SIZE];
  while (!BOOTSEL)
  {
  
    int packetSize = UdpData.parsePacket();
    if (packetSize == ADC_BUFFER_SIZE)
    {  
      if(fill < 4) {
        UdpData.read(jitter_buffer[inp], ADC_BUFFER_SIZE);
        inp = (inp+1) & 3;
        fill++;
        peer_last_audio = peer_last_seen = millis();

      } else {
        while (UdpData.available()) UdpData.read(); //drop packet
        peer_last_audio = peer_last_seen = millis();
      }
      
    } else if (packetSize > 0) {
      // Drain junk
      while (UdpData.available()) UdpData.read();
      peer_last_seen = millis();
    }

    if(fill == 2) play = true;
    if(fill == 0) play = false;

    if(play && (audioOutput.availableForWrite()>=ADC_BUFFER_SIZE*4)) {
      for (uint16_t i = 0; i < ADC_BUFFER_SIZE*2; i += 2)
      {
        int16_t sample = alaw_expand(jitter_buffer[outp][i/2]) << 4;
        outputSamples[i]     = sample;
        outputSamples[i + 1] = sample;
      }

      if(audioOutput.getOverflow()) Serial.println("Overflow in audio output");
      if(audioOutput.getUnderflow()) Serial.println("Underflow in audio output");
      audioOutput.write((uint8_t*)outputSamples, ADC_BUFFER_SIZE * 4);
      
      outp = (outp+1) & 3;
      fill--;
    }

    if(millis()-peer_last_audio < 1000) {
      led_on();
    } else if(millis()-peer_last_seen < 3000) {
      blink(3);
    } else {
      blink(2);
    }

    if(settings.connection_method == P2P && settings.p2p_role == STA && millis()-peer_last_seen > 3000) {
      Serial.println("no connection to AP, reboot");
      reboot();
    }

    pair();
  }
}

void UDPAdvertisement()
{
  static long lastAdvertisementTime = 0;
  if(millis() - lastAdvertisementTime > 1000)
  {
    lastAdvertisementTime = millis();
    //continue advertising
    Udp.beginPacket(IPAddress(255, 255, 255, 255), advertisePort);
    Udp.write("walkie talkie advertisement");
    Udp.endPacket();
  }
}

void pair()
{
  //advertise presence by advertising
  UDPAdvertisement();

  //check for advertisements
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
    arduino::IPAddress remoteIP = Udp.remoteIP();
    Serial.print("paired with: ");
    Serial.println(remoteIP);
    peer_last_seen = millis();
    if(remoteIP != settings.remoteIP){
      settings.remoteIP = remoteIP;
      save();
    }
  }
}

void GetWIFIWIFIManager() {
  String title("Pico Walkie Talkie");
  String name("PICO_WALKIE_TALKIE");
  String shortname("WALKIE_TALKIE");
  String maker("101 Things");
  String version("0.0.1");

  WiFiManager wm("WALKIE_TALKIE", "password");
  wm.setContentText(title, name, shortname, maker, version);
  wm.autoConnect();
}

void GetWIFISerial() {
  char ssid[33];
  char password[64];
  getWiFiConnection(ssid, password);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    blink(1);
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void GetWIFIP2P() {
  const char *ssid = "pico_p2p";
  const char *password = "pico_p2p_pass";

  if(settings.p2p_initialised) {
    
    if(settings.p2p_role == STA) {
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      uint32_t start = millis();
      while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        if(millis()-start > 6000) reboot();
        blink(1);
      }
      Serial.println("Connected as STA");
      Serial.println(WiFi.localIP());
      return;
    }
    else
    {
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        blink(1);
      }
      Serial.println("Peer connected to AP");
      Serial.println(WiFi.softAPIP());
      return;
    }

  }

  while (1) {

    // 1. Try to connect as STA (fast)
    Serial.println("Trying STA...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED &&
           millis() - start < 6000) {
      blink(1);
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected as STA");
      Serial.println(WiFi.localIP());
      settings.p2p_initialised = true;
      settings.p2p_role = STA;
      save();
      return;
    }

    WiFi.disconnect(true);
    delay(200);

    // 2. Become AP (slow, patient)
    Serial.println("Becoming AP...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    start = millis();
    while (WiFi.softAPgetStationNum() == 0 &&
           millis() - start < 12000) {
      blink(1);
    }

    if (WiFi.softAPgetStationNum() > 0) {
      Serial.println("Peer connected to AP");
      Serial.println(WiFi.softAPIP());
      settings.p2p_initialised = true;
      settings.p2p_role = AP;
      save();
      return;
    }

    WiFi.softAPdisconnect(true);
    delay(500 + random(0, 1000));
    reboot();

  }
}

void reboot() {
  watchdog_enable(1, true);
  while (1) { tight_loop_contents(); }
}

void load() {
  s_settings temp_settings;
  EEPROM.get(eeprom_address, temp_settings);
  if(temp_settings.version == eeprom_version){
    settings = temp_settings;
  }
}

void save() {
  EEPROM.put(eeprom_address, settings);
  EEPROM.commit();
}

void setup() {

  Serial.begin(115200);
  EEPROM.begin(512);

  Serial.println();
  Serial.println("PiPico Walkie Talkie");
  load();

  if(settings.connection_method == WIFI_MANAGER) {
    Serial.println("Use WIFI Manager");
    GetWIFIWIFIManager();
  }
  else if(settings.connection_method == P2P) {
    Serial.println("Use P2P");
    GetWIFIP2P();
  }

  Udp.begin(advertisePort);
  UdpData.begin(dataPort);

  //quiet regulator
  cyw43_arch_gpio_put(1, 1);
  audioInput.begin(16, 10000);
  audioOutput.setBitsPerSample(16);
  if (!audioOutput.begin(10000)) {
    Serial.println("Failed to initialize I2S!");
    while (1); // do nothing
  }
  peer_last_seen = millis();
  Serial.println("setup complete");
}

void loop() {

  if(BOOTSEL)
  {
    led_on();
    sendData();
    led_off();
  }
  else
  {
    recvData();
  }

}