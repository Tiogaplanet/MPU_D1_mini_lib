/**
 * @file SendIRDongleCode.ino
 * @brief Example sketch demonstrating sending IR dongle codes from a MiP.
 *
 * @details
 * This sketch initializes communication with a MiP and repeatedly transmits a
 * 16-bit IR dongle code using the sendIRDongleCode() API. The transmission
 * power can be adjusted via the MIP_IR_TX_POWER macro to experiment with
 * range and reliability. Each transmission is logged to Serial1 in a
 * human-readable hexadecimal format.
 *
 * The example exercises these API calls:
 *   - sendIRDongleCode()
 *
 * Usage notes:
 *   - Load this sketch on a MiP configured to transmit IR dongle codes.
 *   - Use another MiP or compatible IR receiver to observe or react to the
 *     transmitted codes.
 *
 * @copyright Copyright (C) 2018 Samuel Trassare (https://github.com/tiogaplanet)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <MPU_D1_mini.h>

/**
 * @brief IR transmit power used for dongle code transmissions.
 *
 * @details Valid values typically range from 0x01 (low) to 0x78 (maximum).
 * Adjust this macro to experiment with transmission range and reliability.
 */
#define MIP_IR_TX_POWER 0x78

/**
 * @brief Global MiP instance used to control the robot and send IR codes.
 *
 * @details Use this object to call MiP API functions such as begin() and
 * sendIRDongleCode().
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
 * - Initializes communication with the MiP via mip.begin().
 * - If the connection fails, prints an error to Serial1 and returns early.
 * - On success, prints a short description indicating the sketch is ready
 *   to send IR dongle codes.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("SendIRDongleCode.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("SendIRDongleCode.ino: Send code to another MiP using IR."));
}

/**
 * @brief Arduino loop function.
 *
 * @details
 * - Constructs a 16-bit dongle code (high byte followed by low byte).
 * - Prints the code to Serial1 in hexadecimal format for debugging.
 * - Calls sendIRDongleCode(dongleCode, MIP_IR_TX_POWER) to transmit the code.
 * - Waits one second between transmissions.
 *
 * Modify the dongleCode assignment to test different transmitted values.
 */
void loop() {
  if (!connectResult) return;  // If connecting to MiP failed in setup(), exit now.
  
  uint16_t dongleCode;
  char formattedOutput[14];

  // Compose a 16-bit dongle code from two bytes. Change these bytes to test.
  dongleCode = 0x45;
  dongleCode <<= 8;
  dongleCode |= 0x67;

  // Format and print the code being sent.
  sprintf(formattedOutput, " Sending 0x%04X", dongleCode);
  Serial1.println(formattedOutput);

  // Transmit the 16-bit dongle code using the configured IR transmit power.
  mip.sendIRDongleCode(dongleCode, MIP_IR_TX_POWER);

  // Pause between transmissions to avoid flooding the receiver.
  delay(1000);
}
