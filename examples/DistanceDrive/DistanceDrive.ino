/**
 * @file DistanceDrive.ino
 * @brief Example sketch demonstrating MiP distance-based drive commands.
 *
 * @details This sketch shows how to use the MiP library to queue and execute
 * motion.distanceDrive() commands that move the robot forward/backward and
 * rotate it by specified degrees. The example queues a sequence of commands:
 *   - Drive forward a short distance.
 *   - Turn 360 degrees left.
 *   - Turn 360 degrees right.
 *   - Drive backward a short distance.
 *
 * Commands are queued with consecutive calls to distanceDrive() so the robot
 * executes them in order. Timing and blocking are handled by the MiP device
 * firmware; the sketch simply issues the commands during setup().
 *
 * The example exercises these API calls:
 *   - motion.distanceDrive()
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
 * @details Use this object to call MiP API functions such as begin() and
 * distanceDrive().
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details Initializes communication with the MiP robot by calling mip.begin().
 * If the connection fails, an error message is printed to Serial1 and setup
 * returns early. On success, the sketch prints a short description and then
 * queues a series of distanceDrive() commands to be executed by the robot.
 *
 * The queued sequence:
 *   1. Drive forward a short distance (30 units) without turning.
 *   2. Turn left 360 degrees in place.
 *   3. Turn right 360 degrees in place.
 *   4. Drive backward a short distance (30 units) without turning.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("DistanceDrive.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(
    F("DistanceDrive.ino: Use distanceDrive function. Drive forward, turn "
      "360 degrees in each direction and backward."));

  // Queue up multiple commands to run in sequence.
  mip.motion.distanceDrive(MIP_DRIVE_FORWARD, 30, MIP_TURN_RIGHT, 0);
  mip.motion.distanceDrive(MIP_DRIVE_FORWARD, 0, MIP_TURN_LEFT, 360);
  mip.motion.distanceDrive(MIP_DRIVE_FORWARD, 0, MIP_TURN_RIGHT, 360);
  mip.motion.distanceDrive(MIP_DRIVE_BACKWARD, 30, MIP_TURN_RIGHT, 0);

  Serial1.println();
  Serial1.println(F("DistanceDrive.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs all actions in setup() and does not require
 * repeated work in loop(). The function is intentionally left empty so the
 * queued distanceDrive() commands can execute on the robot without further
 * intervention from the sketch.
 */
void loop() {}

