#include <dummy.h>

#include <WiFi.h>
#include "netatmoapi.h"
#include "settings.h"

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  10

NetatmoApi api(NETATMO_API_USERNAME, NETATMO_API_PASSWORD, NETATMO_API_CLIENT_ID, NETATMO_API_CLIENT_SECRET);

void setup() {

#if DEBUG_ENABLED

  // Enable debug led
  pinMode(DEBUG_LED, OUTPUT);
  digitalWrite(DEBUG_LED, HIGH);

  // Initialize debug serial
  Serial.begin(115200);
  delay(1000);

#endif

  // Connect to wifi
  bool wifiConnected = wifiConnect(3000);
  
  if (wifiConnected) {

    // Fetch CO2
    int co2 = api.readCO2();

  }

#if DEBUG_ENABLED
  // Disable debug led
  digitalWrite(DEBUG_LED, LOW);
#endif

  // Enter deep sleep
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();

}

bool wifiConnect(unsigned int timeoutMs) {

  // Connect
  DEBUG_PRINT("Connecting to WiFi, SSID: ");
  DEBUG_PRINTLN(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  // Check connection until timeout  
  unsigned int remainingTime = timeoutMs;
  unsigned int delayTime = 100;
  while (WiFi.status() != WL_CONNECTED && remainingTime >= delayTime) {   
    remainingTime -= delayTime;
    DEBUG_PRINT(".");
    delay(delayTime);    
  }
  DEBUG_PRINTLN();

  if (WiFi.status() == WL_CONNECTED) {
    DEBUG_PRINT("Connected to WiFi, got IP: ");
    DEBUG_PRINTLN(WiFi.localIP());
    return true;
  } else {
    DEBUG_PRINTLN("Failed to connect to WiFi");
    return false;
  }

}

void loop() {}


