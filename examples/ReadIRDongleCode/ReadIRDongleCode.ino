/**
 * @file ReadIRDongleCode.ino
 * @brief Example sketch demonstrating receiving IR dongle codes from another MiP.
 *
 * @details
 * This sketch initializes communication with a MiP and continuously polls for
 * incoming IR dongle codes transmitted by another MiP. When an IR code event
 * is available, the sketch reads the 32-bit code using readIRDongleCode() and
 * prints the four constituent bytes in hexadecimal to Serial1 for inspection.
 *
 * The example exercises these API calls:
 *   - begin()
 *   - infrared.availableCodeEvents()
 *   - infrared.readDongleCode()
 *
 * Usage notes:
 *   - Load this sketch on a MiP that is configured to receive IR dongle codes.
 *   - Use another MiP (or IR dongle) to transmit codes for this sketch to read.
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
 * availableIRCodeEvents(), and readIRDongleCode().
 */
MiP mip;

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
 * - Attempts to initialize the MiP connection via mip.begin().
 * - If the connection fails, prints an error to Serial1 and returns early.
 * - On success, prints a short description to Serial1 indicating the sketch
 *   is ready to receive IR dongle codes.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("ReadIRDongleCode.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("ReadIRDongleCode.ino: Receive code from another MiP using IR."));
}

/**
 * @brief Arduino loop function.
 *
 * @details
 * - Polls for pending IR code events using infrared.availableCodeEvents().
 * - When an event is available, calls infrared.readDongleCode() to retrieve a 32-bit
 *   code value.
 * - Prints the code as four separate bytes in hexadecimal format to Serial1.
 *
 * The printed format breaks the 32-bit value into four bytes:
 *   - (receiveCode >> 28) & 0xFF
 *   - (receiveCode >> 16) & 0xFF
 *   - (receiveCode >> 8)  & 0xFF
 *   - receiveCode & 0xFF
 *
 * This layout matches the device's IR code packing and makes it easy to
 * visually compare transmitted codes.
 */
void loop() {
  if (!connectResult) return;  // If connecting to MiP failed in setup(), exit now.
	  
  uint32_t receiveCode;

  if (mip.infrared.availableCodeEvents()) {
    receiveCode = mip.infrared.readDongleCode();

    Serial1.print(F(" Received "));
    Serial1.print(((receiveCode >> 28) & 0xFF), HEX);
    Serial1.print(F(" "));
    Serial1.print(((receiveCode >> 16) & 0xFF), HEX);
    Serial1.print(F(" "));
    Serial1.print(((receiveCode >> 8) & 0xFF), HEX);
    Serial1.print(F(" "));
    Serial1.print((receiveCode & 0xFF), HEX);
    Serial1.println();
  }
}

