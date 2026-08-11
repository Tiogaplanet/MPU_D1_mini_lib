/**
 * @file ZeroEEPROM.ino
 * @brief Example sketch that writes zeros to each byte of MiP's user EEPROM.
 *
 * @details
 * This sketch demonstrates how to iterate over MiP's user EEPROM address
 * range and write a zero value to each byte using the eeprom.write() API.
 * After writing each byte, the sketch reads it back with eeprom.read() and
 * prints the address and recovered value to Serial1 for verification.
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.eeprom.write()
 *   - mip.eeprom.read()
 *
 * Usage notes:
 *   - Running this sketch will overwrite MiP's user EEPROM contents with
 *     zeros. Use with caution if the EEPROM contains important data.
 *   - The sketch pauses one second between writes so the user can observe
 *     progress on Serial1.
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
 * eeprom.write(), and eeprom.read().
 */
MiP mip;

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Temporary storage for a single EEPROM byte read back from MiP.
 *
 * @details Used to hold the value returned by eeprom.read() for display.
 * Declared at file scope for clarity; the sketch writes zeros and then reads
 * each address back into this variable.
 */
uint8_t eepromContents;

/**
 * @brief Arduino setup function.
 *
 * @details
 * - Initializes the MiP connection via mip.begin(). If the connection fails,
 *   prints an error to Serial1 and returns early.
 * - Iterates over MiP's user EEPROM address range from offset 0x00 up to
 *   (MiP_EEPROM::LAST_EEPROM_ADDRESS - MiP_EEPROM::BASE_EEPROM_ADDRESS)
 *   inclusive (offsets 0 to 15) and:
 *     1. Writes a zero to each EEPROM offset using eeprom.write(offset, 0x00).
 *     2. Waits one second to allow observation and avoid flooding UART.
 *     3. Reads the byte back with eeprom.read(offset) and prints the physical
 *        address and recovered value in hexadecimal to Serial1 for verification.
 *
 * Note:
 *   - This operation will irreversibly overwrite any existing user EEPROM
 *     data stored on MiP. Back up any important data before running.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("ZeroEEPROM.ino: Failed connecting to MiP."));
    return;
  }

  Serial1.println(F("ZeroEEPROM.ino: Writes zeros to each byte in EEPROM."));

  // Calculate total number of user EEPROM offsets (0 to 15)
  const uint8_t maxOffset =
      MiP_EEPROM::LAST_EEPROM_ADDRESS - MiP_EEPROM::BASE_EEPROM_ADDRESS;

  // Iterate over the valid user EEPROM offsets and write zeros
  for (uint8_t offset = 0; offset <= maxOffset; offset++) {
    // Write a zero to EEPROM at offset i
    mip.eeprom.write(offset, 0x00);

    // Delay so the user can observe progress and to avoid rapid-fire writes
    delay(1000);

    // Read back the value we just wrote for verification
    eepromContents = mip.eeprom.read(offset);

    // Calculate actual physical EEPROM memory address (0x20 to 0x2F)
    uint8_t physicalAddress = MiP_EEPROM::BASE_EEPROM_ADDRESS + offset;

    // Print the physical EEPROM address and the recovered value in hex
    Serial1.print(F(" 0x"));
    Serial1.print(physicalAddress, HEX);
    Serial1.print(F(": 0x"));
    if (eepromContents < 0x10) {
      Serial1.print(F("0"));  // Leading zero padding for single hex digits
    }
    Serial1.println(eepromContents, HEX);
  }

  Serial1.println();
  Serial1.println(F("ZeroEEPROM.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs its work in setup() and does not require
 * repeated actions in loop().
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) {
    return;
  }
}
