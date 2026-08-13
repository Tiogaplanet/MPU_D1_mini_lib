/**
 * @file TurnLeftRight.ino
 * @brief Example sketch demonstrating turning MiP left and right.
 *
 * @details
 * This sketch connects to MiP and alternately commands turning 180 degrees left
 * and then 180 degrees right three times. It demonstrates how to use the MiP
 * motion APIs to perform rotational movements and how to sequence motions
 * using simple delays.
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.motion.turnLeft()
 *   - mip.motion.turnRight()
 *
 * Usage notes:
 *   - Ensure MiP is powered and able to turn in the available space.
 *   - Adjust turn angles and speeds passed to turnLeft/turnRight to change
 *     behavior and responsiveness.
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
 * @brief Global MiP instance used to communicate with MiP.
 *
 * @details Use this object to call MiP API functions such as begin(),
 * motion.turnLeft(), and motion.turnRight(). Keeping the instance at file scope
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
 * - If the connection fails, prints an error to Serial1 and returns early.
 * - On success, prints a short description and then performs three cycles of:
 *     1) motion.turnLeft(180, 12)
 *     2) delay(2000)
 *     3) motion.turnRight(180, 12)
 *     4) delay(2000)
 *
 * The second parameter to turnLeft/turnRight controls speed; adjust as needed.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("TurnLeftRight.ino: Failed connecting to MiP."));
    return;
  }

  Serial1.println(F("TurnLeftRight.ino: Use turnLeft & turnRight() functions. "
                    "Turn 180 degrees to left and then 180 degrees to right."));

  for (uint8_t i = 0; i < 3; i++) {
    // Turn 180 degrees left at speed 12.
    mip.motion.turnLeft(180, 12);
    delay(2000);

    // Turn 180 degrees right at speed 12.
    mip.motion.turnRight(180, 12);
    delay(2000);
  }

  Serial1.println();
  Serial1.println(F("TurnLeftRight.ino: Done."));
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
