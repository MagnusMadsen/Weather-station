#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// ----------- WiFi og MQTT-indstillinger -----------
const char* ssid = "Waoo4920_8847";
const char* password = "ckdd4889";  // ← WiFi-adgangskode
const char* mqtt_server = "192.168.1.5"; // Din Ubuntu-server IP

WiFiClient espClient;
PubSubClient client(espClient);

// ----------- Pins -----------
#define LDR_PIN 33
#define RAIN_PIN 32
#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_BME280 bme;

// ----------- WiFi-forbindelse -----------
void setup_wifi() {
  delay(100);
  Serial.println("[WIFI] Forbinder til WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WIFI] Tilsluttet! IP: " + WiFi.localIP().toString());
}

// ----------- MQTT-genforbindelse -----------
void reconnect() {
  while (!client.connected()) {
    Serial.print("[MQTT] Forbinder...");
    if (client.connect("ESP32_vejrstation")) {
      Serial.println(" forbundet!");
    } else {
      Serial.print(" fejl, rc=");
      Serial.print(client.state());
      Serial.println(" → prøver igen om 5 sek.");
      delay(5000);
    }
  }
}

// ----------- Setup -----------
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!bme.begin(0x76)) {
    Serial.println("[FEJL] Kunne ikke finde BME280. Tjek forbindelsen!");
    while (1);
  }

  pinMode(LDR_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);

  Serial.println("[INFO] Systemet er klar.");
}

// ----------- Loop -----------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temperatur = bme.readTemperature();
  float fugtighed = bme.readHumidity();
  float tryk = bme.readPressure() / 100.0F;
  int lysValue = analogRead(LDR_PIN);
  int regnValue = analogRead(RAIN_PIN);

  client.publish("vejr/temperatur", String(temperatur).c_str());
  client.publish("vejr/fugtighed", String(fugtighed).c_str());
  client.publish("vejr/tryk", String(tryk).c_str());
  client.publish("vejr/lys", String(lysValue).c_str());
  client.publish("vejr/regn", String(regnValue).c_str());

  Serial.println("[MQTT] Data sendt:");
  Serial.printf("Temp: %.2f °C | Fugt: %.1f %% | Tryk: %.1f hPa\n", temperatur, fugtighed, tryk);
  Serial.printf("Lys: %d | Regn: %d\n", lysValue, regnValue);
  Serial.println("------------------------------\n");

  delay(3000);  // 3 sekunders interval
}
