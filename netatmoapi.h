#ifndef NETATMOAPI_H
#define NETATMOAPI_H

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "settings.h"

class NetatmoApi {

  public:
    NetatmoApi(const char* api_username, const char* api_password, const char* api_client_id, const char* api_client_secret) {
      this->api_username = api_username;
      this->api_password = api_password;
      this->api_client_id = api_client_id;
      this->api_client_secret = api_client_secret;
      client.setCACert(ca_cert);
    }

   int readCO2() {

    if (!fetchToken()) {
      return -1;
    }

    return 0;
    
   }

  private:

   bool fetchToken() {

      // Determine request body length
      int bodyLength = 84 // Fixed part of body data
                       + strlen(api_username)
                       + strlen(api_password)
                       + strlen(api_client_id)
                       + strlen(api_client_secret);

      // Create request body
      char body[bodyLength];
      strcpy(body, "grant_type=password");
      strcat(body, "&username=");
      strcat(body, api_username);
      strcat(body, "&password=");
      strcat(body, api_password);
      strcat(body, "&client_id=");
      strcat(body, api_client_id);
      strcat(body, "&client_secret=");
      strcat(body, api_client_secret);
      strcat(body, "&scope=read_station");

      // Do post request
      if (!doPostRequest("/oauth2/token", body)) {
        
        return false;
      }

      // Read until start of JSON
      while (client.available() && client.peek() != '{') {
        client.read();
      }

      // Parse JSON object
      DynamicJsonDocument doc(512);
      DeserializationError error = deserializeJson(doc, client);
      if (error) {
        DEBUG_PRINT("deserializeJson() failed: ");
        DEBUG_PRINTLN(error.c_str());
        return false;
      }

      // Read token
      this->token = doc["access_token"].as<char*>();
      DEBUG_PRINT("Retrieved access token: ");
      DEBUG_PRINTLN(token);

      client.stop();

      return true;

    }

    bool doPostRequest(const char* resource, const char* body) {

      DEBUG_PRINTLN("-- Performing post request -- ");

      // Connect to server
      if (!client.connect(server, 443)) {
        DEBUG_PRINT("Failed to connect to server '");
        DEBUG_PRINT(server);
        DEBUG_PRINTLN("'");
        return false;
      }

      DEBUG_PRINTLN("-- Post request -- ");
      DEBUG_PRINT("POST ");
      DEBUG_PRINT(resource);
      DEBUG_PRINTLN(" HTTP/1.1");
      DEBUG_PRINT("Host: ");
      DEBUG_PRINTLN(server);
      DEBUG_PRINTLN("Connection: close");
      DEBUG_PRINT("Content-Length: ");
      DEBUG_PRINTLN(strlen(body));
      DEBUG_PRINTLN("Content-Type: application/x-www-form-urlencoded");
      DEBUG_PRINTLN();
      DEBUG_PRINTLN(body);

      // Send HTTP request
      client.print("POST ");
      client.print(resource);
      client.println(" HTTP/1.1");
      client.print("Host: ");
      client.println(server);
      client.println("Connection: close");
      client.print("Content-Length: ");
      client.println(strlen(body));
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println();
      client.print(body);

      // Wait for HTTP response
      // @TODO: handle timeout
      while (!client.available()) {
        delay(50);
      }

      // Check for 200 HTTP status code
      char status[32] = {0};
      client.readBytesUntil('\r', status, sizeof(status));
      if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
        DEBUG_PRINT("Unsuccessful response: ");
        DEBUG_PRINTLN(status);
        return false;
      }

      return true;

    }

    WiFiClientSecure client;

    const char* api_username;
    const char* api_password;
    const char* api_client_id;
    const char* api_client_secret;

    const char* server = "api.netatmo.com";

    const char* token;

    /* use $ openssl s_client -showcerts -connect api.netatmo.com:443
      to get this certificate if it expires */
    const char* ca_cert =
      "-----BEGIN CERTIFICATE-----\n"
      "MIIEADCCAuigAwIBAgIBADANBgkqhkiG9w0BAQUFADBjMQswCQYDVQQGEwJVUzEh\n"
      "MB8GA1UEChMYVGhlIEdvIERhZGR5IEdyb3VwLCBJbmMuMTEwLwYDVQQLEyhHbyBE\n"
      "YWRkeSBDbGFzcyAyIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MB4XDTA0MDYyOTE3\n"
      "MDYyMFoXDTM0MDYyOTE3MDYyMFowYzELMAkGA1UEBhMCVVMxITAfBgNVBAoTGFRo\n"
      "ZSBHbyBEYWRkeSBHcm91cCwgSW5jLjExMC8GA1UECxMoR28gRGFkZHkgQ2xhc3Mg\n"
      "MiBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0eTCCASAwDQYJKoZIhvcNAQEBBQADggEN\n"
      "ADCCAQgCggEBAN6d1+pXGEmhW+vXX0iG6r7d/+TvZxz0ZWizV3GgXne77ZtJ6XCA\n"
      "PVYYYwhv2vLM0D9/AlQiVBDYsoHUwHU9S3/Hd8M+eKsaA7Ugay9qK7HFiH7Eux6w\n"
      "wdhFJ2+qN1j3hybX2C32qRe3H3I2TqYXP2WYktsqbl2i/ojgC95/5Y0V4evLOtXi\n"
      "EqITLdiOr18SPaAIBQi2XKVlOARFmR6jYGB0xUGlcmIbYsUfb18aQr4CUWWoriMY\n"
      "avx4A6lNf4DD+qta/KFApMoZFv6yyO9ecw3ud72a9nmYvLEHZ6IVDd2gWMZEewo+\n"
      "YihfukEHU1jPEX44dMX4/7VpkI+EdOqXG68CAQOjgcAwgb0wHQYDVR0OBBYEFNLE\n"
      "sNKR1EwRcbNhyz2h/t2oatTjMIGNBgNVHSMEgYUwgYKAFNLEsNKR1EwRcbNhyz2h\n"
      "/t2oatTjoWekZTBjMQswCQYDVQQGEwJVUzEhMB8GA1UEChMYVGhlIEdvIERhZGR5\n"
      "IEdyb3VwLCBJbmMuMTEwLwYDVQQLEyhHbyBEYWRkeSBDbGFzcyAyIENlcnRpZmlj\n"
      "YXRpb24gQXV0aG9yaXR5ggEAMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQEFBQAD\n"
      "ggEBADJL87LKPpH8EsahB4yOd6AzBhRckB4Y9wimPQoZ+YeAEW5p5JYXMP80kWNy\n"
      "OO7MHAGjHZQopDH2esRU1/blMVgDoszOYtuURXO1v0XJJLXVggKtI3lpjbi2Tc7P\n"
      "TMozI+gciKqdi0FuFskg5YmezTvacPd+mSYgFFQlq25zheabIZ0KbIIOqPjCDPoQ\n"
      "HmyW74cNxA9hi63ugyuV+I6ShHI56yDqg+2DzZduCLzrTia2cyvk0/ZM/iZx4mER\n"
      "dEr/VxqHD3VILs9RaRegAhJhldXRQLIQTO7ErBBDpqWeCtWVYpoNz4iCxTIM5Cuf\n"
      "ReYNnyicsbkqWletNw+vHX/bvZ8=\n"
      "-----END CERTIFICATE-----\n";

};

#endif

