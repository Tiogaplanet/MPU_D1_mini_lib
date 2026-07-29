/**
 * @file Radar.ino
 * @brief Example sketch demonstrating MiP radar distance sensing and reporting.
 *
 * @details This sketch shows how to use the MiP library to enable radar mode,
 * read radar distance categories, and report changes to the user over Serial1.
 * The sketch waits for the robot to be standing upright before enabling radar
 * mode and then continuously polls readRadar() in loop(), printing a human
 * readable description whenever the radar reading changes.
 *
 * The example exercises these API calls:
 *   - radar.enable()
 *   - isUpright()
 *   - radar.read()
 *
 * @copyright Copyright (C) 2018 Adam Green (https://github.com/adamgreen)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <MPU_D1_mini.h>

/**
 * @brief Global MiP instance used to communicate with the robot.
 *
 * @details Use this object to call MiP API functions such as begin(),
 * radar.enable(), isUpright(), and radar.read().
 */
MiP mip;

/**
 * @brief Tracks whether the initial connection to the MiP succeeded.
 *
 * @details Stored so other parts of the sketch could check connection state
 * if extended.
 */
bool connectResult;

/**
 * @brief Arduino setup function.
 *
 * @details Initializes communication with the MiP robot by calling mip.begin().
 * If the connection fails, an error message is printed to Serial1 and setup
 * returns early. The function waits until the robot reports it is upright,
 * then enables radar mode so the robot will begin reporting radar distance
 * categories via readRadar().
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("Radar.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("Radar.ino: Display current radar readings to user."));

  Serial1.println(F(" Waiting for robot to be standing upright."));
  while (!mip.isUpright()) {
    // Busy-wait until MiP reports upright; required before enabling radar.
  }

  // Enable radar mode so readRadar() returns distance categories.
  mip.radar.enable();
}

/**
 * @brief Arduino loop function.
 *
 * @details Continuously polls the MiP radar using readRadar(). When a valid
 * radar reading is returned and it differs from the previous reading, the
 * sketch prints a human-readable description of the detected distance range
 * to Serial1. The switch statement maps MiPRadar enum values to strings:
 *   - MIP_RADAR_NONE: no obstruction detected
 *   - MIP_RADAR_10CM_30CM: distant obstruction (10cm–30cm)
 *   - MIP_RADAR_0CM_10CM: near obstruction (0cm–10cm)
 *
 * The code ignores invalid readings and only reports on changes to avoid
 * spamming the serial output.
 */
void loop() {
  if (!connectResult) return;  // If connecting to MiP failed in setup(), exit now.

  static MiPRadar lastRadar = MIP_RADAR_INVALID;  // Remember last reported radar state.
  MiPRadar currentRadar = mip.radar.read();        // Read current radar category.

  // Only act when a valid reading is available and it changed since last time.
  if (currentRadar != MIP_RADAR_INVALID && lastRadar != currentRadar) {
    Serial1.print(F(" Radar = "));
    switch (currentRadar) {
      case MIP_RADAR_NONE:
        Serial1.println(F("None"));
        break;
      case MIP_RADAR_10CM_30CM:
        Serial1.println(F("10cm - 30cm"));
        break;
      case MIP_RADAR_0CM_10CM:
        Serial1.println(F("0cm - 10cm"));
        break;
      default:
        // Defensive: handle any future or unexpected enum values gracefully.
        Serial1.println(F("Unknown"));
        break;
    }
    lastRadar = currentRadar;
  }
}
