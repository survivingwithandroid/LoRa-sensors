#include <Arduino.h>

// LoRa include
#include <SPI.h>
#include <LoRa.h>

// display include
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// DHT11
#include <Adafruit_Sensor.h>
#include <DHT.h>

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

// DHT11
#define DHTTYPE DHT11
#define DHT_PIN 23

// TEMT6000 Light sensor
#define LIGHT_PIN 13

// Moisture sensor
#define MOISTURE_SENSOR_PIN 34
#define MOISTURE_THRESHOLD 700

DHT dht(DHT_PIN, DHTTYPE);

Adafruit_SSD1306 display(128, 32, &Wire, OLED_RST);

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
  display.println("Lora sender");
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
  Serial.println("LoRa initialized");
  display.setCursor(0,15);
  display.println("LoRa network OK!");
  display.display();
  delay(2000);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Setup LoRa Sender....");
  resetDisplay();
  initializeDisplay();
  initLoRa();
  dht.begin();

  pinMode(LIGHT_PIN,  INPUT); 
}

void loop() {
  
  // Read temperature
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  Serial.print("Temp:");
  Serial.println(t);
  Serial.print("Humidity:");
  Serial.println(h);

  // Read the Light intensity
  float intensity = analogRead(LIGHT_PIN); 
 
  Serial.print("Intensity:");
  Serial.println(intensity);

  float moisture_level = analogRead(MOISTURE_SENSOR_PIN);
  
  LoRa.beginPacket();
  LoRa.print(t);
  LoRa.print("|");
  LoRa.print(h);
  LoRa.print("|");
  LoRa.print(intensity);
  LoRa.print("|");
  LoRa.print(moisture_level);
  LoRa.print("|");
  LoRa.endPacket();
  display.clearDisplay();
  display.setCursor(0,1);
  display.print("Temp: ");
  display.println(t);
  display.print("Hum: ");
  display.println(h);
  display.print("Light: ");
  display.println(intensity);
  display.print("Moisture: ");
  display.println(moisture_level);
  display.display();
  
  delay(2000);
}