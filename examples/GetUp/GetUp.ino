/**
 * @file GetUp.ino
 * @brief Example sketch demonstrating MiP get-up behavior after a fall.
 *
 * @details This sketch shows how to use the MiP library to command the robot
 * to intentionally fall forward and then attempt to recover using getUp().
 * The sequence performed in setup() is:
 *   - Initialize communication with the MiP robot.
 *   - Command the robot to fall forward.
 *   - Attempt to get up from the front using getUp(MIP_GETUP_FROM_FRONT).
 * The sketch prints status messages to Serial1 so the user can observe the
 * sequence. The example exercises the following API calls:
 *   - fallForward()
 *   - getUp()
 *
 * @copyright Copyright (C) 2018 Adam Green (https://github.com/adamgreen)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
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
 *   - Commands the robot to fall forward, waits briefly, then attempts to
 *     get up from the front using getUp(MIP_GETUP_FROM_FRONT).
 *   - Prints progress and completion messages to Serial1.
 *
 * Note: The example includes a comment that this particular get-up attempt
 * "never works" on the referenced hardware/firmware; the call is included
 * to demonstrate the API usage.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("GetUp.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("GetUp.ino: Use getUp(). Attempt to get up from a front fall."));
  Serial1.println(F("GetUp.ino: (This one never works!)"));

  /* Command the robot to fall forward. */
  mip.fallForward();
  delay(3000);

  /* Attempt to get up from the front. */
  mip.getUp(MIP_GETUP_FROM_FRONT);
  delay(3000);

  Serial1.println();
  Serial1.println(F("GetUp.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs all actions in setup() and does not require
 * repeated work in loop(). The function is intentionally left empty so the
 * demonstration runs only once during initialization.
 */
void loop() {
}
