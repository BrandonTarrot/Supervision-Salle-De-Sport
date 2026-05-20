#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// --- Configuration Wi-Fi ---
const char* ssid = "PDA_ESP32_S";
const char* password = "Ciel123!";

// --- Configuration DHT ---
#define DHTPIN 33     // Le pin ou est connecte le DATA du DHT
#define DHTTYPE DHT11 // Ou DHT22 si tu as le modele blanc
DHT dht(DHTPIN, DHTTYPE);

// --- Configuration Serveur ---
const char* serverUrl = "http://172.19.6.120:5000/api/temperature_humidite";

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.print("Connexion au Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnecte !");
}

void loop() {
  // Lecture des vraies valeurs
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // On verifie si la lecture a echoue
  if (isnan(h) || isnan(t)) {
    Serial.println("Erreur de lecture du DHT !");
    delay(2000);
    return;
  }

  Serial.printf("Temp: %.1f C | Hum: %.1f %%\n", t, h);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Construction du JSON avec les vraies valeurs
    String jsonPayload = "{\"temperature\":" + String(t) + 
                         ", \"humidite\":" + String(h) + 
                         ", \"zone\":\"1\"}";

    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.print("Serveur a recu : ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erreur envoi : ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  }

  delay(60000); // Envoi toutes les 10 secondes pour les tests
}