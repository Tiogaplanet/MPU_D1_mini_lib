/**
 * @file Sleep.ino
 * @brief Demonstrates connecting, disconnecting, reconnecting, and sleeping
 * MiP.
 *
 * @details
 * This example shows how to manage a MiP connection from an MPU: D1 mini using
 * the MiP library. It demonstrates:
 *   - Establishing a UART connection with mip.begin()
 *   - Releasing the connection with mip.end()
 *   - Re-establishing the connection with mip.begin()
 *   - Putting MiP into a low-power sleep state with mip.sleep()
 *
 * The sketch prints status messages to mip.console so you can observe the chest
 * LED behavior and the connection lifecycle. Note that after calling
 * mip.sleep() MiP typically requires a power cycle before accepting new
 * connections.
 *
 * The example exercises these API calls:
 *   - begin()
 *   - end()
 *   - sleep()
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
 * @details Use this object to call MiP API functions such as begin(), end(),
 * and sleep(). Keeping the instance at file scope makes it available in both
 * setup() and loop().
 */
MiP mip;

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Arduino setup function.
 *
 * @details
 * - Attempts to initialize communication with MiP using mip.begin().
 * - If the connection fails, prints an error to mip.console and returns early.
 * - On success, prints explanatory messages and demonstrates:
 *     1. Leaving the connection open for a short period (chest LED should be
 * green).
 *     2. Calling mip.end() to disconnect (chest LED should revert to blue).
 *     3. Reconnecting with mip.begin() to show chest LED returns to green.
 *     4. Calling mip.sleep() to put MiP into a low-power state (requires power
 * cycle to reconnect).
 *
 * The delays are included so the user can observe LED and connection state
 * changes.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    mip.console.println(F("Sleep.ino: Failed connecting to MiP."));
    return;
  }

  mip.console.println(F("Sleep.ino: Shows begin(), end(), and sleep() functionality."));
  mip.console.println(F(" Chest LED should be green to indicate UART connection."));

  // Allow time to observe the connected state.
  delay(5000);

  mip.console.println(F(" Disconnecting from MiP. Chest LED should revert to blue."));
  mip.end();  // Release the UART connection; chest LED typically changes.

  // Wait while disconnected so the LED change is visible.
  delay(5000);

  // Attempt to reconnect to MiP.
  connectResult = mip.begin();
  if (!connectResult) {
    mip.console.println(F(" Sleep.ino: Failed reconnecting to MiP."));
    return;
  }
  mip.console.println(F(" Attempted to reconnect to MiP. Chest LED should be green again."));

  // Allow time to observe the reconnected state.
  delay(5000);

  mip.console.println(F(" Putting MiP to sleep."));
  mip.console.println(F(" MiP requires a power cycle before accepting connections again."));
  mip.sleep();  // Put MiP into low-power mode.

  mip.console.println(F("Sleep.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs its demonstration in setup() and does not
 * require repeated work in loop().
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) { return; }
}
