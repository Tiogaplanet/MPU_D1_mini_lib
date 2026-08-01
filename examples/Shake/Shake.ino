/**
 * @file Shake.ino
 * @brief Example sketch demonstrating MiP shake detection.
 *
 * @details
 * This simple example initializes communication with a MiP and continuously
 * polls the device for shake events. When the MiP detects a shake, the sketch
 * prints a notification to Serial1. The sketch demonstrates basic use of the
 * MiP API for initialization and the shake-detection query:
 *   - begin()
 *   - shake.read()
 *
 * Typical usage:
 *   - Load this sketch onto the MPU: D1 mini connected to a MiP Power Up.
 *   - Open Serial1 to observe "Shake detected!" messages when the robot is shaken.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MPU_D1_mini.h>

/**
 * @brief Global MiP instance used to communicate with the robot.
 *
 * @details Use this object to call MiP API functions such as begin() and
 * shake.read(). Keeping the instance at file scope makes it available
 * throughout setup() and loop().
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
 * @details Initializes the MiP connection by calling mip.begin(). If the
 * connection fails, an error message is printed to Serial1 and the sketch
 * returns early. On success, a short description is printed to Serial1 to
 * indicate the sketch is ready to detect shakes.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("Shake.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("Shake.ino: Detect shakes."));
}

/**
 * @brief Arduino loop function.
 *
 * @details Continuously polls the MiP for shake events using hasBeenShaken().
 * When a shake is detected, the sketch prints "Shake detected!" to Serial1.
 * This loop is intentionally minimal to keep the example focused on the
 * shake-detection API.
 */
void loop() {
  if (!connectResult) return;  // If connecting to MiP failed in setup(), exit now.

  if (mip.shake.read()) {
    Serial1.println(F(" Shake detected!"));
  }
}

