#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define LARGEUR 128 
#define HAUTEUR 64 
Adafruit_SSD1306 ecran(LARGEUR, HAUTEUR, &Wire, -1);

// --- CONFIGURATION WI-FI & API ---
const char* ssid = "PDA_ESP32_S";         
const char* password = "Ciel123!"; 

// !!! METTRE L'IP DE MON RASPBERRY PI !!!
const char* serverName = "http://172.19.6.120:5000/api/presence"; 

const int brochePIR = 25; 
int frame = 0; 
int dernierEtat = -1; 

// --- VARIABLES POUR AUGMENTER LA PORTEE VIRTUELLEMENT ---
unsigned long dernierMouvementTemps = 0;
const long delaiTolerance = 4000; 

// --- Animation Salle de Sport CORRIGÉE ET FLUIDE ---
void dessinerMuscu(int x, int y, bool actif) {
  ecran.drawLine(x - 30, y + 25, x + 30, y + 25, SSD1306_WHITE);

  if (actif) {
    frame = (frame + 1) % 6; 
    int squatH = 0;
    if (frame == 1 || frame == 5) squatH = 3;
    else if (frame == 2 || frame == 4) squatH = 6;
    else if (frame == 3) squatH = 9; 

    int teteY = y + squatH;
    int epaulesY = teteY + 4;
    int hanchesY = y + 14 + (squatH / 2); 

    ecran.drawCircle(x, teteY, 4, SSD1306_WHITE);
    ecran.drawLine(x, epaulesY, x, hanchesY, SSD1306_WHITE);

    int barreY = epaulesY + 1; 
    ecran.drawLine(x - 14, barreY, x + 14, barreY, SSD1306_WHITE);
    ecran.drawCircle(x - 14, barreY, 3, SSD1306_WHITE); 
    ecran.drawCircle(x + 14, barreY, 3, SSD1306_WHITE); 

    ecran.drawLine(x, epaulesY, x - 8, barreY, SSD1306_WHITE);
    ecran.drawLine(x, epaulesY, x + 8, barreY, SSD1306_WHITE);

    if (squatH > 4) { 
      ecran.drawLine(x, hanchesY, x - 10, hanchesY + 3, SSD1306_WHITE); 
      ecran.drawLine(x - 10, hanchesY + 3, x - 6, y + 25, SSD1306_WHITE); 
      ecran.drawLine(x, hanchesY, x + 10, hanchesY + 3, SSD1306_WHITE); 
      ecran.drawLine(x + 10, hanchesY + 3, x + 6, y + 25, SSD1306_WHITE);
    } else { 
      ecran.drawLine(x, hanchesY, x - 6, y + 19, SSD1306_WHITE); 
      ecran.drawLine(x - 6, y + 19, x - 4, y + 25, SSD1306_WHITE);
      ecran.drawLine(x, hanchesY, x + 6, y + 19, SSD1306_WHITE); 
      ecran.drawLine(x + 6, y + 19, x + 4, y + 25, SSD1306_WHITE);
    }
  } 
  else {
    frame = (frame + 1) % 8;
    int respire = (frame < 4) ? 1 : 0; 
    int hanchesY = y + 15 - respire;

    ecran.drawCircle(x, y, 4, SSD1306_WHITE);               
    ecran.drawLine(x, y + 4, x, hanchesY, SSD1306_WHITE);   
    ecran.drawLine(x, hanchesY, x - 5, y + 25, SSD1306_WHITE); 
    ecran.drawLine(x, hanchesY, x + 5, y + 25, SSD1306_WHITE); 
    ecran.drawLine(x, y + 8, x - 6, y + 14 - respire, SSD1306_WHITE); 
    ecran.drawLine(x, y + 8, x + 6, y + 14 - respire, SSD1306_WHITE); 
  }
}

void envoyerDonneesAPI(int etatPresence) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(2000); 

    String jsonPayload = "{\"presence\":" + String(etatPresence) + ", \"id_zone\":\"1\"}";
    
    Serial.print("Envoi au serveur : ");
    Serial.println(jsonPayload);

    int httpResponseCode = http.POST(jsonPayload);
    
    if (httpResponseCode > 0) {
      Serial.print("Reponse du serveur (Code) : ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erreur d'envoi HTTP : ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  } else {
    Serial.println("Erreur : Non connecte au Wi-Fi");
  }
}

void setup() {
  Serial.begin(115200);
  
  Wire.begin(21, 22);
  ecran.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  ecran.clearDisplay();
  ecran.display(); 
  
  pinMode(brochePIR, INPUT);
  
  Serial.print("Connexion au Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connecte !");
}

void loop() {
  int lectureCapteur = digitalRead(brochePIR);
  unsigned long tempsActuel = millis();

  if (lectureCapteur == HIGH) {
    dernierMouvementTemps = tempsActuel;
  }

  int etat = (lectureCapteur == HIGH || (tempsActuel - dernierMouvementTemps < delaiTolerance)) ? HIGH : LOW;

  ecran.clearDisplay();
  ecran.setTextSize(1);
  ecran.setTextColor(SSD1306_WHITE);

  if (etat == HIGH) {
    ecran.setCursor(12, 0);
    ecran.print("DETECTION PRESENTE");
    ecran.setCursor(25, 9);
    ecran.print("DANS LA SALLE");
    dessinerMuscu(64, 34, true); 
  } else {
    ecran.setCursor(15, 0);
    ecran.print("AUCUNE DETECTION");
    ecran.setCursor(25, 9);
    ecran.print("DANS LA SALLE");
    dessinerMuscu(64, 34, false);
  }

  ecran.display();

  if (etat != dernierEtat) {
    envoyerDonneesAPI(etat);
    dernierEtat = etat;
  }

  delay(150); 
}