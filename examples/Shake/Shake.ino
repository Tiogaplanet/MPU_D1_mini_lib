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
 *   - hasBeenShaken()
 *
 * Typical usage:
 *   - Load this sketch onto the MPU: D1 mini connected to a MiP Power Up.
 *   - Open Serial1 to observe "Shake detected!" messages when the robot is shaken.
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
 * @details Use this object to call MiP API functions such as begin() and
 * hasBeenShaken(). Keeping the instance at file scope makes it available
 * throughout setup() and loop().
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details Initializes the MiP connection by calling mip.begin(). If the
 * connection fails, an error message is printed to Serial1 and the sketch
 * returns early. On success, a short description is printed to Serial1 to
 * indicate the sketch is ready to detect shakes.
 */
void setup() {
  bool connectResult = mip.begin();
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
  if (mip.hasBeenShaken()) {
    Serial1.println(F(" Shake detected!"));
  }
}
