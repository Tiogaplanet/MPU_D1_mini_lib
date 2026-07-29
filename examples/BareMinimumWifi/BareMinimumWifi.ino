/**
 * @file BareMinimumWifi.ino
 * @brief Demonstrates the absolute minimum code required to connect MiP to a WiFi network.
 *
 * @details This sketch serves as a clean starting template for custom projects using the
 * Wemos D1 mini and MiP. It initializes the MiP library, establishes a WiFi connection
 * using the provided credentials, prints the assigned IP address, and sets up the loop
 * for Over-The-Air (OTA) updates and custom user code.
 *
 * While using the MPU:D1 mini library, the entirety of the esp8266 library is available
 * for use, meaning you could manage your own WiFi connectivity with calls to WiFi.begin(),
 * WiFi.connect(), etc. Therefore, this library only wraps the aforementioned two WiFi
 * functions to provide minor additional capbility: MiP manages the ssid, password, and
 * hostname variables and animates MiP's eyes while attempting to establish a WiFi
 * connection.
 *
 * Example used in API documentation:
 *   - wifi.begin(ssid, password, hostname)

 * @copyright Copyright (C) 2018 Samuel Trassare (https://github.com/Tiogaplanet)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <MPU_D1_mini.h>

/** @brief The SSID (name) of your local WiFi network. */
const char* ssid = "..............";

/** @brief The password for your local WiFi network. */
const char* password = "..............";

/** @brief The mDNS hostname assigned to MiP on the network. */
const char* hostname = "MiPFi";

/** @brief The global MiP library instance used to control the robot. */
MiP mip;

/** @brief Stores the result of the MiP and WiFi initialization attempt. */
bool connectResult;


/**
 * @brief Arduino setup routine.
 *
 * @details Initializes the MiP object and attempts to establish a WiFi connection
 * using the provided SSID, password, and hostname. If successful, it prints the
 * connected IP address to the debug serial monitor.
 */
void setup() {
  connectResult = mip.begin();

  if (!connectResult) {
    Serial1.println(F("BareMinimumWifi.ino: Failed connecting to MiP."));
    return;
  }

  Serial1.println(F("BareMinimumWifi.ino: Connect to a wireless access point."));

  if (mip.wifi.begin(ssid, password, hostname) == WL_CONNECTED) {
    // You could delete this chunk of code.
    // It's here only to show your IP address.
    Serial1.print(F(" IP address: "));
    Serial1.println(WiFi.localIP());
  }

  Serial1.println(F("BareMinimumWifi.ino: Done."));
}

/**
 * @brief Main Arduino loop routine.
 *
 * @details Handles Over-The-Air (OTA) firmware updates to keep the board accessible
 * over the network. This is the main entry point for adding custom control logic
 * and behaviors for the MiP robot.
 */
void loop() {
  if (!connectResult) return;  // If connecting to MiP failed in setup(), exit now.

  // Without this we can't do OTA programming.
  ArduinoOTA.handle();

  // Put your amazing code here.



  /////////////////////////////////////////////////////////////////////////////
}