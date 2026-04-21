#include <WiFi.h>          // bibliothèque pour utiliser le WiFi de l’ESP32
#include <HTTPClient.h>    // bibliothèque pour envoyer des requêtes HTTP
#include <DHT.h>           // bibliothèque pour le capteur DHT11

#define DHTPIN 4           // broche GPIO 4 utilisée pour le capteur
#define DHTTYPE DHT11      // type de capteur utilisé (DHT11)

DHT dht(DHTPIN, DHTTYPE);  // création de l’objet capteur DHT

const char* ssid = "PDA_ESP32_S";       // nom de ton réseau WiFi
const char* password = "Ciel123!";    // mot de passe du WiFi

// adresse de ton API (IP du Raspberry + route)
String serverName = "http://172.19.6.120:5000/api/temphumi";

void setup() {

  Serial.begin(115200);   // démarre la communication série (pour debug)
  dht.begin();            // initialise le capteur DHT11

  WiFi.begin(ssid, password);  // connexion au WiFi

  Serial.print("Connexion au WiFi");

  // boucle jusqu'à connexion
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);                 // attend 0.5 seconde
    Serial.print(".");          // affiche des points
  }

  Serial.println("\nConnecté au WiFi !"); // message de confirmation
}

void loop() {

  float h = dht.readHumidity();      // lecture de l'humidité
  float t = dht.readTemperature();   // lecture de la température

  // vérifie si les valeurs sont valides
  if (isnan(h) || isnan(t)) {
    Serial.println("Erreur de lecture du capteur DHT11");
    return;   // on sort de la boucle si erreur
  }

  // vérifie que le WiFi est toujours connecté
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;  // création d’un objet HTTP

    http.begin(serverName);  // initialise la connexion avec l’API

    http.addHeader("Content-Type", "application/json");  
    // indique qu'on envoie des données en format JSON

    // création du message JSON avec température et humidité
    String jsonData = "{\"temperature\":" + String(t) + ",\"humidite\":" + String(h) + "}";

    // envoi des données avec méthode POST
    int httpResponseCode = http.POST(jsonData);

    Serial.print("Code HTTP reçu : ");
    Serial.println(httpResponseCode);  
    // affiche le code retour (200 = OK)

    http.end();  // ferme la connexion HTTP
  } else {
    Serial.println("WiFi non connecté"); // message si perte de connexion
  }

  delay(5000);  // attend 5 secondes avant prochain envoi
}