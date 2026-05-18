#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// --- Configuration de l'écran OLED ---
#define LARGEUR 128 
#define HAUTEUR 64 
Adafruit_SSD1306 ecran(LARGEUR, HAUTEUR, &Wire, -1);

// --- Paramètres de connexion Wi-Fi et Serveur ---
const char* ssid = "PDA_ESP32_S";
const char* password = "Ciel123!";
const char* serverIP = "172.19.6.120";
const int serverPort = 5000;

// --- Définition des broches des composants ---
const int PHOTO_RESISTOR_PIN = 32; // Capteur de luminosité (LDR)
const int LED_PIN = 2;              // LED de secours
const int brochePIR = 25;           // Capteur de présence PIR
#define DHTPIN 33                   // Capteur de température et humidité
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);
int frame = 0; 
unsigned long dernierEnvoi = 0;    // Stocke le temps du dernier envoi à l'API

// Déclaration de la fonction pour l'animation du stickman
void dessinerMuscu(int x, int y, bool actif);

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(brochePIR, INPUT);
  
  dht.begin(); // Démarrage du capteur DHT
  
  // Initialisation de l'écran OLED
  Wire.begin(21, 22);
  ecran.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  ecran.clearDisplay();
  
  // Connexion au réseau Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
}

void loop() {
  // 1. Lecture des capteurs
  int etatPresence = digitalRead(brochePIR);
  int sensorValue = analogRead(PHOTO_RESISTOR_PIN);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // 2. Calcul pour convertir la valeur de la LDR en Lux
  float resistance = (float)(4095 - sensorValue) * 10 / sensorValue;
  float lux = 10000 / pow((resistance * 10), 0.75); 

  // 3. Gestion automatique de la LED selon la lumière
  if (sensorValue < 2000) digitalWrite(LED_PIN, HIGH); // Allumer s'il fait sombre
  else digitalWrite(LED_PIN, LOW);                     // Éteindre s'il fait jour

  // 4. Gestion de l'affichage sur l'écran OLED
  ecran.clearDisplay();
  ecran.setTextSize(1);
  ecran.setTextColor(SSD1306_WHITE);

  if (etatPresence == HIGH) {
    // Affichage si une présence est détectée
    ecran.setCursor(12, 0); ecran.print("DETECTION PRESENTE");
    ecran.setCursor(25, 9); ecran.print("DANS LA SALLE");
    dessinerMuscu(64, 34, true); // Animation Squat active
  } else {
    // Affichage si la salle est vide
    ecran.setCursor(15, 0); ecran.print("AUCUNE DETECTION");
    ecran.setCursor(25, 9); ecran.print("DANS LA SALLE");
    dessinerMuscu(64, 34, false); // Personnage statique au repos
  }
  ecran.display();

  // 5. Envoi des données à l'API toutes les 60 secondes (60000 ms)
  if (millis() - dernierEnvoi >= 60000) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String urlBase = "http://" + String(serverIP) + ":" + String(serverPort);
      
      // Envoi de la luminosité au format JSON
      http.begin(urlBase + "/api/luminosite");
      http.addHeader("Content-Type", "application/json");
      http.POST("{\"luminosite\":" + String(lux) + ", \"zone\":\"1\"}");
      http.end();

      // Envoi de la température et de l'humidité au format JSON
      http.begin(urlBase + "/api/temperature_humidite");
      http.addHeader("Content-Type", "application/json");
      http.POST("{\"temperature\":" + String(t) + ", \"humidite\":" + String(h) + ", \"zone\":\"1\"}");
      http.end();
      
      dernierEnvoi = millis(); // Relancer le chronomètre d'envoi
    }
  }
  delay(150); 
}

// --- FONCTION : Dessin du Stickman Musculation ---
void dessinerMuscu(int x, int y, bool actif) {
  // Ligne de sol (Zone Bleue)
  ecran.drawLine(x-30, y+25, x+30, y+25, SSD1306_WHITE);

  if (actif) {
    // --- ANIMATION SQUAT (Détection présente) ---
    frame = (frame + 1) % 4; 
    int squatH = (frame == 1 || frame == 2) ? 8 : 0; // Hauteur de la flexion
    
    ecran.drawCircle(x, y + squatH, 4, SSD1306_WHITE);      // Tête
    ecran.drawLine(x, y+4+squatH, x, y+15, SSD1306_WHITE);  // Tronc
    
    // Barre de musculation sur les épaules
    ecran.drawLine(x-12, y+5+squatH, x+12, y+5+squatH, SSD1306_WHITE);
    ecran.fillCircle(x-12, y+5+squatH, 2, SSD1306_WHITE);   // Disque G
    ecran.fillCircle(x+12, y+5+squatH, 2, SSD1306_WHITE);   // Disque D

    // RECORRECTION : Dessin des jambes qui fléchissent
    if (squatH > 0) { 
      // Jambes pliées (Squat bas)
      ecran.drawLine(x, y+15, x-8, y+18, SSD1306_WHITE); 
      ecran.drawLine(x-8, y+18, x-5, y+25, SSD1306_WHITE); 
      ecran.drawLine(x, y+15, x+8, y+18, SSD1306_WHITE); 
      ecran.drawLine(x+8, y+18, x+5, y+25, SSD1306_WHITE);
    } else { 
      // Jambes tendues (Squat haut)
      ecran.drawLine(x, y+15, x-4, y+25, SSD1306_WHITE); 
      ecran.drawLine(x, y+15, x+4, y+25, SSD1306_WHITE);
    }
  } 
  else {
    // --- MODE STATIQUE (Aucune détection) ---
    ecran.drawCircle(x, y, 4, SSD1306_WHITE);           // Tête
    ecran.drawLine(x, y+4, x, y+15, SSD1306_WHITE);      // Corps
    
    // RECORRECTION : Ajout des membres manquants au repos
    ecran.drawLine(x, y+15, x-4, y+25, SSD1306_WHITE);   // Jambe G
    ecran.drawLine(x, y+15, x+4, y+25, SSD1306_WHITE);   // Jambe D
    ecran.drawLine(x, y+8, x-5, y+15, SSD1306_WHITE);    // Bras G
    ecran.drawLine(x, y+8, x+5, y+15, SSD1306_WHITE);    // Bras D
  }
}
