/**
 * @file ReadWriteEEPROM.ino
 * @brief Example sketch demonstrating reading and writing MiP's user EEPROM.
 *
 * @details This sketch demonstrates how to read from and write to MiP's
 * non-volatile user EEPROM memory. MiP provides 16 user-addressable EEPROM
 * bytes at address offsets 0 through 15 (physical addresses 0x20 to 0x2F).
 *
 * The sketch performs the following sequence in setup():
 *   - Initializes communication with MiP.
 *   - Reads and displays the current contents of all 16 user EEPROM bytes.
 *   - Increments a persistent boot/execution counter stored at offset 0.
 *   - Writes the new counter value to offset 0 using eeprom.write().
 *   - Reads back offset 0 using eeprom.read() to verify the write succeeded.
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.eeprom.read(addressOffset)
 *   - mip.eeprom.write(addressOffset, userData)
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
 * @brief Global MiP instance used to communicate with MiP.
 *
 * @details Use this object to call MiP API functions such as begin(),
 * eeprom.read(), and eeprom.write().
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
 * If the connection fails, an error message is printed to Serial1 and setup
 * returns early. On success, the function:
 *   - Iterates through offsets 0 to 15, reading and printing the current EEPROM
 *     contents.
 *   - Reads offset 0, increments its value by 1, and writes it back to EEPROM.
 *   - Re-reads offset 0 to verify the updated value persisted.
 */
void setup() {
  connectResult = mip.begin();

  if (!connectResult) {
    Serial1.println(F("ReadWriteEEPROM.ino: Failed connecting to MiP."));
    return;
  }

  Serial1.println(
      F("ReadWriteEEPROM.ino: Read and write MiP's 16-byte user EEPROM."));

  // 1. Read and display the current contents of all 16 user EEPROM offsets (0-15)
  Serial1.println(F("\n Current User EEPROM Contents (Offsets 0 to 15):"));
  for (uint8_t offset = 0; offset < 16; offset++) {
    uint8_t val = mip.eeprom.read(offset);

    Serial1.print(F("   Offset "));
    if (offset < 10) Serial1.print(F(" "));  // Align single-digit offsets
    Serial1.print(offset);
    Serial1.print(F(": 0x"));
    if (val < 0x10) Serial1.print(F("0"));    // Leading zero padding for hex
    Serial1.print(val, HEX);
    Serial1.print(F(" ("));
    Serial1.print(val);
    Serial1.println(F(")"));
  }

  // 2. Read the current value at offset 0, increment it, and write it back
  Serial1.println(F("\n Incrementing persistent counter at offset 0..."));
  uint8_t currentCounter = mip.eeprom.read(0);
  uint8_t newCounter = currentCounter + 1;

  Serial1.print(F(" Writing value "));
  Serial1.print(newCounter);
  Serial1.println(F(" to EEPROM offset 0..."));

  mip.eeprom.write(0, newCounter);

  // 3. Read back offset 0 to verify the write succeeded
  uint8_t readbackValue = mip.eeprom.read(0);
  Serial1.print(F(" Readback value from offset 0: "));
  Serial1.println(readbackValue);

  if (readbackValue == newCounter) {
    Serial1.println(F(" Verification PASS: EEPROM write succeeded!"));
  } else {
    Serial1.println(F(" Verification FAIL: EEPROM readback mismatch!"));
  }

  Serial1.println();
  Serial1.println(F("ReadWriteEEPROM.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs all actions in setup() and does not require
 * repeated work in loop().
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) {
    return;
  }
}
