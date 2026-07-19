/**
 * @file Status.ino
 * @brief Example sketch that monitors and reports MiP status changes.
 *
 * @details
 * This sketch demonstrates how to query and report various status values from
 * a MiP robot using the MiP library. It connects to the robot, then
 * continuously polls for changes to the battery voltage and the robot's
 * positional state. When a change is detected the new value is printed to
 * Serial1. The sketch is intended as a simple diagnostic example to show
 * how to use the status-related API calls.
 *
 * The example exercises these API calls:
 *   - readBatteryVoltage()
 *   - readPosition()
 *   - isOnBack()
 *   - isFaceDown()
 *   - isUpright()
 *   - isPickedUp()
 *   - isHandStanding()
 *   - isFaceDownOnTray()
 *   - isOnBackWithKickstand()
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
 * @brief Global MiP instance used to communicate with the robot.
 *
 * @details Use this object to call the MiP status APIs such as
 * readBatteryVoltage() and readPosition().
 */
MiP mip;

/**
 * @brief Last reported battery voltage (volts).
 *
 * @details Stored so the sketch only prints battery updates when the value
 * actually changes.
 */
static float lastBatteryLevel = 0.0f;

/**
 * @brief Last reported position enum value.
 *
 * @details Initialized to an invalid value so the first readPosition() will
 * always be treated as a change and printed.
 */
static MiPPosition lastPosition = (MiPPosition)-1;

/**
 * @brief Arduino setup function.
 *
 * @details
 * - Initializes the MiP connection via mip.begin().
 * - If the connection fails, prints an error to Serial1 and returns early.
 * - On success, prints a short banner indicating the sketch will display
 *   status changes.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("Status.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("Status.ino: Display MiP status as it changes."));
}

/**
 * @brief Arduino loop function.
 *
 * @details
 * - Polls the MiP for the current battery voltage and position.
 * - If the battery voltage differs from the last reported value, prints the
 *   new voltage and updates lastBatteryLevel.
 * - If the position differs from the last reported position, queries the
 *   various position predicates (isOnBack(), isFaceDown(), isUpright(), etc.)
 *   and prints each matching position description. Updates lastPosition to
 *   avoid repeated prints for the same state.
 *
 * The loop is intentionally lightweight and prints only on changes to avoid
 * spamming the serial output.
 */
void loop() {
  float currentBatteryLevel = mip.readBatteryVoltage();
  MiPPosition currentPosition = mip.readPosition();

  /* Report battery voltage when it changes. */
  if (currentBatteryLevel != lastBatteryLevel) {
    Serial1.print(F(" Battery: "));
    Serial1.print(currentBatteryLevel);
    Serial1.println(F("V"));
    lastBatteryLevel = currentBatteryLevel;
  }

  /* Report position changes by evaluating all position predicates. */
  if (currentPosition != lastPosition) {
    if (mip.isOnBack()) {
      Serial1.println(F(" Position: On Back"));
    }
    if (mip.isFaceDown()) {
      Serial1.println(F(" Position: Face Down"));
    }
    if (mip.isUpright()) {
      Serial1.println(F(" Position: Upright"));
    }
    if (mip.isPickedUp()) {
      Serial1.println(F(" Position: Picked Up"));
    }
    if (mip.isHandStanding()) {
      Serial1.println(F(" Position: Hand Stand"));
    }
    if (mip.isFaceDownOnTray()) {
      Serial1.println(F(" Position: Face Down on Tray"));
    }
    if (mip.isOnBackWithKickstand()) {
      Serial1.println(F(" Position: On Back With Kickstand"));
    }

    lastPosition = currentPosition;
  }
}
