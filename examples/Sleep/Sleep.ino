/**
 * @file Sleep.ino
 * @brief Demonstrates connecting, disconnecting, reconnecting, and sleeping a MiP.
 *
 * @details
 * This example shows how to manage a MiP connection from an MPU: D1 mini using
 * the MiP library. It demonstrates:
 *   - Establishing a UART connection with mip.begin()
 *   - Releasing the connection with mip.end()
 *   - Re-establishing the connection with mip.begin()
 *   - Putting the MiP into a low-power sleep state with mip.sleep()
 *
 * The sketch prints status messages to Serial1 so you can observe the chest
 * LED behavior and the connection lifecycle. Note that after calling
 * mip.sleep() the MiP typically requires a power cycle before accepting new
 * connections.
 *
 * The example exercises these API calls:
 *   - begin()
 *   - end()
 *   - sleep()
 *
 * @copyright Copyright (C) 2018 Adam Green (https://github.com/adamgreen)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MPU_D1_mini.h>

/**
 * @brief Global MiP instance used to control the robot.
 *
 * @details Use this object to call MiP API functions such as begin(), end(),
 * and sleep(). Keeping the instance at file scope makes it available in both
 * setup() and loop() if the sketch is extended.
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details
 * - Attempts to initialize communication with the MiP using mip.begin().
 * - If the connection fails, prints an error to Serial1 and returns early.
 * - On success, prints explanatory messages and demonstrates:
 *     1. Leaving the connection open for a short period (chest LED should be green).
 *     2. Calling mip.end() to disconnect (chest LED should revert to blue).
 *     3. Reconnecting with mip.begin() to show chest LED returns to green.
 *     4. Calling mip.sleep() to put the MiP into a low-power state (requires power cycle to reconnect).
 *
 * The delays are included so the user can observe LED and connection state changes.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("Sleep.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("Sleep.ino: Shows begin(), end(), and sleep() functionality."));
  Serial1.println(F(" Chest LED should be green to indicate UART connection."));

  // Allow time to observe the connected state.
  delay(5000);

  Serial1.println(F(" Disconnecting from MiP. Chest LED should revert to blue."));
  mip.end();  // Release the UART connection; chest LED typically changes.

  // Wait while disconnected so the LED change is visible.
  delay(5000);

  // Attempt to reconnect to the MiP.
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F(" Sleep.ino: Failed reconnecting to MiP!"));
    return;
  }
  Serial1.println(F(" Attempted to reconnect to MiP. Chest LED should be green again."));

  // Allow time to observe the reconnected state.
  delay(5000);

  Serial1.println(F(" Putting MiP to sleep."));
  Serial1.println(F(" MiP requires a power cycle before accepting connections again."));
  mip.sleep();  // Put MiP into low-power mode.

  Serial1.println(F("Sleep.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs its demonstration in setup() and does not
 * require repeated work in loop(). The function is intentionally left empty
 * so the sketch completes once during initialization.
 */
void loop() {
}
