/**
 * @file SendDongleCode.ino
 * @brief Example sketch demonstrating sending 2-, 3-, and 4-byte IR dongle codes.
 *
 * @details
 * This sketch initializes communication with MiP and sequentially transmits
 * 2-byte, 3-byte, and 4-byte IR dongle codes using the updated sendDongleCode() API.
 * The transmission power is configured via the MIP_IR_TX_POWER constant.
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.infrared.sendDongleCode(code, length, power)
 *   - mip.infrared.sendDongleCode(irCodeStruct, power)
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
 * @brief IR transmit power used for dongle code transmissions (1-120).
 */
static constexpr uint8_t MIP_IR_TX_POWER = 0x78;

/**
 * @brief Global MiP instance used to communicate with MiP.
 *
 * @details Use this object to call MiP API functions throughout the sketch.
 */
MiP mip;

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Tracks the current test state in the loop transmission cycle.
 */
uint8_t cycleState = 0;

/**
 * @brief Arduino setup function.
 *
 * @details Initializes communication with MiP by calling mip.begin().
 * If the connection fails, an error message is printed to Serial1 and setup
 * returns early. On success, a description message is printed to Serial1.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("SendDongleCode.ino: Failed connecting to MiP."));
    return;
  }

  Serial1.println(
    F("SendDongleCode.ino: Send 2-, 3-, and 4-byte IR codes to another MiP."));
}

/**
 * @brief Arduino loop function.
 *
 * @details Sequentially transmits 2-byte, 3-byte, and 4-byte IR dongle codes
 * to another MiP every 2 seconds. Demonstrates both direct-value and struct-based
 * overloads of the sendDongleCode() API:
 *   - State 0: Transmits a 2-byte IR code (0x4567).
 *   - State 1: Transmits a 3-byte IR code (0x123456).
 *   - State 2: Transmits a 4-byte IR code (0xA1B2C3D4) via MiPIRDongleCode struct.
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) {
    return;
  }

  switch (cycleState) {
    case 0: {
      // --- Test 1: Send a 2-byte IR code (0x4567) ---
      uint16_t code2Byte = 0x4567;
      Serial1.print(F(" Sending 2-byte IR Code: 0x"));
      Serial1.println(code2Byte, HEX);

      mip.infrared.sendDongleCode(code2Byte, 2, MIP_IR_TX_POWER);
      delay(50);  // Allow UART TX to complete before console mux switch
      break;
    }

    case 1: {
      // --- Test 2: Send a 3-byte IR code (0x123456) ---
      uint32_t code3Byte = 0x123456;
      Serial1.print(F(" Sending 3-byte IR Code: 0x"));
      Serial1.println(code3Byte, HEX);

      mip.infrared.sendDongleCode(code3Byte, 3, MIP_IR_TX_POWER);
      delay(50);
      break;
    }

    case 2: {
      // --- Test 3: Send a 4-byte IR code using MiPIRDongleCode struct (0xA1B2C3D4) ---
      MiPIRDongleCode code4Byte(0xA1B2C3D4, 4);
      Serial1.print(F(" Sending 4-byte IR Code: 0x"));
      Serial1.print(code4Byte.code, HEX);
      Serial1.println(F(" (via struct)"));

      mip.infrared.sendDongleCode(code4Byte, MIP_IR_TX_POWER);
      delay(50);
      break;
    }
  }

  // Cycle to next test state
  cycleState = (cycleState + 1) % 3;

  // Pause between transmissions
  delay(2000);
}
