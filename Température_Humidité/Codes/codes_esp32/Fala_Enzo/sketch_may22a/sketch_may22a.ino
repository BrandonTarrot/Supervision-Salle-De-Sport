#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include "DHT.h"

// Configuration réseau
const char* ssid = "PDA_ESP32_S";
const char* password = "Ciel123!";
const char* serverIP = "172.19.6.120";
const int serverPort = 5000;

// Configuration du capteur DHT11
#define DHTPIN 33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Configuration des broches du moteur
const int M2A = 14;
const int M2B = 12;

// Configuration du capteur de luminosité et des NeoPixels
const int PHOTO_RESISTOR_PIN = 32;
const int NEOPIXEL_PIN = 15;
const int NUM_PIXELS = 2;

Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Seuils de déclenchement
const float SEUIL_TEMP = 23.0;
const float SEUIL_HUMI = 60.0;

void connectWiFi();
void handleLuminosity();
void handleTemperatureHumidity();

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);
  digitalWrite(M2A, LOW);
  digitalWrite(M2B, LOW);

  pixels.begin();
  pixels.clear();
  pixels.show();

  connectWiFi();
}

void loop() {
  handleLuminosity();
  handleTemperatureHumidity();

  Serial.println("--------------------------");
  delay(60000);
}

void handleLuminosity() {
  // Lecture du convertisseur analogique-numérique (0 - 4095)
  int sensorValue = analogRead(PHOTO_RESISTOR_PIN);
  // Calcul de la résistance du capteur Grove (diviseur de tension)
  float resistance = (float)(4095 - sensorValue) * 10 / sensorValue;
  // Conversion de la mesure en Lux (LDR GL5528)
  float lux = 10000 / pow((resistance * 10), 0.75);

  Serial.print("[LUMINOSITE] Brute: ");
  Serial.print(sensorValue);
  Serial.print(" | R: ");
  Serial.print(resistance);
  Serial.print(" K | Lux: ");
  Serial.println(lux);

  // Traitement des NeoPixels selon les seuils demandés
  if (lux < 200.0) {  
    for(int i = 0; i < NUM_PIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    }
    pixels.show();
    Serial.println("[NEOPIXEL] Alerte : Intensite basse (< 200 lx) -> Rouge");
  } 
  else if (lux < 500.0) {
    for(int i = 0; i < NUM_PIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 255, 0));
    }
    pixels.show();
    Serial.println("[NEOPIXEL] Statut : Intensite normale (200-500 lx) -> Vert");
  } 
  else {
    pixels.clear();
    pixels.show();
    Serial.println("[NEOPIXEL] Statut : Eclaire (> 500 lx) -> Eteint");
  }

  // Envoi HTTP vers l'API de luminosité
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + String(serverIP) + ":" + String(serverPort) + "/api/luminosite";
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"luminosite\":" + String(lux) + ", \"zone\":\"1\"}";
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.printf("[HTTP LUX] Code de retour : %d\n", httpResponseCode);
    }
    http.end();
  }
}

void handleTemperatureHumidity() {
  // Lecture du capteur temp-humi
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("[ERREUR] Lecture impossible sur le capteur DHT11 (GPIO 33)");
    return;
  }

  Serial.printf("[DHT11] Temp : %.1f°C | Hum : %.1f %%\n", t, h);

  // Commande de l'actionneur ventilation
  int etatVentilo = 0;
  if (t > SEUIL_TEMP || h > SEUIL_HUMI) {
    digitalWrite(M2A, HIGH);
    digitalWrite(M2B, LOW);
    etatVentilo = 1;
    Serial.println("[MOTEUR] Ventilateur : ALLUME");
  } else {
    digitalWrite(M2A, LOW);
    digitalWrite(M2B, LOW);
    etatVentilo = 0;
    Serial.println("[MOTEUR] Ventilateur : ETEINT");
  }

  // Envoi HTTP vers l'API température et humidité
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + String(serverIP) + ":" + String(serverPort) + "/api/temperature_humidite";
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"temperature\":" + String(t, 1) +
                         ",\"humidite\":"    + String(h, 1) +
                         ",\"zone\":1" +
                         ",\"ventilateur\":" + String(etatVentilo) + "}";
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.printf("[HTTP DHT] Code de retour : %d\n", httpResponseCode);
    }
    http.end();
  }
}

void connectWiFi() {
  Serial.print("Initialisation de la connexion Wi-Fi");
  WiFi.begin(ssid, password);

  unsigned long debut = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - debut > 15000) { // Timeout de sécurité à 15 secondes
      Serial.println("\n[WIFI] Timeout - Échec de la connexion");
      return;
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnexion établie.");
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());
}