#include <WiFi.h>
#include <HTTPClient.h>
#include <math.h> // Nécessaire pour log10() et pow()

// --- Configuration Wi-Fi ---
const char* ssid = "PDA_ESP32_S";
const char* password = "Ciel123!";
const char* server = "192.168.4.1"; 

// --- Constantes du Circuit (Pont Diviseur) ---
const int PHOTO_RESISTOR_PIN = 34;
const int LED_PIN = 5; 
const float V_IN = 3.3;          // Tension d'alim
const float R_FIXE = 0.82;       // 820 Ohms convertis en kOhms pour la formule
const int ADC_MAX = 4095;        // Résolution 12 bits

void setup() {
  Serial.begin(115200);
  Serial.println("Démarrage du système de supervision...");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); 

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnecté au Wi-Fi !");
  Serial.print("IP ESP32 : ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // 1. Lire la valeur brute (0 - 4095)
  int rawValue = analogRead(PHOTO_RESISTOR_PIN);

  // 2. Conversion en Tension (U)
  float U = (rawValue / (float)ADC_MAX) * V_IN;

  // 3. Calcul de la résistance Rldr (en kOhms)
  float Rldr;
  if (U < 3.28) { // Évite la division par zéro si U est trop proche de 3.3V
    Rldr = (U * R_FIXE) / (V_IN - U);
  } else {
    Rldr = 500.0; // Valeur très haute (obscurité)
  }

  // 4. Conversion en Lux (Basé sur l'équation log-log : R = 100 * L^-0.75)
  float lux = 0.0;
  if (Rldr > 0) {
    // Formule inverse : L = 10^((log10(Rldr) - 2) / -0.75)
    lux = pow(10, (log10(Rldr) - 2) / -0.75);
  }

  // 5. Gestion de la LED locale
  // Seuil de 100 Lux pour allumer la LED (ajustable selon tes besoins)
  if (lux < 100.0) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // 6. Affichage console
  Serial.printf("Brut: %d | Tension: %.2fV | Rldr: %.2fkOhm | Lux: %.2f lx\n", rawValue, U, Rldr, lux);

  // 7. Envoi des données au serveur Python
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.setTimeout(5000);
    
    char url[128];
    sprintf(url, "http://%s:5000/api/luminosite", server);
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    char payload[256];
    // On envoie la valeur lux calculée
    sprintf(payload, "{\"luminosite\": %.2f, \"unit\": \"lx\", \"raw_value\": %d}", lux, rawValue);

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.printf("Serveur atteint. Code : %d\n", httpResponseCode);
    } else {
      Serial.printf("Erreur d'envoi : %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  }

  delay(5000); // 2 minutes
}