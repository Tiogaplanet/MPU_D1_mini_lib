/**
 * @file TurnLeftRight.ino
 * @brief Example sketch demonstrating turning the MiP left and right.
 *
 * @details
 * This sketch connects to a MiP robot and alternately commands it to turn
 * 180 degrees left and then 180 degrees right three times. It demonstrates
 * how to use the MiP motion APIs to perform rotational movements and how to
 * interrupt or sequence motions using simple delays.
 *
 * The example exercises these API calls:
 *   - begin()
 *   - turnLeft()
 *   - turnRight()
 *
 * Usage notes:
 *   - Ensure the MiP is powered and able to turn in the available space.
 *   - Adjust turn angles and speeds passed to turnLeft/turnRight to change
 *     behavior and responsiveness.
 *
 * @copyright Copyright (C) 2018 Adam Green (https://github.com/adamgreen)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <MPU_D1_mini.h>

/**
 * @brief Global MiP instance used to control the robot.
 *
 * @details Use this object to call MiP API functions such as begin(),
 * turnLeft(), and turnRight(). Keeping the instance at file scope makes it
 * available in both setup() and loop().
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details
 * - Initializes the MiP connection via mip.begin().
 * - If the connection fails, prints an error to Serial1 and returns early.
 * - On success, prints a short description and then performs three cycles of:
 *     1) turnLeft(180, 12)
 *     2) delay(2000)
 *     3) turnRight(180, 12)
 *     4) delay(2000)
 *
 * The second parameter to turnLeft/turnRight controls speed; adjust as needed.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("TurnLeftRight.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("TurnLeftRight.ino: Use turnLeft & turnRight() functions. Turn 180 degrees to left and then 180 degrees to right."));

  for (uint8_t i = 0; i < 3; i++) {
    // Turn 180 degrees left at speed 12.
    mip.turnLeft(180, 12);
    delay(2000);

    // Turn 180 degrees right at speed 12.
    mip.turnRight(180, 12);
    delay(2000);
  }

  Serial1.println();
  Serial1.println(F("TurnLeftRight.ino: Done."));
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
