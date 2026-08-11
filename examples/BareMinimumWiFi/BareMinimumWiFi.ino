/**
 * @file BareMinimumWiFi.ino
 * @brief Demonstrates the absolute minimum code required to connect MiP to a
 * WiFi network and enable Over-The-Air (OTA) updates.
 *
 * @details This sketch serves as a clean starting template for custom projects
 * using the Wemos D1 mini and MiP. It initializes the MiP library, establishes
 * a WiFi connection using the provided credentials, prints the assigned IP
 * address, and sets up the loop for Over-The-Air (OTA) updates and custom user
 * code.
 *
 * While using the MiP Power Up - D1 mini library, the entirety of the esp8266 library
 * is available for use, meaning you could manage your own WiFi connectivity with
 * calls to WiFi.begin(), WiFi.connect(), etc. Therefore, this library wraps those
 * WiFi functions to provide minor additional capability: MiP manages the ssid,
 * password, and hostname variables and animates MiP's eyes while attempting to
 * establish a WiFi connection.
 *
 * Example API methods demonstrated:
 *   - mip.begin()
 *   - mip.wifi.begin(ssid, password, hostname)
 *   - mip.wifi.isConnected()
 *   - mip.wifi.localIP()
 *
 * @author Samuel Trassare (Original Author)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MiP_Power_Up_-_D1_mini.h>

/** @brief The SSID (network name) of your local WiFi network. */
const char *ssid = "..............";

/** @brief The password for your local WiFi network. */
const char *password = "..............";

/** @brief The mDNS hostname assigned to MiP on the network (resolves as MiPFi.local). */
const char *hostname = "MiPFi";

/** @brief The global MiP library instance used to control MiP. */
MiP mip;

/** @brief Stores the result of the MiP initialization attempt. */
bool connectResult;

/**
 * @brief Arduino setup routine.
 *
 * @details Initializes the MiP object and attempts to establish a WiFi
 * connection using the provided SSID, password, and hostname. If successful, it
 * prints the connected IP address to the debug serial monitor.
 */
void setup() {
  // Initialize communication with MiP
  connectResult = mip.begin();

  if (!connectResult) {
    Serial1.println(F("BareMinimumWiFi.ino: Failed connecting to MiP."));
    return;
  }

  Serial1.println(
      F("BareMinimumWiFi.ino: Connecting to wireless access point..."));

  // Connect to WiFi and initialize mDNS and ArduinoOTA network services
  uint8_t wifiStatus = mip.wifi.begin(ssid, password, hostname);

  if (wifiStatus != WL_CONNECTED) {
    Serial1.println(F("BareMinimumWiFi.ino: Failed connecting to WiFi."));
    return;
  }

  // Display assigned local IP address upon successful connection
  Serial1.print(F(" IP address: "));
  Serial1.println(mip.wifi.localIP());

  Serial1.println(F("BareMinimumWiFi.ino: Done. Connected and ready."));
}

/**
 * @brief Main Arduino loop routine.
 *
 * @details Handles Over-The-Air (OTA) firmware updates to keep the board
 * accessible over the network. This is the main entry point for adding custom
 * control logic and behaviors for MiP.
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) {
    return;
  }

  // Handle background Over-The-Air (OTA) programming requests
  ArduinoOTA.handle();

  // Put your amazing code here!

  /////////////////////////////////////////////////////////////////////////////
}
