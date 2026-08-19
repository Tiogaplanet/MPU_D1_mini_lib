/**
 * @file Gesture.ino
 * @brief Example sketch demonstrating MiP's gesture detection and reporting.
 *
 * @details This sketch shows how to use the MiP library's gesture detection
 * APIs to enable gesture mode, poll for gesture events, and report the
 * detected gestures to mip.console. The sketch waits for MiP to be upright
 * before enabling gesture mode and then continuously reads available gesture
 * events using gesture.availableEvents() and gesture.readEvent().
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.position.isUpright()
 *   - mip.gesture.enable()
 *   - mip.gesture.availableEvents()
 *   - mip.gesture.readEvent()
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
 * position.isUpright(), gesture.enable(), gesture.availableEvents(), and
 * gesture.readEvent().
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
 * returns early. The function waits until MiP reports standing upright,
 * then enables gesture mode so MiP will begin reporting gesture events.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    mip.console.println(F("Gesture.ino: Failed connecting to MiP!"));
    return;
  }

  mip.console.println(F("Gesture.ino: Detect gesture and inform user as they occur."));

  mip.console.println(F(" Waiting for MiP to be standing upright."));
  while (!mip.position.isUpright()) {
    // Yield CPU time to prevent ESP8266 watchdog resets while waiting
    delay(100);
  }

  mip.gesture.enable();
}

/**
 * @brief Arduino loop function.
 *
 * @details Continuously polls for pending gesture events using
 * gesture.availableEvents(). For each available event, gesture.readEvent()
 * returns a MiPGesture value which is mapped to a human-readable message
 * printed to mip.console. The switch statement covers all defined gesture
 * values including a defensive case for MIP_GESTURE_INVALID.
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) { return; }

  while (mip.gesture.availableEvents() > 0) {
    MiPGesture gesture = mip.gesture.readEvent();
    mip.console.print(F(" Detected "));
    switch (gesture) {
      case MIP_GESTURE_LEFT: mip.console.println(F("Left gesture!")); break;
      case MIP_GESTURE_RIGHT: mip.console.println(F("Right gesture!")); break;
      case MIP_GESTURE_CENTER_SWEEP_LEFT:
        mip.console.println(F("Center Sweep Left gesture!"));
        break;
      case MIP_GESTURE_CENTER_SWEEP_RIGHT:
        mip.console.println(F("Center Sweep Right gesture!"));
        break;
      case MIP_GESTURE_CENTER_HOLD:
        mip.console.println(F("Center Hold gesture!"));
        break;
      case MIP_GESTURE_FORWARD: mip.console.println(F("Forward gesture!")); break;
      case MIP_GESTURE_BACKWARD: mip.console.println(F("Backward gesture!")); break;
      case MIP_GESTURE_INVALID:
        /**
         * @note MIP_GESTURE_INVALID should not normally be returned when
         * gesture.availableEvents() reported > 0, but handle it defensively.
         */
        mip.console.println(F(" INVALID gesture!"));
        break;
    }
  }

  // Yield control briefly to prevent watchdog reset triggers
  delay(10);
}
