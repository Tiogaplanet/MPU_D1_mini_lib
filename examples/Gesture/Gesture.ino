/**
 * @file Gesture.ino
 * @brief Example sketch demonstrating MiP gesture detection and reporting.
 *
 * @details This sketch shows how to use the MiP library's gesture detection
 * APIs to enable gesture mode, poll for gesture events, and report the
 * detected gestures to Serial1. The sketch waits for the robot to be upright
 * before enabling gesture mode and then continuously reads available gesture
 * events using gesture.availableEvents() and gesture.readEvent().
 *
 * The example exercises these API calls:
 *   - gesture.availableEvents()
 *   - gesture.readEvent()
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
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
 * position.isUpright(), gesture.enable(), gesture.availableEvents(), and
 * gesture.readEvent().
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
 * returns early. The function waits until the robot reports it is upright,
 * then enables gesture mode so the robot will begin reporting gesture events.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("Gesture.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(
    F("Gesture.ino: Detect gesture and inform user as they occur."));

  Serial1.println(F(" Waiting for MiP to be standing upright."));
  while (!mip.position.isUpright()) {
    // Waiting for the robot to be upright before enabling gesture mode.
  }
  mip.gesture.enable();
}

/**
 * @brief Arduino loop function.
 *
 * @details Continuously polls for pending gesture events using
 * availableGestureEvents(). For each available event, readGestureEvent()
 * returns a MiPGesture value which is mapped to a human-readable message
 * printed to Serial1. The switch statement covers all defined gesture
 * values including a defensive case for MIP_GESTURE_INVALID.
 */
void loop() {
  if (!connectResult)
    return;  // If connecting to MiP failed in setup(), exit now.

  while (mip.gesture.availableEvents() > 0) {
    MiPGesture gesture = mip.gesture.readEvent();
    Serial1.print(F(" Detected "));
    switch (gesture) {
      case MIP_GESTURE_LEFT:
        Serial1.println(F("Left gesture!"));
        break;
      case MIP_GESTURE_RIGHT:
        Serial1.println(F("Right gesture!"));
        break;
      case MIP_GESTURE_CENTER_SWEEP_LEFT:
        Serial1.println(F("Center Sweep Left gesture!"));
        break;
      case MIP_GESTURE_CENTER_SWEEP_RIGHT:
        Serial1.println(F("Center Sweep Right gesture!"));
        break;
      case MIP_GESTURE_CENTER_HOLD:
        Serial1.println(F("Center Hold gesture!"));
        break;
      case MIP_GESTURE_FORWARD:
        Serial1.println(F("Forward gesture!"));
        break;
      case MIP_GESTURE_BACKWARD:
        Serial1.println(F("Backward gesture!"));
        break;
      case MIP_GESTURE_INVALID:
        /**
       * @note MIP_GESTURE_INVALID should not normally be returned when
       * availableGestureEvents() reported > 0, but handle it defensively.
       */
        Serial1.println(F(" INVALID gesture!"));
        break;
    }
  }
}

