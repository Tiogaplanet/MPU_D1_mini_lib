/**
 * @file Odometer.ino
 * @brief Example sketch demonstrating MiP's odometer read and reset operations.
 *
 * @details This sketch shows how to use the MiP library to read the total
 * distance MiP has traveled since the last reset and how to reset the
 * odometer value. It prints the current distance in centimeters to mip.console,
 * calls odometer.reset() to clear the measurement, and reads back the distance
 * again to verify the reset succeeded.
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.odometer.read()
 *   - mip.odometer.reset()
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MiP_Power_Up.h>

/**
 * @brief Global MiP instance used to communicate with MiP.
 *
 * @details Use this object to call MiP API functions such as begin(),
 * odometer.read(), and odometer.reset().
 */
MiP mip;

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Arduino setup function.
 *
 * @details Called once after the board powers up or resets. This function:
 *   - Initializes communication with MiP via mip.begin().
 *   - If the connection fails, prints an error to mip.console and returns early.
 *   - Reads the current odometer value (in centimeters) using
 *     odometer.read() and prints it to mip.console.
 *   - Resets the odometer using odometer.reset().
 *   - Re-reads odometer.read() to verify the distance counter was cleared.
 *
 * The function prints progress and completion messages to mip.console so the
 * user can observe the initial odometer reading and the verified reset action.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    mip.console.println(F("Odometer.ino: Failed connecting to MiP!"));
    return;
  }

  mip.console.println(F("Odometer.ino: Read out current odometer reading and reset."));

  // Read initial distance travelled prior to reset
  float distanceCm = mip.odometer.read();
  mip.console.print(F(" MiP has travelled "));
  mip.console.print(distanceCm);
  mip.console.println(F(" cm since the last reset."));

  // Reset internal wheel encoder tick counter
  mip.console.println(F(" Resetting odometer..."));
  mip.odometer.reset();

  // Read back distance after reset to confirm zeroing
  distanceCm = mip.odometer.read();
  mip.console.print(F(" MiP has travelled "));
  mip.console.print(distanceCm);
  mip.console.println(F(" cm since the last reset."));

  mip.console.println(F("Odometer.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs all actions in setup() and does not require
 * repeated work in loop().
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) { return; }
}
