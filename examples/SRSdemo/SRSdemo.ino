/**
 * @file SRSdemo.ino
 * @brief Seattle Robotics Society demo showing gesture-driven eye animations.
 *
 * @details
 * This sketch was used as a live demonstration at the Seattle Robotics Society
 * meeting on April 21, 2018. It connects to a MiP robot, waits for the robot
 * to stand upright, enables gesture mode, and then responds to left/right
 * hand-sweep gestures by playing a short eye LED animation in the direction
 * of the gesture. When idle the outer eye LEDs blink rapidly to indicate the
 * robot is ready for input.
 *
 * The sketch implements a small state machine with the following states:
 *   - RESTART: clear LEDs and disable gesture mode while waiting for upright.
 *   - WAITING_TO_STANDUP: wait for the robot to report upright.
 *   - WAITING_FOR_GESTURE: blink outer eyes and poll for left/right gestures.
 *   - PLAYING_EYE_ANIMATION: play a 5-frame eye animation in the chosen
 * direction.
 *
 * The example demonstrates use of these MiP APIs:
 *   - begin(), position.isUpright(), gesture.enable(), gesture.disable()
 *   - headLEDs.write()
 *
 * The helper functions blinkOuterEyes() and animateEyes() encapsulate the
 * LED keyframe updates used by the animation sequence.
 *
 * A note from the maintainer: I have updated the names of the function calls
 * as they are named in the MPU:D1 mini library. Nothing else is changed.
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
 * @brief Global MiP instance used to communicate with the robot.
 *
 * @details Use this object to call MiP API functions such as begin(),
 * position.isUpright(), gesture.enable(), gesture.disable(), and
 * headLEDs.write().
 */
static MiP g_mip;

/**
 * @brief Local copy of the head LED state used for animations.
 *
 * @details The MiPHeadLEDs struct stores the state for the four head LEDs
 * (led1..led4). The sketch updates this struct and writes it to the robot
 * with g_mip.writeHeadLEDs().
 */
static MiPHeadLEDs g_headLEDs;

/**
 * @brief Tracks whether the initial connection to the MiP succeeded.
 *
 * @details Stored so other parts of the sketch could check connection state
 * if extended.
 */
bool connectResult;

/**
 * @brief Initialize connection to MiP and print demo banner.
 *
 * @details Attempts to initialize the MiP connection via g_mip.begin().
 * If the connection fails, an error is printed to Serial1 and setup returns
 * early. On success, a short banner is printed to indicate the demo is running.
 */
