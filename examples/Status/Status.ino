/**
 * @file Status.ino
 * @brief Example sketch that monitors and reports MiP status changes.
 *
 * @details
 * This sketch demonstrates how to query and report various status values from
 * MiP using the MiP library. It connects to MiP, then continuously polls for
 * changes to battery voltage and physical stance/position. When a change is
 * detected, the new value is printed to Serial1. The sketch serves as a clean
 * diagnostic example showing status-related API calls.
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.battery.readVoltage()
 *   - mip.position.read()
 *   - mip.position.isOnBack()
 *   - mip.position.isFaceDown()
 *   - mip.position.isUpright()
 *   - mip.position.isPickedUp()
 *   - mip.position.isHandStanding()
 *   - mip.position.isFaceDownOnTray()
 *   - mip.position.isOnBackWithKickstand()
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
#include <math.h>

/**
 * @brief Global MiP instance used to communicate with MiP.
 *
 * @details Use this object to call MiP status APIs such as
 * battery.readVoltage() and position.read().
 */
MiP mip;

/**
 * @brief Last reported battery voltage (volts).
 *
 * @details Stored so the sketch only prints battery updates when the value
 * changes significantly.
 */
static float lastBatteryLevel = 0.0f;

/**
 * @brief Last reported position enum value.
 *
 * @details Initialized to an invalid value so the first position.read() call
 * will always be treated as a change and printed.
 */
static MiPPosition lastPosition = static_cast<MiPPosition>(-1);

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Arduino setup function.
 *
 * @details
 * - Initializes the MiP connection via mip.begin().
 * - If the connection fails, prints an error to Serial1 and returns early.
 * - On success, prints a short banner indicating the sketch will display
 *   status changes.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("Status.ino: Failed connecting to MiP."));
    return;
  }

  Serial1.println(F("Status.ino: Display MiP status as it changes."));
}

/**
 * @brief Arduino loop function.
 *
 * @details
 * - Polls MiP for current battery voltage and stance position.
 * - If the battery voltage differs significantly (>= 0.05V) from the last
 *   reported reading, prints the new voltage and updates lastBatteryLevel.
 * - If the position differs from the last reported position, queries the
 *   various position predicates (isOnBack(), isFaceDown(), isUpright(), etc.)
 *   and prints matching position descriptions. Updates lastPosition to avoid
 *   repeated prints for the same state.
 *
 * The loop prints only on changes to avoid spamming the serial output and
 * yields briefly to keep background CPU tasks responsive.
 */
void loop() {
  if (!connectResult) {
    return;  // Exit immediately if connecting to MiP failed during setup()
  }

  float currentBatteryLevel = mip.battery.readVoltage();
  MiPPosition currentPosition = mip.position.read();

  /* Report battery voltage when it changes meaningfully (by at least 0.05V). */
  if (fabs(currentBatteryLevel - lastBatteryLevel) >= 0.05f) {
    Serial1.print(F(" Battery: "));
    Serial1.print(currentBatteryLevel, 2);
    Serial1.println(F("V"));
    lastBatteryLevel = currentBatteryLevel;
  }

  /* Report position changes by evaluating all position predicates. */
  if (currentPosition != lastPosition) {
    if (mip.position.isOnBack()) {
      Serial1.println(F(" Position: On Back"));
    }
    if (mip.position.isFaceDown()) {
      Serial1.println(F(" Position: Face Down"));
    }
    if (mip.position.isUpright()) {
      Serial1.println(F(" Position: Upright"));
    }
    if (mip.position.isPickedUp()) {
      Serial1.println(F(" Position: Picked Up"));
    }
    if (mip.position.isHandStanding()) {
      Serial1.println(F(" Position: Hand Stand"));
    }
    if (mip.position.isFaceDownOnTray()) {
      Serial1.println(F(" Position: Face Down on Tray"));
    }
    if (mip.position.isOnBackWithKickstand()) {
      Serial1.println(F(" Position: On Back With Kickstand"));
    }

    lastPosition = currentPosition;
  }

  // Yield control briefly to prevent watchdog reset triggers
  delay(50);
}
