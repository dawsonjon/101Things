#include <WiFi.h>
#include <WiFiUdp.h>

#include "PWMAudio.h"
#include "ADCAudio.h"

#include "pico/cyw43_arch.h"

#ifndef STASSID
#define STASSID
#define STAPSK
#endif

//const char remoteIP[] = "192.168.1.101";
arduino::IPAddress remoteIP;
unsigned int dataPort = 5005;  // local port to listen on
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

void blink_slow()
{
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(500);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(500);                      // wait for a second
}

void blink_fast()
{
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(100);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(100);                      // wait for a second
}

void led_on()
{
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
}

void led_off()
{
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
}

void setup() {

  Serial.begin(115200);
  WiFi.begin(STASSID, STAPSK);
    while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    blink_slow();
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", advertisePort);
  
  Serial.println("pairing....");
  Udp.begin(advertisePort);
  while(1)
  {
    //advertise presence by advertising
    Udp.beginPacket(IPAddress(255, 255, 255, 255), advertisePort);
    Udp.write("walkie talkie advertisement");
    Udp.endPacket();

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

    Serial.print(".");
    blink_fast();

  }
  //quite regulator
  cyw43_arch_gpio_put(1, 1);
  Serial.println("connecting....");  
  audioInput.begin(16, 10000);
  audioOutput.begin(0, 10000);
}

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
    audioInput.input_samples(inputSamples);
    for(uint16_t i = 0; i<1024; ++i)
    {
      int16_t sample = inputSamples[i];
      dc = dc + ((sample - dc) >> 1);
      packetBuffer[i]=alaw_compress(sample-dc);
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
  }
}

long lastAdvertisementTime = 0;
void loop() {
  
  if(BOOTSEL)
  {
    Serial.println("connecting to server");
    WiFiClient client;
    client.connect(remoteIP, dataPort);
    if(client)
    {
      exchangeData(client, true);
    }
    client.stop(); 
  }
  else
  {
    Serial.println("waiting for connection");
    WiFiServer server(dataPort);
    server.begin();
    delay(1000);
    WiFiClient client = server.accept();
    if(client)
    {
      if(client.connected())
      {
        Serial.println("connected");
        exchangeData(client, false);
        Serial.println("disconnected");
      }
      client.stop();
    }
    server.stop();
  }

  delay(1000);


  if(millis() - lastAdvertisementTime > 1000)
  {
    lastAdvertisementTime = millis();
    //continue advertising
    Serial.println("advert");
    Udp.beginPacket(IPAddress(255, 255, 255, 255), advertisePort);
    Udp.write("walkie talkie advertisement");
    Udp.endPacket();
  }

}