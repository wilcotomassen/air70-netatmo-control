#include <dummy.h>

#include <WiFi.h>
#include "netatmoapi.h"
#include "settings.h"

NetatmoApi api(NETATMO_API_USERNAME, NETATMO_API_PASSWORD, NETATMO_API_CLIENT_ID, NETATMO_API_CLIENT_SECRET, NETATMO_API_DEVICE_ID);

void setup() {

  // Setup pins
  pinMode(NOTIFICATION_LED, OUTPUT);

#if DEBUG_ENABLED

  // Enable debug led
  digitalWrite(NOTIFICATION_LED, HIGH);

  // Initialize debug serial
  Serial.begin(115200);
  delay(1000);

#endif

  // Connect to wifi
  bool wifiConnected = wifiConnect(3000);
  bool successfulRead = false;
  
  if (wifiConnected) {

    // Fetch CO2
    int co2 = api.readCO2();

    if (co2 != -1) {
      successfulRead = true;
    }
    

  }

#if DEBUG_ENABLED
  // Disable debug led
  digitalWrite(NOTIFICATION_LED, LOW);
  delay(50);
#endif

  if (successfulRead) {
    // Blink once for sucessfull read
    digitalWrite(NOTIFICATION_LED, HIGH);
    delay(100);
    digitalWrite(NOTIFICATION_LED, HIGH);
  } else {
    // Blink three times shortly for unsucessfull read
    for (int i = 0; i < 3; i++) {
      digitalWrite(NOTIFICATION_LED, HIGH);
      delay(50);
      digitalWrite(NOTIFICATION_LED, HIGH);
    }
  }
  
  // Enter deep sleep
  esp_sleep_enable_timer_wakeup(POLL_INTERVAL);
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


