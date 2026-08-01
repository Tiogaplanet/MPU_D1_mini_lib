/**
 * @file EnableGameMode.ino
 * @brief Example sketch demonstrating enabling MiP game modes.
 *
 * @details This sketch cycles through the MiP robot's built-in game modes
 * (Cage, Dance, Stack, Trick, Roam, App), enabling each mode in turn and
 * verifying the change using the corresponding isXModeEnabled() query.
 * It prints status messages to Serial1 and pauses between mode changes so
 * the behavior can be observed. The delay between mode changes can be
 * shortened for bench testing or lengthened to watch the robot perform.
 *
 * The example exercises these API calls:
 *   - mode.enableApp()
 *   - mode.enableCage()
 *   - mode.enableDance()
 *   - mode.enableStack()
 *   - mode.enableTrick()
 *   - mode.enableRoam()
 *   - mode.isAppEnabled()
 *   - mode.isCageEnabled()
 *   - mode.isDanceEnabled()
 *   - mode.isStackEnabled()
 *   - mode.isTrickEnabled()
 *   - mode.isRoamEnabled()
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
 * mode.enableCage(), mode.enableDance(), mode.enableStack(),
 * mode.enableTrick(), mode.enableRoam(), mode.enableApp(), and the
 * corresponding mode.isXEnabled() query functions.
 */
MiP mip;

/**
 * @brief Delay period between mode changes in milliseconds.
 *
 * @details Set to a short value (10000 ms) for bench testing with Serial1,
 * or increase to observe the robot's behavior for longer intervals.
 */
int delayPeriod = 10000;

/**
 * @brief Tracks whether the initial connection to the MiP succeeded.
 *
 * @details Stored so other parts of the sketch could check connection state
 * if extended.
 */
bool connectResult;

/**
 * @brief Arduino setup function.
 *
 * @details Initializes communication with the MiP robot by calling mip.begin().
 * If the connection fails, an error message is printed to Serial1 and setup
 * returns early. On success, a brief status message is printed and the sketch
 * waits briefly before entering the main loop that cycles through modes.
 */
void setup() {
  // First need to initialize the Serial1 connection with the MiP.
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("EnableGameMode.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("EnableGameMode.ino: Cycles through each mode available."));

  delay(500);

  // Reset MiP's volume to default so you can hear him cycling through the modes.
  mip.sound.writeVolume(MIP_VOLUME_7);
}

/**
 * @brief Arduino loop function.
 *
 * @details Repeatedly enables each available game mode on the MiP robot in
 * sequence. After enabling a mode, the sketch queries the corresponding
 * mode.isXEnabled() function to verify the mode was activated and prints a
 * confirmation message to Serial1. The sketch then delays for delayPeriod
 * milliseconds before moving to the next mode.
 *
 * Sequence:
 *   - mode.enableCage()  -> mode.isCageEnabled()
 *   - mode.enableDance() -> mode.isDanceEnabled()
 *   - mode.enableStack() -> mode.isStackEnabled()
 *   - mode.enableTrick() -> mode.isTrickEnabled()
 *   - mode.enableRoam()  -> mode.isRoamEnabled()
 *   - mode.enableApp()   -> mode.isAppEnabled()
 */
void loop() {
  if (!connectResult)
    return;  // If connecting to MiP failed in setup(), exit now.

  mip.mode.enableCage();
  if (mip.mode.isCageEnabled()) {
    Serial1.println(F(" Cage mode enabled."));
  }
  delay(delayPeriod);

  mip.mode.enableDance();
  if (mip.mode.isDanceEnabled()) {
    Serial1.println(F(" Dance mode enabled."));
  }
  delay(delayPeriod);

  mip.mode.enableStack();
  if (mip.mode.isStackEnabled()) {
    Serial1.println(F(" Stack mode enabled."));
  }
  delay(delayPeriod);

  mip.mode.enableTrick();
  if (mip.mode.isTrickEnabled()) {
    Serial1.println(F(" Trick mode enabled."));
  }
  delay(delayPeriod);

  mip.mode.enableRoam();
  if (mip.mode.isRoamEnabled()) {
    Serial1.println(F(" Roam mode enabled."));
  }
  delay(delayPeriod);

  mip.mode.enableApp();
  if (mip.mode.isAppEnabled()) {
    Serial1.println(F(" App mode enabled."));
  }
  delay(delayPeriod);
}

