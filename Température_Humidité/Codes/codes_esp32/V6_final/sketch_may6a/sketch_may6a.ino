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
// SEUILS DE DECLENCHEMENT
// ==============================
// CONSEIL : Tu peux baisser temporairement SEUIL_TEMP a 15.0 pour tester 
// le passage du ventilateur a 1 (Allume) dans ta base de donnees.
const float SEUIL_TEMP = 23.0;
const float SEUIL_HUMI = 60.0;

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
      Serial.println("\n[WIFI] Timeout - pas de connexion !");
      return;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WIFI] Connecte ! IP : " + WiFi.localIP().toString());
}

// ==============================
// FONCTION : Envoi API (CORRIGEE)
// ==============================
void envoyerDonnees(float t, float h, int etatVentilo) {
  // Reconnexion automatique si WiFi perdu
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WIFI] Connexion perdue, reconnexion...");
    connecterWiFi();
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // SYNTAXE CORRIGEE : Plus de guillemets/antislashs superflus autour des deux-points
    String json = "{\"temperature\":" + String(t, 1) +
                  ",\"humidite\":"    + String(h, 1) +
                  ",\"zone\":1" +
                  ",\"ventilateur\":" + String(etatVentilo) + "}";

    int code = http.POST(json);

    if (code > 0) {
      Serial.printf("[API] Reponse : %d\n", code);
    } else {
      Serial.printf("[API] Erreur envoi : %s\n", http.errorToString(code).c_str());
    }

    http.end();
  } else {
    Serial.println("[API] Envoi annule - pas de WiFi");
  }
}

// ==============================
// SETUP
// ==============================
void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);
  digitalWrite(M2A, LOW);        // Moteur eteint au demarrage
  digitalWrite(M2B, LOW);

  Serial.println("\n--- CONFIGURATION TERMINEE ---");
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
    Serial.println("Verifier le cablage et la resistance pull-up.");
  } else {
    Serial.println("------------------------------------");
    Serial.printf("[CAPTEUR] Temp : %.1f°C | Hum : %.1f %%\n", t, h);
    Serial.printf("[SEUILS]  Temp > %.0f°C ou Hum > %.0f%%\n", SEUIL_TEMP, SEUIL_HUMI);

    // 2. CONTRÔLE VENTILATEUR & STOCKAGE DE L'ETAT
    int etatVentilo = 0;
    if (t > SEUIL_TEMP || h > SEUIL_HUMI) {
      digitalWrite(M2A, HIGH);
      digitalWrite(M2B, LOW);
      etatVentilo = 1; // Allume
      Serial.println("[MOTEUR] Ventilateur : ALLUME");
    } else {
      digitalWrite(M2A, LOW);
      digitalWrite(M2B, LOW);
      etatVentilo = 0; // Eteint
      Serial.println("[MOTEUR] Ventilateur : ETEINT");
    }

    // 3. ENVOI API (avec la variable etatVentilo)
    envoyerDonnees(t, h, etatVentilo);
  }

  // 4. PAUSE 60 SECONDES
  Serial.println("Attente 60 secondes...\n");
  delay(60000);
}