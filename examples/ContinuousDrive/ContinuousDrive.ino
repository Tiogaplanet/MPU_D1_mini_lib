/**
 * @file ContinuousDrive.ino
 * @brief Example sketch demonstrating MiP continuous drive control.
 *
 * @details This sketch shows how to use the MiP library's continuousDrive()
 * function to drive the robot continuously with specified forward/backward
 * speed and turning rate. The example drives forward with a right turn for
 * a fixed interval, then drives backward with a left turn for a fixed
 * interval, and then stops. Timing is implemented using millis() so the
 * loop remains non-blocking.
 *
 * The example exercises these API calls:
 *   - motion.continuousDrive()
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
 * @details Use this object to call MiP API functions such as begin() and
 * continuousDrive().
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
 *
 * @details Initializes communication with the MiP robot by calling mip.begin().
 * If the connection fails, an error message is printed to Serial1 and setup
 * returns early. On success, a brief status message is printed describing the
 * continuous drive demonstration.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("ContinuousDrive.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("ContinuousDrive.ino: Use continuousDrive() function. Drive forward with right turn and then backward with left turn."));
}

/**
 * @brief Arduino loop function.
 *
 * @details Implements a simple state machine that runs the continuous drive
 * demonstration. The state machine has three states:
 *   - RIGHT_TURN: drive forward with a right turn for 2000 ms.
 *   - LEFT_TURN: drive backward with a left turn for 2000 ms.
 *   - DONE: idle state after demonstration completes.
 *
 * Timing is computed using millis() to avoid blocking delays. While in the
 * RIGHT_TURN and LEFT_TURN states, continuousDrive() is called with signed
 * speed values to indicate direction and turning.
 */
void loop() {
  if (!connectResult) return;  // If connecting to MiP failed in setup(), exit now.
  
  /**
   * @brief States for the continuous drive demonstration.
   *
   * @details RIGHT_TURN drives forward with a right turn, LEFT_TURN drives
   * backward with a left turn, DONE indicates the sequence has completed.
   */
  static enum States {
    RIGHT_TURN,
    LEFT_TURN,
    DONE
  } state = RIGHT_TURN;

  /**
   * @brief Start time for the current state interval.
   *
   * @details Initialized once to the current millis() value and updated
   * when transitioning between states to measure elapsed time per state.
   */
  static uint32_t startTime = millis();

  uint32_t currentTime = millis();
  uint32_t elapsedTime = currentTime - startTime;

  switch (state) {
    case RIGHT_TURN:
      if (elapsedTime < 2000) {
        /* Drive forward at half-speed and turn right at half rate.
         * continuousDrive(forwardSpeed, turnRate)
         * Positive values drive forward; negative values drive backward.
         * Here both parameters are positive to indicate forward motion with
         * a rightward turning component.
         */
        mip.motion.continuousDrive(16, 16);
      } else {
        startTime = currentTime;
        state = LEFT_TURN;
      }
      break;
    case LEFT_TURN:
      if (elapsedTime < 2000) {
        /* Drive backward at half-speed and turn left at half rate.
         * Negative forwardSpeed indicates backward motion; negative turnRate
         * indicates leftward turning component in this usage.
         */
        mip.motion.continuousDrive(-16, -16);
      } else {
        Serial1.println();
        Serial1.println(F("ContinuousDrive.ino: Done."));
        state = DONE;
      }
      break;
    default:
      break;
  }
}
