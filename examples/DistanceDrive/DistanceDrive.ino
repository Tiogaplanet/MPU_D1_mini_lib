/**
 * @file DistanceDrive.ino
 * @brief Example sketch demonstrating MiP distance-based drive commands.
 *
 * @details This sketch shows how to use the MiP library to queue and execute
 * motion.distanceDrive() commands that move MiP forward/backward and rotate
 * specified degrees. The example queues a sequence of commands:
 *   - Drive forward 30 cm.
 *   - Turn 360 degrees left.
 *   - Turn 360 degrees right.
 *   - Drive backward 30 cm.
 *
 * Commands are queued with consecutive calls to distanceDrive() so MiP
 * executes them in order. Timing and queuing are handled by MiP's device
 * firmware; the sketch simply issues the commands during setup().
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.motion.distanceDrive()
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MiP_Power_Up.h>

/**
 * @brief Global MiP instance used to communicate with MiP.
 *
 * @details Use this object to call MiP API functions such as begin() and
 * motion.distanceDrive().
 */
MiP mip;

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Arduino setup function.
 *
 * @details Initializes communication with MiP by calling mip.begin().
 * If the connection fails, an error message is printed to mip.console and setup
 * returns early. On success, the sketch prints a short description and then
 * queues a series of distanceDrive() commands to be executed by MiP.
 *
 * The queued sequence:
 *   1. Drive forward 30 centimeters without turning.
 *   2. Turn left 360 degrees in place.
 *   3. Turn right 360 degrees in place.
 *   4. Drive backward 30 centimeters without turning.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    mip.console.println(F("DistanceDrive.ino: Failed connecting to MiP!"));
    return;
  }

  mip.console.println(F("DistanceDrive.ino: Use distanceDrive function. Drive forward, turn "
                    "360 degrees in each direction and backward."));

  // Queue up multiple motion commands for MiP to run in sequence.
  // distanceDrive(driveDirection, cm, turnDirection, degrees)
  mip.motion.distanceDrive(MIP_DRIVE_FORWARD, 30, MIP_TURN_RIGHT, 0);
  mip.motion.distanceDrive(MIP_DRIVE_FORWARD, 0, MIP_TURN_LEFT, 360);
  mip.motion.distanceDrive(MIP_DRIVE_FORWARD, 0, MIP_TURN_RIGHT, 360);
  mip.motion.distanceDrive(MIP_DRIVE_BACKWARD, 30, MIP_TURN_RIGHT, 0);

  mip.console.println();
  mip.console.println(F("DistanceDrive.ino: Done."));
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
