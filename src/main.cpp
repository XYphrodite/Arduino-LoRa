#include <Arduino.h>
#include <SPI.h>
#include "LoraSx1262.h"

LoraSx1262 radio;
int temperature = 20;
int humidity = 45;
//const byte control = 48;

//#define RX
#ifndef RX
#define TX
#endif

#ifdef RX

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define RADIO_BUFF_SIZE 15

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

byte receiveBuff[RADIO_BUFF_SIZE];

int counter = 0;

void showText();

void setup()
{
  memset(receiveBuff, 0, RADIO_BUFF_SIZE);
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("LoRa reciever");
  delay(50);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  else
  {
    Serial.println("Dispaly success");
  }
  delay(1000);
  if (!radio.begin())
  { // Initialize the radio
    Serial.println("Failed to initialize radio");
    for (;;)
      ;
  }
  else
  {
    Serial.println("Radio success");
  }
  delay(1000);
}

void loop()
{
  int bytesRead = radio.lora_receive_async(receiveBuff, sizeof(receiveBuff));
  Serial.print("Bytes read: ");
  Serial.println(bytesRead);
  if (bytesRead == 2)
  {
    // Print the payload out over serial
    Serial.print("Received: ");
    // Serial.write(receiveBuff, bytesRead);
    // byte r_c = receiveBuff[0];
    // if (r_c != control)
    // {
    //   short shift_count = 0;
    //   while (r_c != control)
    //   {
    //     r_c = (r_c >> 1);
    //     shift_count++;
    //     if (shift_count >= 4)
    //       break;
    //   }
    //   if (shift_count!=0 && shift_count < 4)
    //   {
    //     receiveBuff[3] = 0;
    //     uint32_t tmp = *(uint32_t*)receiveBuff;
    //     tmp = tmp >> shift_count;
    //     memcpy(receiveBuff, &tmp,4);
    //   }
    //   else {
        
    //   }
    // }
    //Serial.print(receiveBuff[0]);
    //Serial.print(" ");
    temperature = receiveBuff[0];
    Serial.print(temperature); // Add a newline after printing
    Serial.print(" ");
    humidity = receiveBuff[1];
    Serial.print(humidity);
    Serial.println();
    counter++;
    showText();
  }
  delay(100);
}

void showText()
{
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.print("Pack: ");
  display.println(counter);
  display.print("Temp: ");
  display.print(temperature);
  display.println("'C");
  display.print("Hum:  ");
  display.print(humidity);
  display.println("%");
  display.display();
}

#endif
#ifdef TX

#include "DHT.h"

#define DHTPIN 4 // Digital pin connected to the DHT sensor

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

#define PAYLOAD_SIZE 2

DHT dht;
byte payload[PAYLOAD_SIZE];

void setup()
{
  // Initialize device.
  dht.setup(DHTPIN);
  memset(payload, 0, PAYLOAD_SIZE);
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Booted");

  if (!radio.begin())
  { // Initialize radio
    Serial.println("Failed to initialize radio.");
    for (;;)
      ;
  }
  else
  {
    Serial.println("Radio success");
  }
}

void loop()
{
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.println(dht.toFahrenheit(temperature), 1);
  // if (isnan(event.temperature))
  // {
  //   Serial.println(F("Error reading temperature!"));
  // }
  // dht.humidity().getEvent(&event);
  // if (isnan(event.relative_humidity))
  // {
  //   Serial.println(F("Error reading humidity!"));
  // }
  // temperature = (int)event.temperature;
  // humidity = (int)event.relative_humidity;
  //payload[0] = control;
  payload[0] = temperature;
  payload[1] = humidity;
  Serial.print("Transmitting... ");
  Serial.print(temperature);
  Serial.print("  ");
  Serial.print(humidity);
  radio.transmit(payload, PAYLOAD_SIZE);
  Serial.println(" Done!");

  delay(1000);
}
#endif
