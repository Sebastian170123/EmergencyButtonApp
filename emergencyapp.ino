#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Sebas"
#define WIFI_PASSWORD "09162523Ya..."


#define API_KEY "AIzaSyAd9e3prXQdqVcKmzUquFIot6egmHH1PRQ"
#define DATABASE_URL "https://emergencybuttonapp-435f8-default-rtdb.firebaseio.com/" 


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


#define LED_PIN 18
bool currentLEDState = false; 
bool signupOK = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); 

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWi-Fi conectado.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Autenticación exitosa.");
    signupOK = true;
  } else {
    Serial.printf("Error: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (!Firebase.RTDB.getString(&fbdo, "ledState")) {
    Firebase.RTDB.setString(&fbdo, "ledState", "0"); 
    Serial.println("Nodo 'ledState' creado con valor 0.");
  } else {
    Serial.println("Nodo 'ledState' ya existe.");
  }
}

void loop() {
  if (Firebase.ready() && signupOK) {
    
    if (Firebase.RTDB.getString(&fbdo, "ledState")) {
      String newStateString = fbdo.stringData(); 

      Serial.print("Estado leído de Firebase: ");
      Serial.println(newStateString); 

      
      int newState = newStateString.toInt();

      if (newState != currentLEDState) {
        currentLEDState = newState; 
        digitalWrite(LED_PIN, currentLEDState); 
        Serial.print("LED cambiado a: ");
        Serial.println(currentLEDState ? "ON" : "OFF");
      }
    } else {
      Serial.println("Error al leer el estado del LED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }

  delay(500); 
}