void setup() {
  // First need to initialize the Serial1 connection with the MiP.
  connectResult = g_mip.begin();
  if (!connectResult) {
    Serial1.println(F("Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("Seattle Robotics Society Demo for April 21st, 2018."));
}

/**
 * @brief Main state machine loop.
 *
 * @details
 * Implements a compact state machine that:
 *   - Clears LEDs and disables gesture mode when restarting.
 *   - Waits for the robot to be upright before enabling gesture mode.
 *   - When upright, blinks the outer eyes and waits for left/right gestures.
 *   - On gesture detection, starts a 5-frame eye animation in the gesture
 * direction.
 *
 * The animation advances every 250 ms while PLAYING_EYE_ANIMATION is active.
 */
void loop() {
  if (!connectResult)
    return;  // If connecting to MiP failed in setup(), exit now.

  enum States {
    RESTART,
    WAITING_TO_STANDUP,
    WAITING_FOR_GESTURE,
    PLAYING_EYE_ANIMATION
  } static state = RESTART;

  static int8_t animationDirection =
    0; /**< +1 = leftward animation, -1 = rightward */
  static uint32_t keyframeStart =
    0;                           /**< Timestamp when current keyframe started */
  static uint8_t frameCount = 0; /**< Remaining frames in current animation */
  MiPGesture gesture;

  switch (state) {
    case RESTART:
      /* Turn all of the eye LEDs off and prepare to wait for the MiP to indicate
     * that it is standing. */
      g_mip.gesture.disable();
      g_headLEDs.clear();
      g_mip.headLEDs.write(g_headLEDs);
      state = WAITING_TO_STANDUP;
      break;

    case WAITING_TO_STANDUP:
      /* Waiting for the robot to indicate that it is standing upright and
     * balancing on its own. */
      if (g_mip.position.isUpright()) {
        /* Switch into gesture mode now that robot is up and balancing. */
        g_mip.gesture.enable();

        /* Blink the left and right most eye LEDs in a fast mode to indicate
       * readiness. */
        blinkOuterEyes();
        state = WAITING_FOR_GESTURE;
      }
      break;

    case WAITING_FOR_GESTURE:
      if (!g_mip.position.isUpright()) {
        /* Robot is no longer up and balancing so go back to restart. */
        state = RESTART;
        return;
      }

      /* Poll for a gesture event. readGestureEvent() returns the last gesture or
     * MIP_GESTURE_INVALID. */
      gesture = g_mip.gesture.readEvent();
      if (gesture == MIP_GESTURE_LEFT) {
        /* User moved hand from right to left; animate leftward. */
        animationDirection = 1;
      } else if (gesture == MIP_GESTURE_RIGHT) {
        /* User moved hand from left to right; animate rightward. */
        animationDirection = -1;
      } else {
        /* No relevant gesture detected; exit and check again on next loop. */
        return;
      }

      /* Start playing eye animation in the chosen direction. */
      g_headLEDs.clear();
      if (animationDirection > 0) {
        g_headLEDs.led1 = MIP_HEAD_LED_ON;
      } else {
        g_headLEDs.led4 = MIP_HEAD_LED_ON;
      }
      keyframeStart = millis();
      frameCount = 5; /* Play five keyframes */
      g_mip.headLEDs.write(g_headLEDs);
      state = PLAYING_EYE_ANIMATION;
      break;

    case PLAYING_EYE_ANIMATION:
      if (!g_mip.position.isUpright()) {
        /* Robot fell or is not balancing; restart the sequence. */
        state = RESTART;
        return;
      }

      /* Advance animation every 250 milliseconds. */
      if (millis() - keyframeStart >= 250) {
        keyframeStart = millis();
        animateEyes(animationDirection);
        frameCount--;
        if (frameCount == 0) {
          /* Animation finished; return to waiting for gestures and blink outer
         * eyes. */
          blinkOuterEyes();
          state = WAITING_FOR_GESTURE;
        }
      }
      break;
  }
}

/**
 * @brief Blink the outer eye LEDs in fast blink mode.
 *
 * @details Sets led1 and led4 to MIP_HEAD_LED_BLINK_FAST and turns off the
 * middle LEDs, then writes the state to the robot.
 */
static void blinkOuterEyes() {
  g_headLEDs.led1 = MIP_HEAD_LED_BLINK_FAST;
  g_headLEDs.led2 = MIP_HEAD_LED_OFF;
  g_headLEDs.led3 = MIP_HEAD_LED_OFF;
  g_headLEDs.led4 = MIP_HEAD_LED_BLINK_FAST;
  g_mip.headLEDs.write(g_headLEDs);
}

/**
 * @brief Advance the eye animation by one keyframe in the given direction.
 *
 * @param direction Positive values shift the lit LED left-to-right; negative
 *                  values shift right-to-left.
 *
 * @details
 * This function shifts the LED states across the four head LEDs to create a
 * simple traveling-light animation. After updating the local g_headLEDs state
 * it writes the new state to the robot.
 */
static void animateEyes(int8_t direction) {
  if (direction > 0) {
    /* Shift LEDs toward the right (led1 -> led2 -> led3 -> led4). */
    g_headLEDs.led4 = g_headLEDs.led3;
    g_headLEDs.led3 = g_headLEDs.led2;
    g_headLEDs.led2 = g_headLEDs.led1;
    g_headLEDs.led1 = MIP_HEAD_LED_OFF;
  } else {
    /* Shift LEDs toward the left (led4 -> led3 -> led2 -> led1). */
    g_headLEDs.led1 = g_headLEDs.led2;
    g_headLEDs.led2 = g_headLEDs.led3;
    g_headLEDs.led3 = g_headLEDs.led4;
    g_headLEDs.led4 = MIP_HEAD_LED_OFF;
  }
  g_mip.headLEDs.write(g_headLEDs);
}

