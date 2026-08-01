/**
 * @file Stop.ino
 * @brief Demonstrates interrupting motion with the MiP stop() API.
 *
 * @details
 * This simple example shows how to start a long turn and then interrupt it
 * using the motion.stop() function. The sketch:
 *   - Connects to the MiP using begin().
 *   - Commands a 360-degree left turn with motion.turnLeft(360, 6).
 *   - Waits briefly and then calls motion.stop() to interrupt the motion.
 *   - Prints progress messages to Serial1 for observation.
 *
 * The example exercises these API calls:
 *   - begin()
 *   - motion.turnLeft()
 *   - motion.stop()
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MiP_Power_Up_-_D1_mini.h>

/**
 * @brief Global MiP instance used to control the robot.
 *
 * @details Use this object to call MiP API functions such as begin(),
 * turnLeft(), and stop(). Keeping the instance at file scope makes it
 * available in both setup() and loop().
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details
 * - Initializes the MiP connection via mip.begin().
 * - If the connection fails, prints an error to Serial1 and returns early.
 * - On success, demonstrates starting a 360-degree left turn and then
 *   interrupting it with mip.stop() after a short delay.
 *
 * The printed messages help the user observe when the turn starts and when
 * it is interrupted.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("Stop.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("Stop.ino: Use stop() function. Interrupt a 360 degree turn with stop()."));

  // Start a 360-degree left turn at speed 6.
  mip.motion.turnLeft(360, 6);

  // Wait briefly to allow the turn to begin, then interrupt it.
  delay(1000);
  mip.motion.stop();
  delay(1000);

  Serial1.println();
  Serial1.println(F("Stop.ino: Done."));
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

