/**
 * @file DriveForwardBackward.ino
 * @brief Example sketch demonstrating MiP forward and backward drive commands.
 *
 * @details This sketch shows how to use the MiP library's driveForward() and
 * driveBackward() functions to move the robot a fixed speed for a specified
 * duration. The example drives forward for one second, waits, then drives
 * backward for one second. It prints status messages to Serial1 to indicate
 * progress and completion.
 *
 * The example exercises these API calls:
 *   - driveForward()
 *   - driveBackward()
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
 * driveForward(), and driveBackward().
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details Called once after the board powers up or resets. This function:
 *  - Initializes communication with the MiP robot by calling mip.begin().
 *  - If the connection fails, prints an error to Serial1 and returns early.
 *  - If successful, prints a description of the demonstration and issues
 *    a forward drive command for 1000 ms, waits 2000 ms, then issues a
 *    backward drive command for 1000 ms and waits another 2000 ms.
 *
 * The function demonstrates non-blocking command usage where the MiP device
 * handles the timed motion while the sketch issues the commands.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("DriveForwardBackward.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("DriveForwardBackward.ino: Use driveForward() and driveBackward() functions. Drive ahead and back, 1 second in each direction."));

  /* Drive forward at speed 15 for 1000 milliseconds. */
  mip.driveForward(15, 1000);
  /* Wait 2000 ms to allow the forward motion to complete and provide a pause. */
  delay(2000);

  /* Drive backward at speed 15 for 1000 milliseconds. */
  mip.driveBackward(15, 1000);
  /* Wait 2000 ms to allow the backward motion to complete and provide a pause. */
  delay(2000);

  Serial1.println();
  Serial1.println(F("DriveForwardBackward.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs all actions in setup() and does not require
 * repeated work in loop(). The function is intentionally left empty so the
 * sketch does not issue additional commands after the demonstration completes.
 */
void loop() {
}
