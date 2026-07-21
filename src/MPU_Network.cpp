/* Copyright (C) 2026  Samuel Trassare (https://github.com/Tiogaplanet)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/**
 * @file MPU_Network.cpp
 * @brief Implements WiFi connection, ArduinoOTA (Over-The-Air updates),
 *        and mDNS support for the WeMos D1 mini (ESP8266).
 *
 * This module handles network initialization on top of the core UART
 * connection to the MiP robot. No direct MiP communication occurs here.
 */
#include "MPU_D1_mini.h"

bool MiP::begin(const char* ssid, const char* password, const char* hostname) {
  bool returnValue = begin();

  // Memory-safe string copy operations to address bug:
  // https://github.com/Tiogaplanet/MiP_ESP8266_Library/issues/26
  strncpy(m_ssid, ssid, sizeof(m_ssid) - 1);
  m_ssid[sizeof(m_ssid) - 1] = '\0';

  strncpy(m_password, password, sizeof(m_password) - 1);
  m_password[sizeof(m_password) - 1] = '\0';

  strncpy(m_hostname, hostname, sizeof(m_hostname) - 1);
  m_hostname[sizeof(m_hostname) - 1] = '\0';

  WiFi.hostname(m_hostname);
  WiFi.begin(m_ssid, m_password);

  // Non-blocking status loop to address bug:
  // https://github.com/Tiogaplanet/MiP_ESP8266_Library/issues/25
  uint8_t attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    MIP_DEBUG_WARN_PRINTLN(F("MiP: Internet connection failed. Retrying..."));
    delay(500);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    MIP_DEBUG_INFO_PRINTLN(F("MiP: WiFi connected successfully"));
  } else {
    MIP_DEBUG_WARN_PRINTLN(
        F("MiP: WiFi connection failed after maximum attempts"));
    // Still return the UART result, but user can check WiFi.status()
  }

  // ArduinoOTA setup
  ArduinoOTA.onStart([]() {
    String type =
        (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    // NOTE: If updating LittleFS this would be the place to unmount LittleFS
    // using LittleFS.end().
    MIP_DEBUG_INFO_PRINT(F("MiP: Start updating "));
    MIP_DEBUG_INFO_PRINTLN(type);
  });

  // Correct formatted percentage string to address bug:
  // https://github.com/Tiogaplanet/MiP_ESP8266_Library/issues/27
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    (void)progress;  // Silences the unused parameter warning.
    if (total == 0)
      return;
    MIP_DEBUG_INFO_PRINTF("Progress: %u%%\r", (progress * 100) / total);
  });

  ArduinoOTA.onEnd([]() { MIP_DEBUG_INFO_PRINTLN(F("End")); });

  ArduinoOTA.onError([](ota_error_t error) {
    MIP_DEBUG_ERROR_PRINTF("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      MIP_DEBUG_ERROR_PRINTLN(F("Auth Failed"));
    } else if (error == OTA_BEGIN_ERROR) {
      MIP_DEBUG_ERROR_PRINTLN(F("Begin Failed"));
    } else if (error == OTA_CONNECT_ERROR) {
      MIP_DEBUG_ERROR_PRINTLN(F("Connect Failed"));
    } else if (error == OTA_RECEIVE_ERROR) {
      MIP_DEBUG_ERROR_PRINTLN(F("Receive Failed"));
    } else if (error == OTA_END_ERROR) {
      MIP_DEBUG_ERROR_PRINTLN(F("End Failed"));
    }
  });

  ArduinoOTA.begin();

  MIP_DEBUG_INFO_PRINTLN(F("MiP: IP address: ") + WiFi.localIP().toString());

  // Set up mDNS responder using the user-specified hostname and ending with
  // ".local". For example, if the user provides the hostname "HappyMiP" the
  // fully-qualified domain name is "HappyMiP.local".
  if (!MDNS.begin(m_hostname)) {
    MIP_DEBUG_ERROR_PRINTLN(F("MiP: Error setting up mDNS responder."));
  } else {
    MIP_DEBUG_INFO_PRINTF(
        "MiP: mDNS responder started with hostname of %s.local\r\n",
        m_hostname);
  }

  return returnValue;
}
