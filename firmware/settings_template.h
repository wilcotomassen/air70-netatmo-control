#ifndef SETTINGS_H
#define SETTINGS_H

// Update this file and rename it to settings.h

// WiFi settings
#define WIFI_SSID "myWiFiSSID"
#define WIFI_PSK "myWiFiPassword"

// Netatmo API settings
#define NETATMO_API_USERNAME "someone@example.com"
#define NETATMO_API_PASSWORD "mySecretPassword"
#define NETATMO_API_CLIENT_ID "netatmoGeneratedClientID"
#define NETATMO_API_CLIENT_SECRET "netatmoGeneratedClientSecret"
#define NETATMO_API_DEVICE_ID "ff:00:00:bb:aa::dd"

// Pin configuration
#define NOTIFICATION_LED 2 // Integrated led is on GPIO2 on Devkit v1

// Misc settings
#define POLL_INTERVAL 10 * 1000000   // Poll interval in uS
#define DEBUG_ENABLED false

// Debug printing macro
#if DEBUG_ENABLED
 #define DEBUG_PRINT(x)  Serial.print(x)
#else
 #define DEBUG_PRINT(x)
#endif

#if DEBUG_ENABLED
 #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
 #define DEBUG_PRINTLN(x)
#endif

#endif
