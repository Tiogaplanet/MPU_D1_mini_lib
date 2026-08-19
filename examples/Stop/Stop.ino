/**
 * @file Stop.ino
 * @brief Demonstrates interrupting motion with the MiP motion.stop() API.
 *
 * @details
 * This simple example shows how to start a long turn and then interrupt it
 * using the motion.stop() function. The sketch:
 *   - Connects to MiP using mip.begin().
 *   - Commands a 360-degree left turn with motion.turnLeft(360, 6).
 *   - Waits briefly and then calls motion.stop() to interrupt the motion.
 *   - Prints progress messages to mip.console for observation.
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.motion.turnLeft()
 *   - mip.motion.stop()
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
 * motion.turnLeft(), and motion.stop(). Keeping the instance at file scope
 * makes it available in both setup() and loop().
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
 * - Initializes the MiP connection via mip.begin().
 * - If the connection fails, prints an error to mip.console and returns early.
 * - On success, demonstrates starting a 360-degree left turn and then
 *   interrupting it with mip.motion.stop() after a short delay.
 *
 * The printed messages help the user observe when the turn starts and when
 * it is interrupted.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    mip.console.println(F("Stop.ino: Failed connecting to MiP."));
    return;
  }

  mip.console.println(F("Stop.ino: Use stop() function. Interrupt a 360 degree turn with stop()."));

  // Start a 360-degree left turn at speed 6.
  mip.motion.turnLeft(360, 6);

  // Wait briefly to allow the turn to begin, then interrupt it.
  delay(1000);
  mip.motion.stop();
  delay(1000);

  mip.console.println();
  mip.console.println(F("Stop.ino: Done."));
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
