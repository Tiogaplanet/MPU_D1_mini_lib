/**
 * @file RawSendReceive.ino
 * @brief Example sketch demonstrating MiP raw send/receive operations.
 *
 * @details This sketch shows how to use the MiP library's serial.rawSend() and
 * serial.rawReceive() APIs to transmit and receive low-level MiP command
 * packets. The example sends a 4-byte command to set the chest LED to purple
 * and then requests the MiP firmware revision using a raw receive command. If a
 * valid firmware response is returned, the sketch prints a human-readable
 * software version string to Serial1.
 *
 * The example exercises these API calls:
 *   - serial.rawSend()
 *   - serial.rawReceive()
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
 * @details Use this object to call MiP API functions such as begin(),
 * serial.rawSend(), and serial.rawReceive().
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details Initializes communication with the MiP robot by calling mip.begin().
 * If the connection fails, an error message is printed to Serial1 and setup
 * returns early. On success, the sketch:
 *   - Sends a 4-byte raw command to set the chest LED to purple.
 *   - Sends a raw request to query the MiP firmware version and attempts to
 *     parse and display the returned version information.
 *
 * The rawReceive() call demonstrates how to provide a receive buffer and
 * examine the returned bytes for a known response format.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("RawSendReceive.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("RawSendReceive.ino: Use raw*() functions. Should set "
                    "chest LED to purple and display MiP firmware revision."));

  /* Send 4-byte MiP command to set Chest LED to Purple.
   * The command bytes are device-specific; here we send the raw packet
   * directly using rawSend(). sizeof(setChestPurple) - 1 excludes the
   * terminating NUL from the string literal.
   */
  uint8_t setChestPurple[] = "\x84\xFF\x01\xFF";
  mip.serial.rawSend(setChestPurple, sizeof(setChestPurple) - 1);

  /* Request the MiP firmware revision information and display it.
   * Prepare a small receive buffer and call serial.rawReceive() with the
   * request packet. On success, validate the response length and expected
   * command byte before printing a formatted version string.
   */
  uint8_t getMiPSoftwareVersion[] = "\x14";
  size_t responseLength = 0;
  uint8_t response[5];
  int result = mip.serial.rawReceive(
    getMiPSoftwareVersion, sizeof(getMiPSoftwareVersion) - 1, response,
    sizeof(response), responseLength);

  /* Check for a successful rawReceive and expected response format:
   *   response[0] == 0x14 indicates a firmware version reply.
   *   response[1..3] contain year, month, day offsets as used by this firmware.
   *   response[4] contains the build number.
   */
  if (result == MiP::MIP_ERROR_NONE && responseLength == 5 && response[0] == 0x14) {
    Serial1.print(F(" MiP Software Version: "));
    Serial1.print(response[1] + 2000);  // Year offset stored as (year - 2000)
    Serial1.print('-');
    Serial1.print(response[2]);  // Month
    Serial1.print('-');
    Serial1.print(response[3]);  // Day
    Serial1.print(F(" (build #"));
    Serial1.print(response[4]);  // Build number
    Serial1.print(')');
  }

  Serial1.println();
  Serial1.println(F("RawSendReceive.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs all actions in setup() and does not require
 * repeated work in loop(). The function is intentionally left empty so the
 * demonstration runs once during initialization and then remains idle.
 */
void loop() {}

