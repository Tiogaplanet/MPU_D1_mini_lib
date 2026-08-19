/**
 * @file RawSendReceive.ino
 * @brief Example sketch demonstrating MiP's raw send and receive operations.
 *
 * @details This sketch shows how to use the MiP library's serial.rawSend() and
 * serial.rawReceive() APIs to transmit and receive low-level MiP command
 * packets. The example sends a 4-byte command to set the chest LED to purple
 * and then requests MiP's firmware revision using a raw receive command. If a
 * valid firmware response is returned, the sketch prints a human-readable
 * software version string to mip.console.
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.serial.rawSend()
 *   - mip.serial.rawReceive()
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MiP_Power_Up.h>

/**
 * @brief Global MiP instance used to communicate with MiP.
 *
 * @details Use this object to call MiP API functions such as begin(),
 * serial.rawSend(), and serial.rawReceive().
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
 * If the connection fails, an error message is printed to mip.console and setup
 * returns early. On success, the sketch:
 *   - Sends a 4-byte raw command to set the chest LED to purple.
 *   - Sends a raw request to query MiP's firmware version and attempts to
 *     parse and display the returned version information.
 *
 * The rawReceive() call demonstrates how to provide a receive buffer and
 * examine the returned bytes for a known response format.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    mip.console.println(F("RawSendReceive.ino: Failed connecting to MiP!"));
    return;
  }

  mip.console.println(F("RawSendReceive.ino: Use raw*() functions. Should set "
                    "chest LED to purple and display MiP firmware revision."));

  /* Send 4-byte MiP command to set Chest LED to Purple.
   * Command structure: [0x84 (SET_CHEST_LED), Red (0xFF), Green (0x01), Blue
   * (0xFF)]
   */
  uint8_t setChestPurple[] = { 0x84, 0xFF, 0x01, 0xFF };
  mip.serial.rawSend(setChestPurple, sizeof(setChestPurple));

  /* Request MiP's firmware revision information and display it.
   * Command structure: [0x14 (GET_SOFTWARE_VERSION)]
   * Prepare a small receive buffer and call serial.rawReceive() with the
   * request packet. On success, validate the response length and expected
   * command byte before printing a formatted version string.
   */
  uint8_t getMiPSoftwareVersion[] = { 0x14 };
  size_t responseLength = 0;
  uint8_t response[5] = { 0 };
  int result = mip.serial.rawReceive(
    getMiPSoftwareVersion, sizeof(getMiPSoftwareVersion), response,
    sizeof(response), responseLength);

  /* Check for a successful rawReceive and expected response format:
   *   response[0] == 0x14 indicates a firmware version reply.
   *   response[1..3] contain year, month, day offsets as used by this firmware.
   *   response[4] contains the build number.
   */
  if (result == MiP::MIP_ERROR_NONE && responseLength == 5 && response[0] == 0x14) {
    mip.console.print(F(" MiP Software Version: "));
    mip.console.print(response[1] + 2000);  // Year offset stored as (year - 2000)
    mip.console.print('-');
    if (response[2] < 10) mip.console.print('0');  // Month zero-padding
    mip.console.print(response[2]);                // Month
    mip.console.print('-');
    if (response[3] < 10) mip.console.print('0');  // Day zero-padding
    mip.console.print(response[3]);                // Day
    mip.console.print(F(" (build #"));
    mip.console.print(response[4]);  // Build number
    mip.console.print(')');
  }

  mip.console.println();
  mip.console.println(F("RawSendReceive.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs all actions in setup() and does not require
 * repeated work in loop().
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) { return; }
}
