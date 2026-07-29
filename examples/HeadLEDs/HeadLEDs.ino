/**
 * @file HeadLEDs.ino
 * @brief Example sketch demonstrating MiP head LED control and queries.
 *
 * @details This sketch shows how to use the MiP library to set and read the
 * robot's head (eye) LEDs using both verified and unverified APIs. It:
 *   - Sets each head LED to a different state using headLEDs.write().
 *   - Reads back the current head LED states with headLEDs.read() and prints
 *     a human-readable description for each LED.
 *   - Restores all head LEDs to the ON state.
 *   - Repeats the sequence using headLEDs.unverifiedWrite() to demonstrate
 *     the unverified API which may not always be accepted by the robot.
 *
 * The example exercises these API calls:
 *   - headLEDs.write()
 *   - headLEDs.read()
 *   - headLEDs.unverifiedWrite()
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
 * headLEDs.write(), headLEDs.read(), and headLEDs.unverifiedWrite().
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details Initializes communication with the MiP robot by calling mip.begin().
 * If the connection fails, an error message is printed to Serial1 and setup
 * returns early. On success, the sketch:
 *   - Sets the four head LEDs to different states (OFF, ON, BLINK_SLOW,
 * BLINK_FAST).
 *   - Reads the current head LED states into a MiPHeadLEDs struct and prints
 *     each LED's state using printLEDString().
 *   - Restores all head LEDs to ON and demonstrates the unverified write API
 *     by attempting the same sequence with headLEDs.unverifiedWrite().
 *
 * The function prints progress and results to Serial1 for observation.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("HeadLEDs.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("HeadLEDs.ino: Use head LED functions. Should set each "
                    "head LED to different state."));
  mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON, MIP_HEAD_LED_BLINK_SLOW,
                     MIP_HEAD_LED_BLINK_FAST);

  MiPHeadLEDs headLEDs;
  mip.headLEDs.read(headLEDs);
  Serial1.println(F("Head LEDs"));
  Serial1.print(F("    led1: "));
  printLEDString(headLEDs.led1);
  Serial1.print(F("    led2: "));
  printLEDString(headLEDs.led2);
  Serial1.print(F("    led3: "));
  printLEDString(headLEDs.led3);
  Serial1.print(F("    led4: "));
  printLEDString(headLEDs.led4);

  delay(4000);

  // Turn all the LEDs back on now.
  Serial1.println(F(" Turning all eye LEDs back on now."));
  headLEDs.led1 = headLEDs.led2 = headLEDs.led3 = headLEDs.led4 =
    MIP_HEAD_LED_ON;
  mip.headLEDs.write(headLEDs);
  delay(1000);

  // Attempt to run through the same sequence of head LED changes using the
  // headLEDs.unverifiedWrite() functions which don't always get accepted by
  // MiP.
  Serial1.println(F(" Trying to set each head LED to a different state."));
  mip.headLEDs.unverifiedWrite(MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON,
                               MIP_HEAD_LED_BLINK_SLOW,
                               MIP_HEAD_LED_BLINK_FAST);
  delay(4000);

  Serial1.println(F(" Trying to set all eye LEDs back on now."));
  headLEDs.led1 = headLEDs.led2 = headLEDs.led3 = headLEDs.led4 =
    MIP_HEAD_LED_ON;
  mip.headLEDs.unverifiedWrite(headLEDs);

  Serial1.println();
  Serial1.println(F("HeadLEDs.ino: Done."));
}

/**
 * @brief Print a human-readable description of a head LED state.
 *
 * @details Helper function that maps a MiPHeadLED enum value to a readable
 * string and prints it to Serial1. Handles the defined LED states:
 *   - MIP_HEAD_LED_OFF
 *   - MIP_HEAD_LED_ON
 *   - MIP_HEAD_LED_BLINK_SLOW
 *   - MIP_HEAD_LED_BLINK_FAST
 *
 * The function is declared static to limit its linkage to this file.
 *
 * @param led The MiPHeadLED enum value to describe and print.
 */
static void printLEDString(MiPHeadLED led) {
  switch (led) {
    case MIP_HEAD_LED_OFF:
      Serial1.println(F(" Off"));
      break;
    case MIP_HEAD_LED_ON:
      Serial1.println(F(" On"));
      break;
    case MIP_HEAD_LED_BLINK_SLOW:
      Serial1.println(F(" Blink Slow"));
      break;
    case MIP_HEAD_LED_BLINK_FAST:
      Serial1.println(F(" Blink Fast"));
      break;
    default:
      Serial1.println();
      break;
  }
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs all actions in setup() and does not require
 * repeated work in loop(). The function is intentionally left empty so the
 * demonstration runs only once during initialization.
 */
void loop() {}
