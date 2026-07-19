/**
 * @file GestureRadarMode.ino
 * @brief Example sketch demonstrating switching between gesture and radar modes.
 *
 * @details This sketch shows how to enable and disable the MiP robot's radar
 * and gesture sensing modes and how to query their states. It performs a
 * sequence of mode changes in setup() while printing verification messages
 * to Serial1:
 *   - enableRadarMode() and verify with isRadarModeEnabled()
 *   - disableRadarMode() and verify it is disabled
 *   - enableGestureMode() and verify with isGestureModeEnabled()
 *   - disableGestureMode() and verify both gesture and radar modes are disabled
 *
 * The example exercises these API calls:
 *   - enableRadarMode()
 *   - disableRadarMode()
 *   - enableGestureMode()
 *   - disableGestureMode()
 *   - isRadarModeEnabled()
 *   - isGestureModeEnabled()
 *   - areGestureAndRadarModesDisabled()
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
 * enableRadarMode(), disableRadarMode(), enableGestureMode(),
 * disableGestureMode(), and the corresponding query functions.
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details Called once after power-up or reset. This function:
 *   - Initializes communication with the MiP via mip.begin().
 *   - If connection fails, prints an error to Serial1 and returns early.
 *   - Demonstrates enabling/disabling radar and gesture modes and prints
 *     pass/fail verification messages using the isXModeEnabled() and
 *     areGestureAndRadarModesDisabled() query functions.
 *
 * The function intentionally performs the checks in sequence so the user can
 * observe the robot's responses on Serial1.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("GestureRadarMode.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("GestureRadarMode.ino: Switches between gesture, radar, and default modes."));

  Serial1.println(F(" Calling mip.enableRadarMode()"));
  mip.enableRadarMode();
  Serial1.print(F(" mip.isRadarModeEnabled() = "));
  if (mip.isRadarModeEnabled()) {
    Serial1.println(F("true - Pass"));
  } else {
    Serial1.println(F("false - Failed"));
  }

  Serial1.println(F(" Calling mip.disableRadarMode()"));
  mip.disableRadarMode();
  Serial1.print(F(" mip.isRadarModeEnabled() = "));
  if (mip.isRadarModeEnabled()) {
    Serial1.println(F("true - Failed"));
  } else {
    Serial1.println(F("false - Pass"));
  }

  Serial1.println(F(" Calling mip.enableGestureMode()"));
  mip.enableGestureMode();
  Serial1.print(F(" mip.isGestureModeEnabled() = "));
  if (mip.isGestureModeEnabled()) {
    Serial1.println(F("true - Pass"));
  } else {
    Serial1.println(F("false - Failed"));
  }

  Serial1.println(F(" Calling mip.disableGestureMode()"));
  mip.disableGestureMode();
  Serial1.print(F(" mip.isGestureModeEnabled() = "));
  if (mip.isGestureModeEnabled()) {
    Serial1.println(F("true - Failed"));
  } else {
    Serial1.println(F("false - Pass"));
  }
  Serial1.print(F(" mip.areGestureAndRadarModesDisabled() = "));
  if (mip.areGestureAndRadarModesDisabled()) {
    Serial1.println(F("true - Pass"));
  } else {
    Serial1.println(F("false - Failed"));
  }

  Serial1.println();
  Serial1.println(F("GestureRadarMode.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs all actions in setup() and does not require
 * repeated work in loop(). The function is intentionally left empty so the
 * sketch completes its verification sequence and remains idle.
 */
void loop() {
}
