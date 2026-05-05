#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define LARGEUR 128 
#define HAUTEUR 64 
Adafruit_SSD1306 ecran(LARGEUR, HAUTEUR, &Wire, -1);

const char* ssid = "PDA_ESP32_S";
const char* password = "Ciel123!";
const char* serverIP = "172.19.6.120";
const int serverPort = 5000;

const int PHOTO_RESISTOR_PIN = 32; 
const int LED_PIN = 2;
const int brochePIR = 25;
#define DHTPIN 33     
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);
int frame = 0; 
unsigned long dernierEnvoi = 0;

void dessinerMuscu(int x, int y, bool actif);

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(brochePIR, INPUT);
  dht.begin();
  Wire.begin(21, 22);
  ecran.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  ecran.clearDisplay();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
}

void loop() {
  int etatPresence = digitalRead(brochePIR);
  int sensorValue = analogRead(PHOTO_RESISTOR_PIN);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  float resistance = (float)(4095 - sensorValue) * 10 / sensorValue;
  float lux = 10000 / pow((resistance * 10), 0.75); 

  if (sensorValue < 2000) digitalWrite(LED_PIN, HIGH);
  else digitalWrite(LED_PIN, LOW);

  ecran.clearDisplay();
  ecran.setTextSize(1);
  ecran.setTextColor(SSD1306_WHITE);

  if (etatPresence == HIGH) {
    ecran.setCursor(12, 0); ecran.print("DETECTION PRESENTE");
    dessinerMuscu(64, 34, true);
  } else {
    ecran.setCursor(15, 0); ecran.print("AUCUNE DETECTION");
    dessinerMuscu(64, 34, false);
  }
  ecran.display();

  if (millis() - dernierEnvoi >= 60000) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String urlBase = "http://" + String(serverIP) + ":" + String(serverPort);
      
      http.begin(urlBase + "/api/luminosite");
      http.addHeader("Content-Type", "application/json");
      http.POST("{\"luminosite\":" + String(lux) + ", \"zone\":\"1\"}");
      http.end();

      http.begin(urlBase + "/api/temperature_humidite");
      http.addHeader("Content-Type", "application/json");
      http.POST("{\"temperature\":" + String(t) + ", \"humidite\":" + String(h) + ", \"zone\":\"1\"}");
      http.end();
      
      dernierEnvoi = millis();
    }
  }
  delay(150); 
}

void dessinerMuscu(int x, int y, bool actif) {
  ecran.drawLine(x-30, y+25, x+30, y+25, SSD1306_WHITE);
  if (actif) {
    frame = (frame + 1) % 4;
    int squatH = (frame == 1 || frame == 2) ? 8 : 0; 
    ecran.drawCircle(x, y + squatH, 4, SSD1306_WHITE);
    ecran.drawLine(x, y+4+squatH, x, y+15, SSD1306_WHITE);
    ecran.drawLine(x-12, y+5+squatH, x+12, y+5+squatH, SSD1306_WHITE);
    ecran.fillCircle(x-12, y+5+squatH, 2, SSD1306_WHITE);
    ecran.fillCircle(x+12, y+5+squatH, 2, SSD1306_WHITE);
  } else {
    ecran.drawCircle(x, y, 4, SSD1306_WHITE);
    ecran.drawLine(x, y+4, x, y+15, SSD1306_WHITE);
  }
}