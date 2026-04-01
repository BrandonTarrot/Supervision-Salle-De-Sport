#include <WiFi.h>
#include <HTTPClient.h>

// --- Configuration Wi-Fi ---
const char* ssid = "PDA_ESP32_S";
const char* password = "Ciel123!";

// --- Configuration Serveur Flask (Raspberry Pi) ---
const char* serverIP = "172.19.6.120"; // L'IP du Raspberry
const int serverPort = 5000;           // Le port de Flask

// --- Capteur et LED ---
const int PHOTO_RESISTOR_PIN = 34;
const int LED_PIN = 5; 

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Connexion Wi-Fi
  Serial.print("Connexion a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connecte !");
  Serial.print("IP de l'ESP32 : ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // 1. Lire la valeur brute
  int adcValue = analogRead(PHOTO_RESISTOR_PIN);

  // 2. Conversion en Lux
  float voltage = adcValue * (3.3 / 4095.0);
  float lux = (voltage < 3.3) ? (500.0 * voltage) / (3.3 - voltage) : 1000.0;

  Serial.printf("Brut: %d | Lux: %.2f\n", adcValue, lux);

  // 3. Gestion de la LED locale
  if (adcValue < 600) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // 4. Envoi au serveur Flask via POST JSON
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // URL vers ton API Flask
    // Change bien l'IP, le port et la route
    String url = "http://172.19.6.120:5000/api/luminosite?lux=" + String(lux) + "&zone=1";

    http.begin(url);
    
    // On specifie que l'on envoie du JSON
    http.addHeader("Content-Type", "application/json");

    // Preparation du corps JSON (doit correspondre aux data.get() de ton Python)
    String jsonPayload = "{\"luminosite\":" + String(lux) + ", \"zone\":\"1\"}";

    Serial.print("Envoi JSON : ");
    Serial.println(jsonPayload);

    // Envoi de la requete POST
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.printf("Reponse serveur : %d\n", httpResponseCode);
      String payload = http.getString();
      Serial.println("Message : " + payload);
    } else {
      Serial.printf("Erreur d'envoi HTTP : %s\n", http.errorToString(httpResponseCode).c_str());
    }
    
    http.end();
  }

  Serial.println("--------------------------");
  delay(60000); // On attend 60 secondes 
}