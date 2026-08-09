/**
 * @file SendDongleCode.ino
 * @brief Example sketch demonstrating sending 2-, 3-, and 4-byte IR dongle codes.
 *
 * @details
 * This sketch initializes communication with MiP and sequentially transmits
 * 2-byte, 3-byte, and 4-byte IR dongle codes using the updated sendDongleCode() API.
 * The transmission power is configured via the MIP_IR_TX_POWER macro.
 *
 * The example exercises these API calls:
 *   - infrared.sendDongleCode(code, length, power)
 *   - infrared.sendDongleCode(irCodeStruct, power)
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
#define MIP_IR_TX_POWER 0x78

MiP mip;
bool connectResult;
uint8_t cycleState = 0;

void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial.println(F("SendDongleCode.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(
    F("SendDongleCode.ino: Send 2-, 3-, and 4-byte IR codes to another MiP."));
}

void loop() {
  if (!connectResult) return;

  char formattedOutput[64];

  switch (cycleState) {
    case 0:
      {
        // --- Test 1: Send a 2-byte IR code (0x4567) ---
        uint16_t code2Byte = 0x4567;
        snprintf(formattedOutput, sizeof(formattedOutput),
                 " Sending 2-byte IR Code: 0x%04X", code2Byte);
        Serial1.println(formattedOutput);

        mip.infrared.sendDongleCode(code2Byte, 2, MIP_IR_TX_POWER);
        delay(50);  // Allow UART TX to complete before console mux switch
        break;
      }

    case 1:
      {
        // --- Test 2: Send a 3-byte IR code (0x123456) ---
        uint32_t code3Byte = 0x123456;
        snprintf(formattedOutput, sizeof(formattedOutput),
                 " Sending 3-byte IR Code: 0x%06lX", (unsigned long)code3Byte);
        Serial1.println(formattedOutput);

        mip.infrared.sendDongleCode(code3Byte, 3, MIP_IR_TX_POWER);
        delay(50);
        break;
      }

    case 2:
      {
        // --- Test 3: Send a 4-byte IR code using MiPIRDongleCode struct (0xA1B2C3D4) ---
        MiPIRDongleCode code4Byte(0xA1B2C3D4, 4);
        snprintf(formattedOutput, sizeof(formattedOutput),
                 " Sending 4-byte IR Code: 0x%08lX (via struct)", (unsigned long)code4Byte.code);
        Serial1.println(formattedOutput);

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
