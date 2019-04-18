#include <dummy.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "settings.h"

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  10

void setup() {

  digitalWrite(DEBUG_LED, HIGH);

  // Initialize debug serial
  Serial.begin(115200);
  delay(1000);

  // Resume wifi
  //esp_wifi_start();

  // Initialize
  pinMode(DEBUG_LED, OUTPUT);

  bool wifiConnected = initWiFi(20);
  if (wifiConnected) {
      Serial.print("Connected, IP: ");
      Serial.println(WiFi.localIP());

      WiFiClientSecure client;
      client.setCACert(ca_cert);

      Serial.println("Connect to server via port 443");
      
    if (!client.connect(server, 443)){
        Serial.println("Connection failed!");
    } else {
     
      
        Serial.println("Connected to server!");
        /* create HTTP request */
        client.println("GET api/getstationsdata HTTP/1.0");
        client.println("Host: api.netatmo.com");
        client.println("Connection: close");
        client.println();

        Serial.print("Waiting for response ");
        while (!client.available()){
            delay(50); //
            Serial.print(".");
        }  
        /* if data is available then receive and print to Terminal */
        while (client.available()) {
            char c = client.read();
            Serial.write(c);
        }

        /* if the server disconnected, stop the client */
        if (!client.connected()) {
            Serial.println();
            Serial.println("Server disconnected");
            client.stop();
        }
  }

  }

  // Enter deep sleep
  digitalWrite(DEBUG_LED, LOW);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);  
  esp_deep_sleep_start();

}

int getCO2() {
  return 0;
}

bool initWiFi(unsigned int tryCount) {
  
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  // Try to connect
  unsigned int remainingAttempts = tryCount;
  while (WiFi.status() != WL_CONNECTED && remainingAttempts > 0) {
    delay(500);
    remainingAttempts--;
  } 
  
  return (WiFi.status() == WL_CONNECTED);
    
}

void loop() {}


