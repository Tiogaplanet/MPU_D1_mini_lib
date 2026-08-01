/**
 * @file GetUp.ino
 * @brief Example sketch demonstrating MiP's get-up behavior.
 *
 * @details This sketch shows how to use the MPU:D1 mini library to command MiP
 * to get up from resting on the kickstand and from face down on the tray.
 * As stated in WowWee's documentation, "Mip will attempt to get up from front
 * [or back] if angle is correct." Give MiP some room for this test because it
 * does drive forward a bit after getting up from the kickstand.
 *
 * The sequence performed in setup() is:
 *   - Initialize communication with the MiP robot.
 *   - Command MiP to get up from resting on the kickstand.
 *   - Command MiP to fall forward on the tray.
 *   - Attempt to get up again.
 *
 * The sketch prints status messages to Serial1 so the user can observe the
 * sequence. The example exercises the following API calls:
 *   - motion.fallForward()
 *   - motion.getUp()
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MPU_D1_mini.h>

/**
 * @brief Global MiP instance used to communicate with the robot.
 *
 * @details Use this object to call MiP API functions such as begin(),
 * fallForward(), and getUp().
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details Called once after the board powers up or resets. This function:
 *   - Initializes communication with the MiP robot via mip.begin().
 *   - If the connection fails, prints an error to Serial1 and returns early.
 *   - Commands MiP to get up from the kickstand then fall forward, wait 
 *     briefly, then attempt to get up from the tray.
 *   - Prints progress and completion messages to Serial1.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("GetUp.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("GetUp.ino: Get up from the kickstand and tray."));

  Serial1.println(F(" Getting up from kickstand."));
  mip.motion.getUp(MIP_GETUP_FROM_BACK);
  delay(1000);

  Serial1.println(F(" Falling forward."));
  mip.motion.fallForward();
  delay(1000);

  Serial1.println(F(" Getting up again."));
  mip.motion.getUp(MIP_GETUP_FROM_FRONT);
  delay(3000);

  Serial1.println(F("GetUp.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs all actions in setup() and does not require
 * repeated work in loop(). The function is intentionally left empty so the
 * demonstration runs only once during initialization.
 */
void loop() {}

