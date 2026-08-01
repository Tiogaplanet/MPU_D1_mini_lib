/**
 * @file Weight.ino
 * @brief Example sketch that reads and reports the MiP weight sensor.

 *
 * @details
 * This sketch demonstrates how to query the MiP weight sensor and print
 * changes to Serial1. It repeatedly reads the device weight using
 * weight.read() and prints the value only when it changes to avoid spamming
 * the serial output. This is useful for monitoring payload changes on MiP's
 * tray or detecting when the robot is picked up or placed down.
 *
 * Demonstrated API:
 *   - weight.read()
 *
 * Usage notes:
 *   - Ensure the MiP is powered and connected before running this sketch.
 *   - MiP must be standing upright, not propped on the kickstand.
 *   - Open Serial1 to observe printed weight updates.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MPU_D1_mini.h>

/**
 * @brief Global MiP instance used to communicate with the robot.
 *
 * @details Use this object to call MiP API functions such as begin() and
 * weight.read(). Keeping the instance at file scope makes it available in
 * both setup() and loop().
 */
MiP mip;

/**
 * @brief Last reported weight value.
 *
 * @details Initialized to an out-of-range sentinel so the first reading is
 * always treated as a change and printed. The MiP weight API returns an
 * int8_t value; this variable stores the last printed value to suppress
 * duplicate prints.
 */
static int8_t lastWeight = -128;

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
 * @details
 * - Initializes the MiP connection via mip.begin().
 * - If the connection fails, prints an error to Serial1 and returns early.
 * - On success, prints a short banner indicating the sketch will display
 *   weight updates.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("Weight.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(
    F("Weight.ino: Read MiP's weight with different objects on the tray."));
}

/**
 * @brief Arduino loop function.
 *
 * @details
 * - Polls the MiP weight sensor using readWeight().
 * - If the current weight differs from the last reported value, prints the
 *   new weight to Serial1 and updates lastWeight.
 *
 * The loop is intentionally lightweight and prints only on changes to avoid
 * flooding the serial output with repeated identical values.
 */
void loop() {
  if (!connectResult)
    return;  // If connecting to MiP failed in setup(), exit now.

  int8_t currentWeight = mip.weight.read();

  if (currentWeight != lastWeight) {
    Serial1.print(F(" Weight = "));
    Serial1.println(currentWeight);
    lastWeight = currentWeight;
  }
}

