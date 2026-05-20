// ==============================
// FONCTION : Envoi API (Mise à jour)
// ==============================
void envoyerDonnees(float t, float h, int etatVentilo) {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // AJOUT : On ajoute "ventilateur" dans le JSON
    String json = "{\"temperature\":" + String(t, 1) +
                  ",\"humidite\":"    + String(h, 1) +
                  ",\"zone\":1" +
                  ",\"ventilateur\":" + String(etatVentilo) + "}";

    int code = http.POST(json);
    Serial.printf("[API] Réponse : %d\n", code);
    http.end();
  }
}

// ==============================
// LOOP (Mise à jour)
// ==============================
void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("[ERREUR] Lecture impossible !");
  } else {
    Serial.println("------------------------------------");
    Serial.printf("[CAPTEUR] Temp : %.1f °C | Hum : %.1f %%\n", t, h);

    // 2. CONTRÔLE VENTILATEUR + STRECKAGE DE L'ÉTAT
    int etatVentilo = 0;
    if (t > SEUIL_TEMP || h > SEUIL_HUMI) {
      digitalWrite(M2A, HIGH);
      digitalWrite(M2B, LOW);
      etatVentilo = 1; // Allumé
      Serial.println("[MOTEUR] Ventilateur : ALLUMÉ");
    } else {
      digitalWrite(M2A, LOW);
      digitalWrite(M2B, LOW);
      etatVentilo = 0; // Éteint
      Serial.println("[MOTEUR] Ventilateur : ÉTEINT");
    }

    // 3. ENVOI API (avec l'état du ventilateur)
    envoyerDonnees(t, h, etatVentilo);
  }

  Serial.println("Attente 60 secondes...\n");
  delay(60000);
}