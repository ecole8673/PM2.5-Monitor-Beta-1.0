#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define sensor A0
#define DHT11PIN 2
#define DHTTYPE DHT11

int gasLevel = 0;
String quality = "";

DHT dht(DHT11PIN, DHTTYPE);

void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  gasLevel = analogRead(sensor);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println("Gas Level:");
  display.setCursor(80, 10);
  display.println(gasLevel);

  display.setCursor(0, 30);
  display.println("Temp  :");
  display.setCursor(80, 30);
  display.println(t);
  display.setCursor(114, 30);
  display.println("C");

  display.setCursor(0, 50);
  display.println("RH    :");
  display.setCursor(80, 50);
  display.println(h);
  display.setCursor(114, 50);
  display.println("%");
  display.display();
}

void setup() {
  Serial.begin(9600);
  pinMode(sensor, INPUT);
  dht.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    Serial.println(F("SSD1306 allocation failed"));
    return;
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(50, 0);
  display.println("Air");
  display.setTextSize(1);
  display.setCursor(23, 20);
  display.println("Quality monitor");
  display.display();
  delay(1200);
  display.clearDisplay();
}

void loop() {
  display.clearDisplay();
  sendSensor();
  display.display();
  delay(2000);
}