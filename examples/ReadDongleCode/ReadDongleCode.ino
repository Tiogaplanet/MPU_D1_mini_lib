/**
 * @file ReadDongleCode.ino
 * @brief Example sketch demonstrating receiving variable-length IR dongle codes.
 *
 * @details
 * This sketch continuously polls for incoming IR dongle codes transmitted by another MiP.
 * When an IR code event is available, it retrieves a MiPIRDongleCode struct,
 * inspects the length field (2, 3, or 4 bytes), and prints the constituent bytes in
 * hexadecimal to Serial1.
 *
 * The example exercises these API calls:
 *   - begin()
 *   - infrared.availableCodeEvents()
 *   - infrared.readDongleCode()
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

MiP mip;

bool connectResult;

void setup() {
  connectResult = mip.begin();

  if (!connectResult) {
    Serial.println(F("ReadDongleCode.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(
    F("ReadDongleCode.ino: Receiving 2-, 3-, and 4-byte IR codes from another MiP."));
}

void loop() {
  if (!connectResult) return;

  if (mip.infrared.availableCodeEvents()) {
    // Read the next IR dongle code struct (contains code value and byte length)
    MiPIRDongleCode irEvent = mip.infrared.readDongleCode();

    if (irEvent.isValid()) {
      Serial1.print(F(" Received "));
      Serial1.print(irEvent.length);
      Serial1.print(F("-byte IR Code: 0x"));

      // Print hex bytes corresponding to exact received byte length
      for (int8_t i = irEvent.length - 1; i >= 0; i--) {
        uint8_t byteVal = (irEvent.code >> (i * 8)) & 0xFF;
        if (byteVal < 0x10) {
          Serial1.print(F("0"));  // Leading zero padding
        }
        Serial1.print(byteVal, HEX);
        if (i > 0) {
          Serial1.print(F(" "));
        }
      }
      Serial1.println();
    }
  }
}
