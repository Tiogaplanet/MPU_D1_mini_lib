/**
 * @file ContinuousDrive.ino
 * @brief Example sketch demonstrating MiP continuous drive control.
 *
 * @details This sketch shows how to use the MiP library's continuousDrive()
 * function to drive MiP continuously with specified forward/backward speed
 * and turning rate. The example drives forward with a right turn for a fixed
 * interval, drives backward with a left turn for a fixed interval, and then
 * stops. Timing is implemented using millis() so the loop remains non-blocking.
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.motion.continuousDrive()
 *   - mip.motion.stop()
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
 * motion.continuousDrive(), and motion.stop().
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
 * returns early. On success, a status message is printed describing the
 * continuous drive demonstration.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    mip.console.println(F("ContinuousDrive.ino: Failed connecting to MiP!"));
    return;
  }

  mip.console.println(F("ContinuousDrive.ino: Use continuousDrive() function. Drive forward "
                    "with right turn and then backward with left turn."));
}

/**
 * @brief Arduino loop function.
 *
 * @details Implements a non-blocking state machine that runs the continuous
 * drive demonstration. The state machine has three states:
 *   - RIGHT_TURN: Drive forward with a right turn for 2000 ms.
 *   - LEFT_TURN: Drive backward with a left turn for 2000 ms.
 *   - DONE: Stops motion and idles after the demonstration completes.
 *
 * Timing is computed using millis() to avoid blocking delays. While in the
 * RIGHT_TURN and LEFT_TURN states, continuousDrive() is called with signed
 * speed values to indicate direction and turning rate.
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) { return; }

  /**
   * @brief States for the continuous drive demonstration state machine.
   */
  static enum States { RIGHT_TURN, LEFT_TURN, DONE } state = RIGHT_TURN;

  /**
   * @brief Start time for the current state interval in milliseconds.
   */
  static uint32_t startTime = millis();

  uint32_t currentTime = millis();
  uint32_t elapsedTime = currentTime - startTime;

  switch (state) {
    case RIGHT_TURN:
      if (elapsedTime < 2000) {
        /* Drive forward at half-speed (+16) and turn right at half rate (+16).
         * continuousDrive(velocity, turnRate)
         * Positive velocity drives forward; negative velocity drives backward.
         * Positive turnRate turns right; negative turnRate turns left.
         */
        mip.motion.continuousDrive(16, 16);
      } else {
        startTime = currentTime;
        state = LEFT_TURN;
      }
      break;

    case LEFT_TURN:
      if (elapsedTime < 2000) {
        /* Drive backward at half-speed (-16) and turn left at half rate (-16).
         * Negative velocity drives backward; negative turnRate turns left.
         */
        mip.motion.continuousDrive(-16, -16);
      } else {
        // Stop motion when the demonstration completes
        mip.motion.stop();

        mip.console.println();
        mip.console.println(F("ContinuousDrive.ino: Done."));
        state = DONE;
      }
      break;

    case DONE:
    default:
      // Yield CPU time in idle state to prevent watchdog reset triggers
      delay(100);
      break;
  }
}
