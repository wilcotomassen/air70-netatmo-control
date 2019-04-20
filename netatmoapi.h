#ifndef NETATMOAPI_H
#define NETATMOAPI_H

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "settings.h"

class NetatmoApi {

  public:
    NetatmoApi(const char* apiUsername, const char* apiPassword, const char* apiClientId, const char* apiClientSecret, const char* apiDeviceId) {
      this->apiUsername = apiUsername;
      this->apiPassword = apiPassword;
      this->apiClientId = apiClientId;
      this->apiClientSecret = apiClientSecret;
      this->apiDeviceId = apiDeviceId;
      client.setCACert(ca_cert);
    }

    int readCO2() {

      // Fetch token
      if (!fetchToken()) {
        return -1;
      }

      // Do stations data call
      int resourceLength = 45
                           + strlen(token)
                           + strlen(apiDeviceId);

      char resourceString[resourceLength];
      strcpy(resourceString, "/api/getstationsdata");
      strcat(resourceString, "?access_token=");
      strcat(resourceString, token);
      strcat(resourceString, "&device_id=");
      strcat(resourceString, apiDeviceId);

      // Do post request
      if (!doGetRequest(resourceString)) {
        disconnectFromServer();
        return false;
      }

      // Read until start of JSON
      while (client.available() && client.peek() != '{') {
        client.read();
      }

      // Parse JSON object
      DynamicJsonDocument doc(4096);
      DeserializationError error = deserializeJson(doc, client);
      if (error) {
        DEBUG_PRINT("deserializeJson() failed: ");
        DEBUG_PRINTLN(error.c_str());
        disconnectFromServer();
        return false;
      }

      // Crude parsing to get the CO2 levels, will only reliably work for 
      // a station with one indoor sensor unit
      int co2 = doc["body"]["devices"][0]["dashboard_data"]["CO2"];
      char co2StrBuffer[20];
      itoa(co2, co2StrBuffer, 10);
      DEBUG_PRINT("Fetch CO2 value: ");
      DEBUG_PRINTLN(co2StrBuffer);
      
      disconnectFromServer();

      return co2;

    }

  private:

    bool fetchToken() {

      // Determine request body length
      int bodyLength = 84 // Fixed part of body data
                       + strlen(apiUsername)
                       + strlen(apiPassword)
                       + strlen(apiClientId)
                       + strlen(apiClientSecret);

      // Create request body
      char body[bodyLength];
      strcpy(body, "grant_type=password");
      strcat(body, "&username=");
      strcat(body, apiUsername);
      strcat(body, "&password=");
      strcat(body, apiPassword);
      strcat(body, "&client_id=");
      strcat(body, apiClientId);
      strcat(body, "&client_secret=");
      strcat(body, apiClientSecret);
      strcat(body, "&scope=read_station");

      // Do post request
      if (!doPostRequest("/oauth2/token", body)) {
        disconnectFromServer();
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
        disconnectFromServer();
        return false;
      }

      // Read token
      this->token = doc["access_token"].as<char*>();
      DEBUG_PRINTLN();
      DEBUG_PRINT("Retrieved access token: ");
      DEBUG_PRINTLN(token);

      disconnectFromServer();

      return true;

    }

    bool doPostRequest(const char* resource, const char* body) {

      DEBUG_PRINTLN("-- Performing POST request -- ");

      if (!connectToServer()) {
        return false;
      }

      // Send HTTP request
      DEBUG_PRINTLN("-- Post request -- ");
      printToClient("POST ");
      printToClient(resource);
      printlnToClient(" HTTP/1.1");
      printToClient("Host: ");
      printlnToClient(server);
      printlnToClient("Connection: close");
      if (strlen(body) > 0) {
        printToClient("Content-Length: ");
        char bodyLenBuffer[20];
        itoa(strlen(body), bodyLenBuffer, 10);
        printlnToClient(bodyLenBuffer);
        printlnToClient("Content-Type: application/x-www-form-urlencoded");
        printlnToClient("");
        printToClient(body);
      }

      return waitForHttpResponse();

    }

    bool doGetRequest(const char* resource) {

      DEBUG_PRINTLN("-- Performing GET request -- ");

      if (!connectToServer()) {
        return false;
      }

      // Send HTTP request
      DEBUG_PRINTLN("-- GET request -- ");
      printToClient("GET ");
      printToClient(resource);
      printlnToClient(" HTTP/1.1");
      printToClient("Host: ");
      printlnToClient(server);
      printlnToClient("");

      return waitForHttpResponse();

    }

    bool connectToServer() {

      // Connect to server
      if (client.connect(server, 443)) {
        DEBUG_PRINT("Connected to server: ");
        DEBUG_PRINTLN(server);
        return true;
      } else {
        DEBUG_PRINT("Failed to connect to server: ");
        DEBUG_PRINTLN(server);
        return false;
      }

    }

    bool disconnectFromServer() {
      client.flush();
      client.stop();
    }

    bool waitForHttpResponse(int timeoutMs = 10000) {

      // Wait for HTTP response
      unsigned int remainingTime = timeoutMs;
      unsigned int delayTime = 100;
      while (!client.available() && remainingTime >= delayTime) {
        remainingTime -= delayTime;
        DEBUG_PRINT(".");
        delay(delayTime);   
      }

      if (!client.available()) {
        DEBUG_PRINTLN("Request timed out");
        return false;
      } else {
        DEBUG_PRINTLN("");
      }

      // Check for 200 HTTP status code
      char status[32] = {0};
      client.readBytesUntil('\r', status, sizeof(status));
      if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
        DEBUG_PRINT("Unsuccessful HTTP response: ");
        DEBUG_PRINTLN(status);
        return false;
      }

      return true;

    }

    void printToClient(const char* content) {
      DEBUG_PRINT(content);
      client.print(content);
    }

    void printlnToClient(const char* content) {
      DEBUG_PRINTLN(content);
      client.println(content);
    }

    WiFiClientSecure client;

    const char* apiUsername;
    const char* apiPassword;
    const char* apiClientId;
    const char* apiClientSecret;
    const char* apiDeviceId;

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

