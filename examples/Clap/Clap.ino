/**
 * @file Clap.ino
 * @brief Example sketch demonstrating MiP clap event APIs.
 * @details
 * This Arduino sketch demonstrates how to use the MiP library's clap-related
 * functions to enable and disable clap event reporting, configure the clap
 * detection delay, and read detected clap events. The sketch performs the
 * following sequence in setup():
 *   - Initializes communication with the MiP robot.
 *   - Disables clap events and verifies the disabled state using
 *     areClapEventsEnabled().
 *   - Writes a clap delay value using writeClapDelay() and reads it back with
 *     readClapDelay().
 *   - Enables clap events and verifies the enabled state.
 * After initialization, the loop() continuously checks for available clap
 * events using availableClapEvents() and reads each event with readClapEvent(),
 * printing the number of detected claps to Serial1.
 *
 * The example exercises these API calls:
 *   - enableClapEvents()
 *   - disableClapEvents()
 *   - areClapEventsEnabled()
 *   - writeClapDelay(uint16_t delay)
 *   - readClapDelay()
 *   - availableClapEvents()
 *   - readClapEvent()
 *
 * This sketch prints status and results to Serial1 and is intended for use
 * with the MPU_D1_mini MiP library and a compatible MiP robot.
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
 * @details
 * This object is used throughout the sketch to call MiP API functions such as
 * begin(), enableClapEvents(), disableClapEvents(), writeClapDelay(),
 * readClapDelay(), availableClapEvents(), and readClapEvent().
 */
MiP mip;

/**
 * @brief Tracks whether the initial connection to the MiP succeeded.
 *
 * @details Stored so other parts of the sketch could check connection state
 * if extended.
 */
bool connectResult;

/**
 * @brief Arduino setup function.
 * @details
 * Called once after the board powers up or resets. This function:
 *  - Attempts to initialize communication with the MiP robot via mip.begin().
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

  Serial1.println(F(" Calling disableClapEvents()"));
  mip.disableClapEvents();
  bool isEnabled = mip.areClapEventsEnabled();
  Serial1.print(F(" areClapEventsEnabled() returns "));
  if (isEnabled) {
    Serial1.println(F("true - fail"));
  } else {
    Serial1.println(F("false - pass"));
  }

  Serial1.println(F(" Calling writeClapDelay(501)"));
  mip.writeClapDelay(501);
  uint16_t delay = mip.readClapDelay();
  Serial1.print(F(" readClapDelay() returns "));
  Serial1.println(delay);

  Serial1.println(F(" Calling enableClapEvents()"));
  mip.enableClapEvents();
  isEnabled = mip.areClapEventsEnabled();
  Serial1.print(F(" areClapEventsEnabled() returns "));
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
 * @details
 * Called repeatedly after setup() completes. This implementation polls the
 * MiP device for pending clap events. While availableClapEvents() reports
 * one or more events, readClapEvent() is called to retrieve the clap count
 * for each event and the result is printed to Serial1.
 *
 * The loop is non-blocking except for the time taken to process available
 * events; it returns quickly when no events are pending.
 */
void loop() {
  if (!connectResult) return;  // If connecting to MiP failed in setup(), exit now.
	  
  while (mip.availableClapEvents() > 0) {
    uint8_t clapCount = mip.readClapEvent();
    Serial1.print(F(" Detected "));
    Serial1.print(clapCount);
    Serial1.println(F(" claps"));
  }
}
