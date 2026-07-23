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
 * connection to the MiP robot.
 */
#include "MPU_D1_mini.h"

bool MiP::wifiBegin(const char* ssid,
                    const char* password,
                    const char* hostname) {
  // Memory-safe string copy operations to address bug:
  // https://github.com/Tiogaplanet/MiP_ESP8266_Library/issues/26
  strncpy(m_ssid, ssid, sizeof(m_ssid) - 1);
  m_ssid[sizeof(m_ssid) - 1] = '\0';

  strncpy(m_password, password, sizeof(m_password) - 1);
  m_password[sizeof(m_password) - 1] = '\0';

  strncpy(m_hostname, hostname, sizeof(m_hostname) - 1);
  m_hostname[sizeof(m_hostname) - 1] = '\0';

  WiFi.hostname(m_hostname);

  if (wifiConnect() == WL_CONNECTED)
    return true;

  return false;
}

void MiP::enableAirplaneMode() {
  WiFi.disconnect();    // Disconnect from current network.
  WiFi.mode(WIFI_OFF);  // Turn off WiFi radio.

  // App mode broadcasts BLE.  If MiP is currently in app mode, switch to
  // the default, power-on gesture mode.
  if (isAppModeEnabled())
    enableGestureMode();
}

uint8_t MiP::disableAirplaneMode() {
  WiFi.mode(WIFI_STA);  // or WIFI_AP, WIFI_AP_STA
  return wifiConnect();
}

uint8_t MiP::wifiConnect() {
  // Safety check: ensure we have a valid SSID
  if (m_ssid[0] == '\0' || strlen(m_ssid) == 0) {
    MIP_DEBUG_ERROR_PRINTLN(
        F("MiP: No SSID configured. Call wifiBegin() first."));
    return WL_DISCONNECTED;
  }

  WiFi.begin(m_ssid, m_password);

  // Save original head LED state before animation
  MiPHeadLEDs originalLEDs;
  readHeadLEDs(originalLEDs);

  // Non-blocking status loop to address bug:
  // https://github.com/Tiogaplanet/MiP_ESP8266_Library/issues/25
  uint8_t attempts = 0;
  uint8_t ledPos = 0;
  bool direction = true;  // true = left-to-right, false = right-to-left

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    // Animate the four head LEDs in a back-and-forth scanning pattern
    MiPHeadLED leds[4] = {
        MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF};

    leds[ledPos] = MIP_HEAD_LED_ON;

    unverifiedWriteHeadLEDs(leds[0], leds[1], leds[2], leds[3]);

    // Update position for next frame
    if (direction) {
      ledPos++;
      if (ledPos >= 3)
        direction = false;
    } else {
      ledPos--;
      if (ledPos == 0)
        direction = true;
    }

    MIP_DEBUG_WARN_PRINTLN(F("MiP: WiFi connection attempt..."));
    delay(300);  // Nice visible animation speed
    attempts++;
  }

  unverifiedWriteHeadLEDs(originalLEDs.led1,
                          originalLEDs.led2,
                          originalLEDs.led3,
                          originalLEDs.led4);

  // Turn all LEDs on when connected, or pulse slow red on failure
  uint8_t connectStatus = WiFi.status();
  if (connectStatus == WL_CONNECTED) {
    MIP_DEBUG_INFO_PRINTLN(F("MiP: WiFi connected successfully"));
    // Set up mDNS responder using the user-specified hostname and ending with
    // ".local". For example, if the user provides the hostname "HappyMiP" the
    // fully-qualified domain name is "HappyMiP.local".
    if (!MDNS.begin(m_hostname)) {
      MIP_DEBUG_ERROR_PRINTLN(F("MiP: Error setting up mDNS responder."));
    } else {
      MIP_DEBUG_INFO_PRINTF(
          "MiP: mDNS responder started with hostname of %s.local\r\n",
          m_hostname);
      MIP_DEBUG_INFO_PRINTLN(F("MiP: IP address: ") +
                             WiFi.localIP().toString());
    }

    // ArduinoOTA setup (unchanged)
    ArduinoOTA.onStart([]() {
      String type =
          (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
      MIP_DEBUG_INFO_PRINT(F("MiP: Start updating "));
      MIP_DEBUG_INFO_PRINTLN(type);
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
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
    return WL_CONNECTED;
  } else {
    MIP_DEBUG_WARN_PRINTLN(
        F("MiP: WiFi connection failed after maximum attempts"));
    // Pulse slow red on chest LED to indicate failure
    writeChestLED(0xFF, 0x00, 0x00, 800, 800);  // Slow red blink
    return connectStatus;
  }
}
