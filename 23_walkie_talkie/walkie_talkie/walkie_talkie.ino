#include <Updater.h>
#include <Updater_Signing.h>

#include <WiFi.h>
#include <WiFiUdp.h>

#include "PWMAudio.h"
#include "ADCAudio.h"
#include "GetWifiConnection.h"

#include "pico/cyw43_arch.h" //needed to set regulator mode

arduino::IPAddress remoteIP;
unsigned int dataPort = 5005;       // local port to listen on
unsigned int advertisePort = 5006;  // local port to listen on
WiFiUDP Udp;

//audio input and buffer
ADCAudio audioInput;
int oversample = 16;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

//audio output and ping pong buffer
PWMAudio audioOutput;
uint16_t outputSamples[2][1024];
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

//use alaw expansion to fit convert single byte to 12-bit PCM
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

void exchangeData(WiFiClient client, bool isClient)
{
  int16_t dc = 2048;
  while(client.connected() && (!isClient || BOOTSEL))
  {
    //send audio data
    uint16_t *inputSamples;
    int32_t amplitude = 0;
    audioInput.input_samples(inputSamples);
    for(uint16_t i = 0; i<1024; ++i)
    {
      int16_t sample = inputSamples[i];
      dc = dc + ((sample - dc) >> 1);
      sample -= dc;
      amplitude += sample * sample;
      packetBuffer[i]=alaw_compress(sample);
    }
    client.write(packetBuffer, 1024);

    //get audio data
    if (client.available()) {
      // read the packet into packetBufffer
      for (uint16_t i = 0; i < 1024; i++) {
        // form 12 bit sample from 8 bits bit sample
        if(client.available())
        {
          uint16_t sample = alaw_expand(client.read())+2048;
          outputSamples[pingPong][i] = sample;
        } 
      }
      audioOutput.output_samples(outputSamples[pingPong], 1024);
      pingPong ^= 1;
    }
    UDPAdvertisement();
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
  Serial.println("Finding other walkie talkies on network");
  
  while(1)
  {
    //advertise presence by advertising
    UDPAdvertisement();

    //check for advertisements
    int packetSize = Udp.parsePacket();
    if(packetSize)
    {
      remoteIP = Udp.remoteIP();
      Serial.println();
      Serial.print("pairing with: ");
      Serial.println(remoteIP);
      break;
    }

    blink(2);
  }
}

void runServer() {
  Serial.println("server running waiting for connection");
  WiFiServer server(dataPort);
  server.begin();
  while(!BOOTSEL)
  {
    blink(3);
    WiFiClient client = server.accept();
    if(client)
    {
      if(client.connected())
      {
        Serial.println("connected");
        led_on();
        exchangeData(client, false);
        led_off();
        Serial.println("disconnected");
      }
      client.stop();
    }
    UDPAdvertisement();
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

      client.connect(remoteIP, dataPort);
      if(client)
      {
        Serial.println("connected");
        led_on();
        exchangeData(client, true);
        led_off();
        Serial.println("disconnected");
      }
      client.stop();
      UDPAdvertisement();
    }
}

void connectToWiFi() {
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

void setup() {

  Serial.begin(115200);
  connectToWiFi();

  //start UDP port and begin pairing
  Serial.printf("UDP server on port %d\n", advertisePort);
  Udp.begin(advertisePort);
  pair();

  //quiet regulator
  cyw43_arch_gpio_put(1, 1);
  audioInput.begin(16, 10000);
  audioOutput.begin(0, 10000);
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