#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// ==============================
// CONFIGURATION WIFI
// ==============================
const char* ssid     = "PDA_ESP32_S";
const char* password = "Ciel123!";

// ==============================
// CAPTEUR (Port Grove D33)
// ==============================
#define DHTPIN  33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ==============================
// MOTEUR (Borniers M2A / M2B)
// ==============================
#define M2A 14
#define M2B 12

// ==============================
// SEUILS DE DÉCLENCHEMENT
// ==============================
const float SEUIL_TEMP = 25.0;
const float SEUIL_HUMI = 70.0;

// ==============================
// API
// ==============================
const char* serverUrl = "http://172.19.6.120:5000/api/temperature_humidite";

// ==============================
// FONCTION : Connexion WiFi
// ==============================
void connecterWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi");
  unsigned long debut = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - debut > 15000) {        // Timeout 15 secondes
      Serial.println("\n[WIFI] Timeout — pas de connexion !");
      return;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WIFI] Connecté ! IP : " + WiFi.localIP().toString());
}

// ==============================
// FONCTION : Contrôle moteur
// ==============================
void controlerVentilateur(float t, float h) {
  if (t > SEUIL_TEMP || h > SEUIL_HUMI) {
    digitalWrite(M2A, HIGH);
    digitalWrite(M2B, LOW);
    Serial.println("[MOTEUR] Ventilateur : ALLUMÉ");
  } else {
    digitalWrite(M2A, LOW);
    digitalWrite(M2B, LOW);
    Serial.println("[MOTEUR] Ventilateur : ÉTEINT");
  }
}

// ==============================
// FONCTION : Envoi API
// ==============================
void envoyerDonnees(float t, float h) {
  // Reconnexion automatique si WiFi perdu
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WIFI] Connexion perdue, reconnexion...");
    connecterWiFi();
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // JSON propre avec 1 décimale
    String json = "{\"temperature\":" + String(t, 1) +
                  ",\"humidite\":"    + String(h, 1) +
                  ",\"zone\":1}";

    int code = http.POST(json);

    if (code > 0) {
      Serial.printf("[API] Réponse : %d\n", code);
    } else {
      Serial.printf("[API] Erreur envoi : %s\n", http.errorToString(code).c_str());
    }

    http.end();
  } else {
    Serial.println("[API] Envoi annulé — pas de WiFi");
  }
}

// ==============================
// SETUP
// ==============================
void setup() {
  Serial.begin(115200);          // CORRIGÉ : 9600 trop lent pour l'ESP32
  delay(1000);

  dht.begin();

  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);
  digitalWrite(M2A, LOW);        // Moteur éteint au démarrage
  digitalWrite(M2B, LOW);

  Serial.println("\n--- CONFIGURATION TERMINÉE ---");
  Serial.println("Capteur : GPIO 33 | Moteur : M2A(14) / M2B(12)");

  connecterWiFi();
}

// ==============================
// LOOP
// ==============================
void loop() {

  // 1. LECTURE CAPTEUR
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("[ERREUR] Lecture impossible sur GPIO 33 !");
    Serial.println("Vérifier le câblage et la résistance pull-up.");
  } else {
    Serial.println("------------------------------------");
    Serial.printf("[CAPTEUR] Temp : %.1f °C | Hum : %.1f %%\n", t, h);
    Serial.printf("[SEUILS]  Temp > %.0f°C ou Hum > %.0f%%\n", SEUIL_TEMP, SEUIL_HUMI);

    // 2. CONTRÔLE VENTILATEUR
    controlerVentilateur(t, h);

    // 3. ENVOI API
    envoyerDonnees(t, h);
  }

  // 4. PAUSE 60 SECONDES
  Serial.println("Attente 60 secondes...\n");
  delay(60000);
}