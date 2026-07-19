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
 *   - enableAppMode()
 *   - enableCageMode()
 *   - enableDanceMode()
 *   - enableStackMode()
 *   - enableTrickMode()
 *   - enableRoamMode()
 *   - isAppModeEnabled()
 *   - isCageModeEnabled()
 *   - isDanceModeEnabled()
 *   - isStackModeEnabled()
 *   - isTrickModeEnabled()
 *   - isRoamModeEnabled()
 *
 * @copyright Copyright (C) 2018 Samuel Trassare (https://github.com/Tiogaplanet)
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
 * enableCageMode(), enableDanceMode(), enableStackMode(), enableTrickMode(),
 * enableRoamMode(), enableAppMode(), and the corresponding isXModeEnabled()
 * query functions.
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
}

/**
 * @brief Arduino loop function.
 *
 * @details Repeatedly enables each available game mode on the MiP robot in
 * sequence. After enabling a mode, the sketch queries the corresponding
 * isXModeEnabled() function to verify the mode was activated and prints a
 * confirmation message to Serial1. The sketch then delays for delayPeriod
 * milliseconds before moving to the next mode.
 *
 * Sequence:
 *   - enableCageMode()  -> isCageModeEnabled()
 *   - enableDanceMode() -> isDanceModeEnabled()
 *   - enableStackMode() -> isStackModeEnabled()
 *   - enableTrickMode() -> isTrickModeEnabled()
 *   - enableRoamMode()  -> isRoamModeEnabled()
 *   - enableAppMode()   -> isAppModeEnabled()
 */
void loop() {
  if (!connectResult) return;  // If connecting to MiP failed in setup(), exit now.
	
  mip.enableCageMode();
  if (mip.isCageModeEnabled()) {
    Serial1.println(F(" Cage mode enabled."));
  }
  delay(delayPeriod);

  mip.enableDanceMode();
  if (mip.isDanceModeEnabled()) {
    Serial1.println(F(" Dance mode enabled."));
  }
  delay(delayPeriod);

  mip.enableStackMode();
  if (mip.isStackModeEnabled()) {
    Serial1.println(F(" Stack mode enabled."));
  }
  delay(delayPeriod);

  mip.enableTrickMode();
  if (mip.isTrickModeEnabled()) {
    Serial1.println(F(" Trick mode enabled."));
  }
  delay(delayPeriod);

  mip.enableRoamMode();
  if (mip.isRoamModeEnabled()) {
    Serial1.println(F(" Roam mode enabled."));
  }
  delay(delayPeriod);

  mip.enableAppMode();
  if (mip.isAppModeEnabled()) {
    Serial1.println(F(" App mode enabled."));
  }
  delay(delayPeriod);
}
