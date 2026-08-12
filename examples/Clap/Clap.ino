/**
 * @file Clap.ino
 * @brief Example sketch demonstrating MiP clap event APIs.
 *
 * @details This Arduino sketch demonstrates how to use MiP's clap-related
 * functions to enable and disable clap event reporting, configure clap
 * detection delay, and read detected clap events. The sketch performs the
 * following sequence in setup():
 *   - Initializes communication with MiP.
 *   - Disables clap events and verifies the disabled state using
 *     clap.areEventsEnabled().
 *   - Writes a clap delay value using clap.writeDelay() and reads it back with
 *     clap.readDelay().
 *   - Enables clap events and verifies the enabled state.
 * After initialization, loop() continuously checks for available clap
 * events using clap.availableEvents() and reads each event with
 * clap.readEvent(), printing the number of detected claps to Serial1.
 *
 * The example exercises these API calls:
 *   - clap.enableEvents()
 *   - clap.disableEvents()
 *   - clap.areEventsEnabled()
 *   - clap.writeDelay(uint16_t delayTime)
 *   - clap.readDelay()
 *   - clap.availableEvents()
 *   - clap.readEvent()
 *
 * This sketch prints status and results to Serial1 and is intended for use
 * with the MiP Power Up - D1 mini library and MiP.
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
 * This object is used throughout the sketch to call MiP API functions such as
 * begin(), clap.enableEvents(), clap.disableEvents(), clap.writeDelay(),
 * clap.readDelay(), clap.availableEvents(), and clap.readEvent().
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
 *  - Attempts to initialize communication with MiP via mip.begin().
 *  - If connection fails, prints an error to Serial1 and returns early.
 *  - Demonstrates disabling clap events and verifies the disabled state.
 *  - Writes a clap delay value (501 ms) and reads it back to verify.
 *  - Enables clap events and verifies the enabled state.
 *  - Prints a message indicating the sketch is waiting for clap events.
 *
 * The function prints status messages and verification results to Serial1.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("Clap.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("Clap.ino: Use clap related functions."));

  // 1. Test disabling clap events and verifying disabled state
  Serial1.println(F(" Calling clap.disableEvents()"));
  mip.clap.disableEvents();
  bool isEnabled = mip.clap.areEventsEnabled();
  Serial1.print(F(" clap.areEventsEnabled() returns "));
  if (isEnabled) {
    Serial1.println(F("true - fail"));
  } else {
    Serial1.println(F("false - pass"));
  }

  // 2. Test configuring and reading back inter-clap delay
  Serial1.println(F(" Calling clap.writeDelay(501)"));
  mip.clap.writeDelay(501);
  uint16_t clapDelayMs = mip.clap.readDelay();
  Serial1.print(F(" clap.readDelay() returns "));
  Serial1.print(clapDelayMs);
  Serial1.println(F(" ms"));

  // 3. Test enabling clap events and verifying enabled state
  Serial1.println(F(" Calling clap.enableEvents()"));
  mip.clap.enableEvents();
  isEnabled = mip.clap.areEventsEnabled();
  Serial1.print(F(" clap.areEventsEnabled() returns "));
  if (isEnabled) {
    Serial1.println(F("true - pass"));
  } else {
    Serial1.println(F("false - fail"));
  }

  Serial1.println();
  Serial1.println(F(" Waiting for clap events!"));
}

/**
 * @brief Arduino loop function.
 *
 * @details Called repeatedly after setup() completes. This implementation
 * polls MiP for pending clap events. While clap.availableEvents() reports one
 * or more events, clap.readEvent() is called to retrieve the clap count for
 * each event and the result is printed to Serial1.
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) { return; }

  // Poll for available clap events
  while (mip.clap.availableEvents() > 0) {
    uint8_t clapCount = mip.clap.readEvent();
    Serial1.print(F(" Detected "));
    Serial1.print(clapCount);
    Serial1.println(F(" clap(s)"));
  }

  // Yield control briefly to prevent watchdog reset triggers
  delay(10);
}
