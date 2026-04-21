#include <WiFi.h>          // bibliothèque WiFi pour ESP32
#include <HTTPClient.h>    // bibliothèque pour envoyer requêtes HTTP
#include <DHT.h>           // bibliothèque du capteur DHT

#define DHTPIN 4           // broche du capteur
#define DHTTYPE DHT11      // type de capteur

DHT dht(DHTPIN, DHTTYPE);  // création objet capteur

const char* ssid = "TON_WIFI";        // nom du wifi
const char* password = "TON_MDP";     // mot de passe wifi

String serverName = "http://192.168.X.X/insert.php"; 
// adresse de ton serveur WAMP (IP de ton PC)

void setup() {

  Serial.begin(115200);   // démarre communication série
  dht.begin();            // démarre capteur

  WiFi.begin(ssid, password);  // connexion wifi

  Serial.print("Connexion WiFi");

  while (WiFi.status() != WL_CONNECTED) { // attend connexion wifi
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnecté au WiFi"); // wifi connecté
}

void loop() {

  float h = dht.readHumidity();      // lit humidité
  float t = dht.readTemperature();   // lit température

  if (isnan(h) || isnan(t)) {        // vérifie erreur capteur
    Serial.println("Erreur capteur");
    return;
  }

  if (WiFi.status() == WL_CONNECTED) { // vérifie wifi

    HTTPClient http; // crée objet HTTP

    String url = serverName + "?temperature=" + String(t) + "&humidite=" + String(h);
    // crée l'URL avec les valeurs du capteur

    http.begin(url); // ouvre connexion au serveur

    int httpResponseCode = http.GET(); // envoie requête

    Serial.print("Code HTTP : ");
    Serial.println(httpResponseCode); // affiche réponse serveur

    http.end(); // ferme connexion
  }

  delay(5000); // attend 5 secondes avant prochaine mesure
}