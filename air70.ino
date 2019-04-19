#include <dummy.h>

#include <WiFi.h>
#include "netatmoapi.h"
#include "settings.h"

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  10

NetatmoApi api(NETATMO_API_USERNAME, NETATMO_API_PASSWORD, NETATMO_API_CLIENT_ID, NETATMO_API_CLIENT_SECRET);

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

      // Print debug msg
      Serial.print("Connected, IP: ");
      Serial.println(WiFi.localIP());

      

      api.fetchToken();

  }

  // Enter deep sleep
  digitalWrite(DEBUG_LED, LOW);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);  
  esp_deep_sleep_start();

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


