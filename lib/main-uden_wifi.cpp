
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// ------------------- PINDEFINERINGER -------------------
#define LDR_PIN 15         // LDR til analog pin 15
#define RAIN_PIN 2        // Regnsensor til analog pin 17
#define SDA_PIN 21         // I2C SDA (valgfrit, default for ESP32)
#define SCL_PIN 22         // I2C SCL (valgfrit, default for ESP32)

// ------------------- SENSOR OBJEKTER -------------------
Adafruit_BME280 bme;       // BME280 objekt

// ------------------- SETUP -------------------
void setup() {
  Serial.begin(115200);
  delay(1000); // Vent lidt på serielforbindelse

  // I2C initialisering (ikke nødvendigt med standard pins)
  Wire.begin(SDA_PIN, SCL_PIN);

  // BME280 opsætning dawd 
  bool status = bme.begin(0x76);  // Tjek om 0x76 virker, ellers prøv 0x77
  if (!status) {
    Serial.println("[FEJL] Kunne ikke finde BME280. Tjek ledninger og adresse!");
    while (1);
  }

  // Konfigurer analoge input pins
  pinMode(LDR_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);

  Serial.println("[INFO] Sensorer initialiseret. Starter målinger...");
}

// ------------------- LOOP -------------------
void loop() {
  // Læs BME280 værdier
  float temperatur = bme.readTemperature();
  float fugtighed = bme.readHumidity();
  float tryk = bme.readPressure() / 100.0F; // hPa

  // Læs analoge værdier
  int lysValue = analogRead(LDR_PIN);
  int regnValue = analogRead(RAIN_PIN);

  // Udskriv resultater
  Serial.println("------------------------------");
  Serial.print("Temperatur: "); Serial.print(temperatur); Serial.println(" °C");
  Serial.print("Fugtighed: "); Serial.print(fugtighed); Serial.println(" %");
  Serial.print("Tryk: "); Serial.print(tryk); Serial.println(" hPa");

  Serial.print("Lysniveau (LDR): "); Serial.println(lysValue);
  Serial.print("Regnsensor (analog): "); Serial.println(regnValue);
  Serial.println("------------------------------\n");

  delay(3000); // Vent 3 sekunder
}

