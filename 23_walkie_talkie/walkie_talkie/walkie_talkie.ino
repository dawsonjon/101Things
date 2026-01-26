#include <Updater.h>
#include <Updater_Signing.h>

#include <WiFi.h>
#include <WiFiUdp.h>

#include <I2S.h>
#include "ADCAudio.h"
#include "GetWifiConnection.h"

#include "pico/cyw43_arch.h" //needed to set regulator mode

arduino::IPAddress remoteIP;
unsigned int dataPort = 5005;       // local port to listen on
unsigned int advertisePort = 5006;  // local port to listen on
WiFiUDP Udp;

//audio input and buffer
ADCAudio audioInput;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

//audio output and ping pong buffer

#define pBCLK 14
#define pWS (pBCLK+1)
#define pDOUT 13

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
          if(envelope > 50) gate_open = true;
          gain = 0.0f;
          smoothed_gain = 0.99f*smoothed_gain + 0.01f*gain;
        }
        packetBuffer[i]=alaw_compress(sample * smoothed_gain);
      }
      client.write(packetBuffer, ADC_BUFFER_SIZE);
      //Serial.print("rx time us ");
      //Serial.print(micros()-t0);
      //Serial.print(" ");
      //Serial.println(audioInput.get_overflow());
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
        //Serial.print("samples ");
        //Serial.print(samples_written/4);
        //Serial.print(" tx time us ");
        //Serial.println(micros()-t0);
        if(audioOutput.getUnderflow()) Serial.println("underflow");
        if(audioOutput.getOverflow()) Serial.println("overflow");
      }

    }
  
    UDPAdvertisement();
  }
}

void exchangeData(WiFiClient client, bool isClient)
{
  int16_t dc = 2048;
  while(client.connected() && (!isClient || BOOTSEL))
  {
      
    //send audio data
    uint16_t *inputSamples;
    audioInput.input_samples(inputSamples);

    if(inputSamples)
    {
      uint32_t t0 = micros();
      for(uint16_t i = 0; i<ADC_BUFFER_SIZE; ++i)
      {
        int16_t sample = inputSamples[i];
        dc = dc + ((sample - dc) >> 1);
        sample -= dc;
        packetBuffer[i]=alaw_compress(sample);
      }
      client.write(packetBuffer, ADC_BUFFER_SIZE);
      Serial.print("rx time us ");
      Serial.print(micros()-t0);
      Serial.print(" ");
      Serial.println(audioInput.get_overflow());
    }

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
          int16_t sample = alaw_expand(client.read()) << 2;
          //write same sample into L and R channel
          outputSamples[i] = sample;
          outputSamples[i+1] = sample; 
          
        }
     
        uint32_t samples_written = audioOutput.write((uint8_t*)outputSamples, bytesToSend);
        Serial.print("samples ");
        Serial.print(samples_written/4);
        Serial.print(" tx time us ");
        Serial.println(micros()-t0);
        if(audioOutput.getUnderflow()) Serial.println("underflow");
        if(audioOutput.getOverflow()) Serial.println("overflow");
      }

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
        //exchangeData(client, false);
        recvData(client);
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
        //exchangeData(client, true);
        sendData(client);
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