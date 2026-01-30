#include <WiFi.h>
#include <WiFiUdp.h>

#include <I2S.h>
#include "ADCAudio.h"
#include "GetWifiConnection.h"
#include "WiFiManager.h"
#include "pico/cyw43_arch.h" //needed to set regulator mode


#define pBCLK 14
#define pWS (pBCLK+1)
#define pDOUT 13

//Non-Volatile Storage
const int eeprom_version = 401;
const int eeprom_address = 256;

enum e_connection_method {
  WIFI_MANAGER,
  PSUEDO_P2P
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
  PSUEDO_P2P,
  STA,
  false,
  0
};

//arduino::IPAddress remoteIP;
unsigned int dataPort = 5005;       // local port to listen on
unsigned int advertisePort = 5006;  // local port to listen on
unsigned int peer_last_seen = 0;
WiFiUDP Udp;
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

void sendData(WiFiClient client)
{

  int16_t dc = 2048;
  float envelope = 0;
  bool gate_open = false;
  float gain = 0, smoothed_gain = 0;
  while(client.connected() && BOOTSEL)
  {
      
    //send audio data
    uint16_t *inputSamples;
    audioInput.input_samples(inputSamples);

    if(inputSamples)
    {
      if(audioInput.get_overflow()) Serial.println("overflow");
      uint32_t t0 = micros();
      for(uint16_t i = 0; i<ADC_BUFFER_SIZE; ++i)
      {
        int16_t sample = inputSamples[i];
        dc = dc + ((sample - dc) >> 1);
        sample -= dc;
        envelope = 0.9f*envelope + 0.1f*abs(sample);
        if(gate_open){
          if(envelope < 3) gate_open = false;
          gain = 1.0f;
          smoothed_gain = 0.95f*smoothed_gain + 0.05f*gain;
        } else {
          if(envelope > 20) gate_open = true;
          gain = 0.0f;
          smoothed_gain = 0.99f*smoothed_gain + 0.01f*gain;
        }
        packetBuffer[i]=alaw_compress(sample * smoothed_gain);
      }
      client.write(packetBuffer, ADC_BUFFER_SIZE);
    }
  
    UDPAdvertisement();
  }
}

void recvData(WiFiClient client)
{    
  while(client.connected())
  {
    //get audio data
    if (client.available()) {
      uint32_t bytesToSend = audioOutput.availableForWrite();
      if(bytesToSend > ADC_BUFFER_SIZE*4) bytesToSend = ADC_BUFFER_SIZE*4;
      if(bytesToSend)
      {
        uint32_t t0 = micros();
        // read the packet into packetBufffer
        for (uint16_t i = 0; i < bytesToSend/2; i+=2) {
          // form 12 bit sample from 8 bits bit sample
          int16_t sample = alaw_expand(client.read()) << 4;
          //write same sample into L and R channel
          outputSamples[i] = sample;
          outputSamples[i+1] = sample; 
          
        }
     
        uint32_t samples_written = audioOutput.write((uint8_t*)outputSamples, bytesToSend);
        if(audioOutput.getUnderflow()) Serial.println("underflow");
        if(audioOutput.getOverflow()) Serial.println("overflow");
      }

    }
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
    Serial.println();
    Serial.print("pairing with: ");
    Serial.println(remoteIP);
    peer_last_seen = millis();
    if(remoteIP != settings.remoteIP){
      settings.remoteIP = remoteIP;
      save();
    }
  }
}

void runServer() {
  Serial.println("server running waiting for connection");
  WiFiServer server(dataPort);
  server.begin();
  while(!BOOTSEL)
  {
    pair();
    if(millis() - peer_last_seen < 3000) {
      blink(3);
    } else {
      blink(2);
    }

    WiFiClient client = server.accept();
    if(client)
    {
      if(client.connected())
      {
        Serial.println("connected");
        led_on();
        recvData(client);
        led_off();
        Serial.println("disconnected");
      }
      client.stop();
      peer_last_seen = millis();
    }

    if(settings.connection_method == PSUEDO_P2P && settings.p2p_role == STA && millis()-peer_last_seen > 3000) {
      Serial.println("no connection to AP, reboot");
      reboot();
    }

  }
  Serial.println("server stopped");
  server.stop();
}

void runClient()
{
    Serial.println("connecting to server");
    while(BOOTSEL)
    {
      WiFiClient client;

      client.connect(settings.remoteIP, dataPort);
      if(client)
      {
        Serial.println("connected");
        led_on();
        sendData(client);
        led_off();
        Serial.println("disconnected");
      }
      client.stop();
      UDPAdvertisement();
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
  sleep_ms(10000);

  Serial.println("PiPico Walkie Talkie");
  load();


  Serial.println(settings.connection_method);
  if(settings.connection_method == WIFI_MANAGER) {
    Serial.println("Use WIFI Manager");
    GetWIFIWIFIManager();
  }
  else if(settings.connection_method == PSUEDO_P2P) {
    Serial.println("Use PSUEDO P2P");
    GetWIFIP2P();
  }

  Udp.begin(advertisePort);

  gpio_init(0);
  gpio_set_dir(0, GPIO_OUT);

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
    runClient();
  }
  else
  {
    runServer();
  }

}