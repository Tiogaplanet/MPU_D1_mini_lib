/**
 * @file GetUp.ino
 * @brief Example sketch demonstrating MiP's get-up behavior.
 *
 * @details This sketch shows how to use the MiP library to command MiP
 * to get up from resting on the kickstand and from face down on the tray.
 * As stated in WowWee's documentation, "MiP will attempt to get up from front
 * [or back] if angle is correct." Give MiP some room for this test because
 * MiP drives forward a bit after getting up from the kickstand.
 *
 * The sequence performed in setup() is:
 *   - Initialize communication with MiP.
 *   - Command MiP to get up from resting on the kickstand.
 *   - Command MiP to fall forward on the tray.
 *   - Attempt to get up again.
 *
 * The sketch prints status messages to Serial1 so the user can observe the
 * sequence. The example exercises these API calls:
 *   - mip.begin()
 *   - mip.motion.fallForward()
 *   - mip.motion.getUp()
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
 * motion.fallForward(), and motion.getUp().
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
 *   - If the connection fails, prints an error to Serial1 and returns early.
 *   - Commands MiP to get up from the kickstand, wait for stabilization, then
 * fall forward and attempt to get up again from the front position.
 *   - Prints progress and completion messages to Serial1.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("GetUp.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("GetUp.ino: Get up from the kickstand and tray."));

  Serial1.println(F(" Getting up from kickstand."));
  mip.motion.getUp(MIP_GETUP_FROM_BACK);
  delay(3000);  // Allow MiP enough time to balance and stabilize

  Serial1.println(F(" Falling forward."));
  mip.motion.fallForward();
  delay(2000);  // Allow time for the fall to complete

  Serial1.println(F(" Getting up again."));
  mip.motion.getUp(MIP_GETUP_FROM_FRONT);
  delay(3000);  // Allow MiP enough time to stand up and balance

  Serial1.println(F("GetUp.ino: Done."));
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
