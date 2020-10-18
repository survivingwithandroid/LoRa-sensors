#include <Arduino.h>

// LoRa include
#include <SPI.h>
#include <LoRa.h>

// display include
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Web server and WiFi
#include <WiFi.h>
#include "ESPAsyncWebServer.h"

// Define OLED PIN
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16

// LoRa pins
#define LORA_MISO 19
#define LORA_CS 18
#define LORA_MOSI 27
#define LORA_SCK 5
#define LORA_RST 14
#define LORA_IRQ 26 


// LoRa Band (change it if you are outside Europe according to your country)
#define LORA_BAND 866E6

// WiFi config
const char *SSID = "your_ssid";
const char *PWD = "your_pwd";


String currentTemperature;
String currentHumidity;
String currentLightIntensity;
String currentMoistureLevel;

Adafruit_SSD1306 display(128, 32, &Wire, OLED_RST);

// Web server running on port 80
AsyncWebServer server(80);

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(SSID);
  
  WiFi.begin(SSID, PWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    // we can even make the ESP32 to sleep
  }
 
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

void resetDisplay() {
  digitalWrite(OLED_RST, LOW);
  delay(25);
  digitalWrite(OLED_RST, HIGH);  
}

void initializeDisplay() {
  Serial.println("Initializing display...");
  
   Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    Serial.println("Failed to initialize the dispaly");
    for (;;);
  }

  Serial.println("Display initialized");
  display.clearDisplay();
  
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Welcome to LORA");
  
  display.setTextSize(1);
  display.println("Lora receiver");
  display.display();
}


void initLoRa() {
  Serial.println("Initializing LoRa....");
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
  // Start LoRa using the frequency
  int result = LoRa.begin(LORA_BAND);
  if (result != 1) {
    display.setCursor(0,10);
    display.println("Failed to start LoRa network!");
    for (;;);
  }

 // LoRa.onReceive(onReceive);
 // LoRa.receive();

  Serial.println("LoRa initialized");
  display.setCursor(0,15);
  display.println("LoRa network OK!");
  display.display();
  delay(2000);
}



// setup API resources
void setup_routing() {
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *req) {
    req->send_P(200, "text/plain", currentTemperature.c_str());
  });

  server.on("/humidity", HTTP_GET, [] (AsyncWebServerRequest *req) {
    req->send_P(200, "text/plain", currentHumidity.c_str());
  });

  server.on("/light", HTTP_GET, [] (AsyncWebServerRequest *req) {
    req->send_P(200, "text/plain", currentLightIntensity.c_str());
  });

  server.on("/moisture", HTTP_GET, [] (AsyncWebServerRequest *req) {
    req->send_P(200, "text/plain", currentMoistureLevel.c_str());
  });

  // start server
  server.begin();
}

void setup() {
  Serial.begin(9600);
  Serial.println("Setup LoRa Sender....");
  connectToWiFi();
  setup_routing();
  resetDisplay();
  initializeDisplay();
  initLoRa();
  
}

void loop() {
  
  //try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    //Serial.print("Received packet ");

    //read packet
    while (LoRa.available()) {
      currentTemperature = LoRa.readStringUntil('|');
      //Serial.print(currentTemperature);
      currentHumidity = LoRa.readStringUntil('|');
      //Serial.print(currentHumidity);
      currentLightIntensity = LoRa.readStringUntil('|');
      //Serial.print(currentLightIntensity);
       currentMoistureLevel = LoRa.readStringUntil('|');
    }

    display.clearDisplay();
    display.setCursor(0,1);
    display.print("Temp: ");
    display.println(currentTemperature);
    display.print("Hum: ");
    display.println(currentHumidity);
    display.print("Light: ");
    display.println(currentLightIntensity);
    display.print("Moisture: ");
    display.println(currentMoistureLevel);
    display.display();
  }
  
}