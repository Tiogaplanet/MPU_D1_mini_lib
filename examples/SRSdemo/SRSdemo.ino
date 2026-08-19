/**
 * @file SRSdemo.ino
 * @brief Seattle Robotics Society demo showing gesture-driven eye animations.
 *
 * @details
 * This sketch was used as a live demonstration at the Seattle Robotics Society
 * meeting on April 21, 2018. It connects to MiP, waits for MiP
 * to stand upright, enables gesture mode, and then responds to left/right
 * hand-sweep gestures by playing a short eye LED animation in the direction
 * of the gesture. When idle, the outer eye LEDs blink rapidly to indicate MiP
 * is ready for input.
 *
 * The sketch implements a small state machine with the following states:
 *   - RESTART: clear LEDs and disable gesture mode while waiting for upright.
 *   - WAITING_TO_STANDUP: wait for MiP to report upright stance.
 *   - WAITING_FOR_GESTURE: blink outer eyes and poll for left/right gestures.
 *   - PLAYING_EYE_ANIMATION: play a 5-frame eye animation in the chosen
 *     direction.
 *
 * The example demonstrates use of these MiP APIs:
 *   - begin(), position.isUpright(), gesture.enable(), gesture.disable()
 *   - headLEDs.write()
 *
 * The helper functions blinkOuterEyes() and animateEyes() encapsulate the
 * LED keyframe updates used by the animation sequence.
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
 * position.isUpright(), gesture.enable(), gesture.disable(), and
 * headLEDs.write().
 */
static MiP g_mip;

/**
 * @brief Local copy of the head LED state used for animations.
 *
 * @details The MiPHeadLEDs struct stores the state for the four head LEDs
 * (led1..led4). The sketch updates this struct and writes it to MiP
 * with g_mip.headLEDs.write().
 */
static MiPHeadLEDs g_headLEDs;

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Blink the outer eye LEDs in fast blink mode.
 *
 * @details Sets led1 and led4 to MIP_HEAD_LED_BLINK_FAST and turns off the
 * middle LEDs, then writes the state to MiP. Placed above loop() for clean
 * prototype resolution across all compilers.
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
 * simple traveling-light animation. After updating the local g_headLEDs state,
 * it writes the new state to MiP. Placed above loop() for clean prototype
 * resolution across all compilers.
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

/**
 * @brief Initialize connection to MiP and print demo banner.
 *
 * @details Attempts to initialize the MiP connection via g_mip.begin().
 * If the connection fails, an error is printed to g_mip.console and setup returns
 * early. On success, a short banner is printed to indicate the demo is running.
 */
void setup() {
  // First need to initialize the g_mip.console connection with MiP.
  connectResult = g_mip.begin();
  if (!connectResult) {
    g_mip.console.println(F("SRSdemo.ino: Failed connecting to MiP!"));
    return;
  }

  g_mip.console.println(F("Seattle Robotics Society Demo for April 21st, 2018."));
}

/**
 * @brief Main state machine loop.
 *
 * @details
 * Implements a compact state machine that:
 *   - Clears LEDs and disables gesture mode when restarting.
 *   - Waits for MiP to be upright before enabling gesture mode.
 *   - When upright, blinks the outer eyes and waits for left/right gestures.
 *   - On gesture detection, starts a 5-frame eye animation in the gesture
 *     direction.
 *
 * The animation advances every 250 ms while PLAYING_EYE_ANIMATION is active.
 */
void loop() {
  // If connecting to MiP failed in setup(), exit now.
  if (!connectResult) { return; }

  enum States {
    RESTART,
    WAITING_TO_STANDUP,
    WAITING_FOR_GESTURE,
    PLAYING_EYE_ANIMATION
  };
  static States state = RESTART;

  static int8_t animationDirection = 0; /**< +1 = leftward animation, -1 =
                                           rightward */
  static uint32_t keyframeStart = 0;    /**< Timestamp when current keyframe
                                           started */
  static uint8_t frameCount = 0; /**< Remaining frames in current animation */
  MiPGesture gesture;

  switch (state) {
    case RESTART:
      /* Turn all of the eye LEDs off and prepare to wait for MiP to indicate
       * standing upright. */
      g_mip.gesture.disable();
      g_headLEDs.clear();
      g_mip.headLEDs.write(g_headLEDs);
      state = WAITING_TO_STANDUP;
      break;

    case WAITING_TO_STANDUP:
      /* Waiting for MiP to indicate standing upright and balancing. */
      if (g_mip.position.isUpright()) {
        /* Switch into gesture mode now that MiP is up and balancing. */
        g_mip.gesture.enable();

        /* Blink the left and right most eye LEDs in fast mode to indicate
         * readiness. */
        blinkOuterEyes();
        state = WAITING_FOR_GESTURE;
      } else {
        delay(50);  // Yield CPU time while waiting to stand
      }
      break;

    case WAITING_FOR_GESTURE:
      if (!g_mip.position.isUpright()) {
        /* MiP is no longer up and balancing so go back to restart. */
        state = RESTART;
        return;
      }

      /* Poll for a gesture event. gesture.readEvent() returns the last gesture
       * or MIP_GESTURE_INVALID. */
      gesture = g_mip.gesture.readEvent();
      if (gesture == MIP_GESTURE_LEFT) {
        /* User moved hand from right to left; animate leftward. */
        animationDirection = 1;
      } else if (gesture == MIP_GESTURE_RIGHT) {
        /* User moved hand from left to right; animate rightward. */
        animationDirection = -1;
      } else {
        /* No relevant gesture detected; yield briefly and check again on next
         * loop. */
        delay(10);
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
        /* MiP fell or is not balancing; restart the sequence. */
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
      } else {
        delay(10);  // Yield CPU time between animation frames
      }
      break;
  }
}
