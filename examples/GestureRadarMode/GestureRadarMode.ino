/**
 * @file GestureRadarMode.ino
 * @brief Example sketch demonstrating switching between gesture and radar
 * modes.
 *
 * @details This sketch shows how to enable and disable MiP's radar
 * and gesture sensing modes and how to query their states. It performs a
 * sequence of mode changes in setup() while printing verification messages
 * to mip.console:
 *   - radar.enable() and verify with radar.isEnabled()
 *   - radar.disable() and verify it is disabled
 *   - gesture.enable() and verify with gesture.isEnabled()
 *   - gesture.disable() and verify both gesture and radar modes are disabled
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.radar.enable()
 *   - mip.radar.disable()
 *   - mip.radar.isEnabled()
 *   - mip.gesture.enable()
 *   - mip.gesture.disable()
 *   - mip.gesture.isEnabled()
 *   - mip.gesture.areGestureAndRadarModesDisabled()
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
 * @details Use this object to call MiP API functions such as begin(),
 * radar.enable(), radar.disable(), gesture.enable(),
 * gesture.disable(), and the corresponding query functions.
 */
MiP mip;

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Arduino setup function.
 *
 * @details Called once after power-up or reset. This function:
 *   - Initializes communication with MiP via mip.begin().
 *   - If connection fails, prints an error to mip.console and returns early.
 *   - Demonstrates enabling/disabling radar and gesture modes and prints
 *     pass/fail verification messages using the radar.isEnabled(),
 *     gesture.isEnabled(), and gesture.areGestureAndRadarModesDisabled() query
 * functions.
 *
 * The function intentionally performs the checks in sequence so the user can
 * observe MiP's responses on mip.console.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    mip.console.println(F("GestureRadarMode.ino: Failed connecting to MiP!"));
    return;
  }

  mip.console.println(F("GestureRadarMode.ino: Switches between gesture, radar, and default modes."));

  // 1. Enable radar mode and verify
  mip.console.println(F(" Calling mip.radar.enable()"));
  mip.radar.enable();
  mip.console.print(F(" mip.radar.isEnabled() = "));
  if (mip.radar.isEnabled()) {
    mip.console.println(F("true - Pass"));
  } else {
    mip.console.println(F("false - Failed"));
  }

  // 2. Disable radar mode and verify
  mip.console.println(F(" Calling mip.radar.disable()"));
  mip.radar.disable();
  mip.console.print(F(" mip.radar.isEnabled() = "));
  if (mip.radar.isEnabled()) {
    mip.console.println(F("true - Failed"));
  } else {
    mip.console.println(F("false - Pass"));
  }

  // 3. Enable gesture mode and verify
  mip.console.println(F(" Calling mip.gesture.enable()"));
  mip.gesture.enable();
  mip.console.print(F(" mip.gesture.isEnabled() = "));
  if (mip.gesture.isEnabled()) {
    mip.console.println(F("true - Pass"));
  } else {
    mip.console.println(F("false - Failed"));
  }

  // 4. Disable gesture mode and verify both modes disabled
  mip.console.println(F(" Calling mip.gesture.disable()"));
  mip.gesture.disable();
  mip.console.print(F(" mip.gesture.isEnabled() = "));
  if (mip.gesture.isEnabled()) {
    mip.console.println(F("true - Failed"));
  } else {
    mip.console.println(F("false - Pass"));
  }

  mip.console.print(F(" mip.gesture.areGestureAndRadarModesDisabled() = "));
  if (mip.gesture.areGestureAndRadarModesDisabled()) {
    mip.console.println(F("true - Pass"));
  } else {
    mip.console.println(F("false - Failed"));
  }

  mip.console.println();
  mip.console.println(F("GestureRadarMode.ino: Done."));
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
