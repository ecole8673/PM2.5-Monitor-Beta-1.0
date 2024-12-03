#include <SoftwareSerial.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT11.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define sensor A0
#define DHT11PIN 2

int gasLevel = 0; // int variable for gas level
String quality = "";
dht DHT;

// PMS7003 connection
#define PMS_RX 10 // Define PMS7003 RX pin (connected to Arduino TX)
#define PMS_TX 11 // Define PMS7003 TX pin (connected to Arduino RX)
SoftwareSerial pmsSerial(PMS_RX, PMS_TX);

struct PMSData {
  uint16_t pm1_0;
  uint16_t pm2_5;
  uint16_t pm10;
};

PMSData pmsData;

void sendSensor() {
  int readData = DHT.read11(DHT11PIN);
  float h = DHT.humidity;
  float t = DHT.temperature;

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setFont();
  display.setCursor(0, 43);
  display.println("Temp  :");
  display.setCursor(80, 43);
  display.println(t);
  display.setCursor(114, 43);
  display.println("C");
  display.setCursor(0, 56);
  display.println("RH    :");
  display.setCursor(80, 56);
  display.println(h);
  display.setCursor(114, 56);
  display.println("%");
}

void air_sensor() {
  gasLevel = analogRead(sensor);

  if (gasLevel < 151) {
    quality = "  GOOD!";
  } else if (gasLevel > 151 && gasLevel < 200) {
    quality = "  Poor!";
  } else if (gasLevel > 200 && gasLevel < 300) {
    quality = "Very bad!";
  } else if (gasLevel > 300 && gasLevel < 500) {
    quality = "Toxic!";
  } else {
    quality = " Toxic";
  }

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(1, 5);
  display.setFont();
  display.println("Air Quality:");
  display.setTextSize(1);
  display.setCursor(5, 23);
  display.println(gasLevel);
  display.setCursor(20, 23);
  display.println(quality);
}

bool readPMS(PMSData &data) {
  if (pmsSerial.available() >= 32) {
    uint8_t buffer[32];
    pmsSerial.readBytes(buffer, 32);

    if (buffer[0] == 0x42 && buffer[1] == 0x4D) { // Check start bytes
      data.pm1_0 = (buffer[10] << 8) | buffer[11];
      data.pm2_5 = (buffer[12] << 8) | buffer[13];
      data.pm10 = (buffer[14] << 8) | buffer[15];
      return true;
    }
  }
  return false;
}

void displayPMSData(const PMSData &data) {
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.println("PM2.5 :");
  display.setCursor(50, 30);
  display.println(data.pm2_5);
  display.setCursor(90, 30);
  display.println("ug/m3");
}

void setup() {
  Serial.begin(9600);
  pmsSerial.begin(9600); // PMS7003 baud rate
  pinMode(sensor, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
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

  display.setTextSize(1.5);
  display.setCursor(20, 20);
  display.println("BY Circuit");
  display.setCursor(20, 40);
  display.println("Digest");
  display.display();
  delay(1000);
  display.clearDisplay();
}

void loop() {
  display.clearDisplay();
  air_sensor();
  sendSensor();

  if (readPMS(pmsData)) {
    displayPMSData(pmsData);
  }

  display.display();
}
