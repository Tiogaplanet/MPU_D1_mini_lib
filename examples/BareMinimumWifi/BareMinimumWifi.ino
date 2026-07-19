/**
 * @file BareMinimumWifi.ino
 * @brief Demonstrates the absolute minimum code required to connect MiP to a WiFi network.
 *
 * @details This sketch serves as a clean starting template for custom projects using the 
 * Wemos D1 mini and MiP. It initializes the MiP library, establishes a WiFi connection
 * using the provided credentials, prints the assigned IP address, and sets up the loop
 * for Over-The-Air (OTA) updates and custom user code.
 *
 * Example used in API documentation:
 *   - begin(ssid, password, hostname)
 
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

/** @brief The mDNS hostname assigned to the ESP8266 on the network. */
const char* hostname = "MiP-0x01";

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
  connectResult = mip.begin(ssid, password, hostname);

  if (!connectResult) {
    Serial1.println(F("BareMinimumWifi.ino: Failed connecting to MiP."));
    return;
  }

  Serial1.println(F("BareMinimumWifi.ino: Connect to a wireless access point."));

  Serial1.print(F(" IP address: "));

  // You could delete this chunk of code.
  // It's here only to show your IP address.
  Serial1.println(WiFi.localIP());

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



  /////////////////////////////////////////////////////////////////////////////////////////
}