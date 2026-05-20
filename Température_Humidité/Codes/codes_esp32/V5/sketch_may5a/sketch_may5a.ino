#include <WiFi.h>        // gestion du WiFi
#include <HTTPClient.h>  // requêtes HTTP
#include "DHT.h"         // capteur DHT

// ==============================
// CONFIGURATION WIFI
// ==============================
const char* ssid = "PDA_ESP32_S";      // nom du réseau WiFi
const char* password = "Ciel123!";     // mot de passe

// ==============================
// CONFIGURATION CAPTEUR
// ==============================
#define DHTPIN 33        // broche GPIO du DHT11
#define DHTTYPE DHT11    // type capteur

DHT dht(DHTPIN, DHTTYPE); // création objet capteur

// ==============================
// CONFIGURATION VENTILATEUR
// ==============================
#define RELAY_PIN 25      // broche relais

float SEUIL_TEMP = 25.0;  // seuil température
float SEUIL_HUMI = 70.0;  // seuil humidité

// ==============================
// CONFIGURATION SERVEUR API
// ==============================
const char* serverUrl = "http://172.19.6.120:5000/api/temperature_humidite";

// ==============================
// SETUP (démarrage)
// ==============================
void setup() {

  Serial.begin(115200);  // vitesse série (plus stable que 9600)
  delay(1000);

  dht.begin();           // initialisation capteur

  pinMode(RELAY_PIN, OUTPUT);   // config relais
  digitalWrite(RELAY_PIN, LOW); // ventilateur OFF au départ

  Serial.println("\n--- DEMARRAGE DU SYSTEME ---");

  // connexion WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi");

  int retry = 0;

  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[OK] WiFi connecté !");
    Serial.print("IP ESP32 : ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[ERREUR] WiFi non connecté → mode local");
  }
}

// ==============================
// LOOP (boucle principale)
// ==============================
void loop() {

  // ==========================
  // 1. LECTURE CAPTEUR
  // ==========================
  float h = dht.readHumidity();     // lecture humidité
  float t = dht.readTemperature();  // lecture température

  // vérification des données
  if (isnan(h) || isnan(t)) {
    Serial.println("[ERREUR] Lecture DHT11 échouée !");
    delay(2000);
    return;
  }

  Serial.println("------------------------------------");
  Serial.printf("Température : %.1f °C\n", t);
  Serial.printf("Humidité    : %.1f %%\n", h);

  // ==========================
  // 2. GESTION VENTILATEUR
  // ==========================
  if (t > SEUIL_TEMP || h > SEUIL_HUMI) {
    digitalWrite(RELAY_PIN, HIGH);   // ON
    Serial.println("VENTILATEUR : ON (seuil dépassé)");
  } else {
    digitalWrite(RELAY_PIN, LOW);    // OFF
    Serial.println("VENTILATEUR : OFF");
  }

  // ==========================
  // 3. ENVOI API
  // ==========================
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    http.begin(serverUrl);  // connexion API

    http.addHeader("Content-Type", "application/json"); // format JSON

    // création JSON
    String jsonPayload = "{";
    jsonPayload += "\"temperature\":" + String(t) + ",";
    jsonPayload += "\"humidite\":" + String(h) + ",";
    jsonPayload += "\"zone\":1";
    jsonPayload += "}";

    // envoi POST
    int httpResponseCode = http.POST(jsonPayload);

    // vérification réponse serveur
    if (httpResponseCode > 0) {
      Serial.print("API OK - Code : ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("API ERREUR - Code : ");
      Serial.println(httpResponseCode);
    }

    http.end(); // fermer connexion

  } else {
    Serial.println("WiFi non connecté → pas d'envoi API");
  }

  // ==========================
  // 4. TEMPS D'ATTENTE
  // ==========================
  delay(10000); // 10 secondes
}