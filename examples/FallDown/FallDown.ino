/**
 * @file FallDown.ino
 * @brief Example sketch demonstrating MiP fall forward and backward actions.
 *
 * @details This sketch demonstrates how to use the MiP library to command the
 * robot to intentionally fall forward and backward using the fallForward()
 * and fallBackward() APIs. The sketch first waits for the robot to be
 * standing upright (isUpright()) before issuing each fall command and prints
 * status messages to Serial1 so the sequence can be observed.
 *
 * The example exercises these API calls:
 *   - fallForward()
 *   - fallBackward()
 *   - isUpright()
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
 * isUpright(), fallForward(), and fallBackward().
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details Initializes communication with the MiP robot by calling mip.begin().
 * If the connection fails, an error message is printed to Serial1 and setup
 * returns early. On success, the function:
 *   - Waits until the robot reports it is upright using isUpright().
 *   - Pauses briefly to ensure stability.
 *   - Commands the robot to fall forward with fallForward().
 *   - Waits again for the robot to become upright, then commands a fall
 *     backward with fallBackward().
 *
 * The function prints progress and status messages to Serial1 to make the
 * demonstration easy to follow.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("FallDown.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("FallDown.ino: Fall forward and backward.\n"));

  Serial1.println(F(" Waiting for robot to be standing upright."));
  while (!mip.isUpright()) {
    // Waiting for the robot to report upright state.
  }
  delay(1000);

  Serial1.println(F(" Falling forward."));
  mip.fallForward();

  delay(1000);
  Serial1.println(F(" Waiting for robot to be standing upright again."));
  while (!mip.isUpright()) {
    // Waiting for the robot to report upright state after falling.
  }
  delay(1000);

  Serial1.println(F(" Falling backward."));
  mip.fallBackward();

  Serial1.println();
  Serial1.println(F("FallDown.ino: Done."));
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
