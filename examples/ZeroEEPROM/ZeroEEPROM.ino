/**
 * @file ZeroEEPROM.ino
 * @brief Example sketch that writes zeros to each byte of the MiP user EEPROM.
 *
 * @details
 * This sketch demonstrates how to iterate over the MiP's user EEPROM address
 * range and write a zero value to each byte using the eeprom.write() API.
 * After writing each byte the sketch reads it back with eeprom.read() and
 * prints the address and recovered value to Serial1 for verification.
 *
 * The example exercises these API calls:
 *   - eeprom.write()
 *   - eeprom.read()
 *
 * Usage notes:
 *   - Running this sketch will overwrite the MiP user EEPROM contents with
 *     zeros. Use with caution if the EEPROM contains important data.
 *   - The sketch pauses one second between writes so the user can observe
 *     progress on Serial1 and the device.
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
 * eeprom.write(), and eeprom.read().
 */
MiP mip;

/**
 * @brief Temporary storage for a single EEPROM byte read back from the device.
 *
 * @details The variable is used to hold the value returned by getUserData()
 * for display. It is declared here for clarity; the sketch writes zeros and
 * then reads each address back into this variable.
 */
uint8_t eepromContents;

/**
 * @brief Arduino setup function.
 *
 * @details
 * - Initializes the MiP connection via mip.begin(). If the connection fails,
 *   prints an error to Serial1 and returns early.
 * - Iterates over the MiP user EEPROM address range from 0x00 up to
 *   (MiP_EEPROM::LAST_EEPROM_ADDRESS - MiP_EEPROM::BASE_EEPROM_ADDRESS)
 * inclusive and:
 *     1. Writes a zero to each EEPROM offset using eeprom.write(offset, 0x00).
 *     2. Waits one second to allow observation and avoid flooding the device.
 *     3. Reads the byte back with eeprom.read(offset) and prints the address
 *        and recovered value in hexadecimal to Serial1 for verification.
 *
 * Note:
 *   - This operation will irreversibly overwrite any existing user EEPROM
 *     data stored on the MiP. Back up any important data before running.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("ZeroEEPROM.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("ZeroEEPROM.ino: Writes zeros to each byte in EEPROM."));

  // Iterate over the valid user EEPROM offsets and write zeros.
  for (uint8_t i = 0x00;
       i <= MiP_EEPROM::LAST_EEPROM_ADDRESS - MiP_EEPROM::BASE_EEPROM_ADDRESS;
       i++) {
    // Write a zero to EEPROM at offset i.
    mip.eeprom.write(i, 0x00);

    // Delay so the user can observe progress and to avoid rapid-fire writes.
    delay(1000);

    // Read back the value we just wrote for verification.
    eepromContents = mip.eeprom.read(i);

    // Print the EEPROM offset and the recovered value in hex.
    Serial1.print(F(" 0x2"));
    Serial1.print(i, HEX);
    Serial1.print(F(": "));
    Serial1.print(F("0x0"));
    Serial1.println(eepromContents, HEX);
  }

  Serial1.print(F("ZeroEEPROM.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs its work in setup() and does not require
 * repeated actions in loop(). The loop is intentionally left empty so the
 * sketch completes once and remains idle.
 */
void loop() {}

