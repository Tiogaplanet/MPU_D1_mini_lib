/* Copyright (C) 2026  Samuel Trassare (https://github.com/Tiogaplanet)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/**
 * @file MPU_HeadLEDs.cpp
 * @brief Implements control and reading of the MiP robot's head LEDs and
 *        provides verified/unverified write methods for the chest LED (via
 *        shared infrastructure).
 *
 * Head LEDs support 4 independent positions with multiple patterns (off, on,
 * blink slow/fast). Verified methods include read-back confirmation with
 * automatic retries.
 */
#include "MPU_D1_mini.h"

// MiP Protocol Commands related to the head LEDs.
// These command codes are placed in the first byte of requests sent to the MiP
// and responses sent back from the MiP. See
// https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
// for the complete list.
#define MIP_CMD_SET_HEAD_LEDS 0x8A
#define MIP_CMD_GET_HEAD_LEDS 0x8B

void MiP::readHeadLEDs(MiPHeadLEDs& headLEDs) {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->readHeadLEDs()");
  int8_t result;

  // Retry the read if it should fail on the first attempt.
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    result = rawGetHeadLEDs(headLEDs);
    if (result == MIP_ERROR_NONE) {
      m_lastError = MIP_ERROR_NONE;
      return;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }
  m_lastError = result;
}

void MiP::writeHeadLEDs(MiPHeadLED led1,
                        MiPHeadLED led2,
                        MiPHeadLED led3,
                        MiPHeadLED led4) {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->writeHeadLEDs()");
  int8_t result;

  // Send the set command and then issue the corresponding get command. Retry if
  // the get fails or doesn't return the expected new setting.
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    rawSetHeadLEDs(led1, led2, led3, led4);

    // Read back and make sure that it was set as expected.
    MiPHeadLEDs headLEDs;
    result = rawGetHeadLEDs(headLEDs);
    if (result == MIP_ERROR_NONE && headLEDs.led1 == led1 &&
        headLEDs.led2 == led2 && headLEDs.led3 == led3 &&
        headLEDs.led4 == led4) {
      // The set was successful so return immediately.
      m_lastError = MIP_ERROR_NONE;
      return;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }

  if (result != MIP_ERROR_NONE) {
    // Kept getting an error back from read attempt.
    m_lastError = result;
  } else {
    // Read was successful but didn't match setting to which we were attempting
    // to change.
    m_lastError = MIP_ERROR_MAX_RETRIES;
  }
}

void MiP::writeHeadLEDs(const MiPHeadLEDs& headLEDs) {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->writeHeadLEDs()");
  writeHeadLEDs(headLEDs.led1, headLEDs.led2, headLEDs.led3, headLEDs.led4);
}

void MiP::unverifiedWriteHeadLEDs(MiPHeadLED led1,
                                  MiPHeadLED led2,
                                  MiPHeadLED led3,
                                  MiPHeadLED led4) {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->unverifiedWriteHeadLEDs()");
  rawSetHeadLEDs(led1, led2, led3, led4);
}

void MiP::unverifiedWriteHeadLEDs(const MiPHeadLEDs& headLEDs) {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->unverifiedWriteHeadLEDs()");
  unverifiedWriteHeadLEDs(
      headLEDs.led1, headLEDs.led2, headLEDs.led3, headLEDs.led4);
}

// ==========================================================================
// Protected functions.
// ==========================================================================

// This internal protected method sends the get head LEDs command with minimal
// error handling. The error recovery happens at a higher level of the driver.
int8_t MiP::rawGetHeadLEDs(MiPHeadLEDs& headLEDs) {
  const uint8_t getHeadLEDs[1] = {MIP_CMD_GET_HEAD_LEDS};
  uint8_t response[1 + 4];
  size_t responseLength;
  int result = rawReceive(getHeadLEDs,
                          sizeof(getHeadLEDs),
                          response,
                          sizeof(response),
                          responseLength);
  if (result)
    return result;
  if (responseLength != sizeof(response) ||
      response[0] != (uint8_t)MIP_CMD_GET_HEAD_LEDS ||
      !isValidHeadLED(response[1]) || !isValidHeadLED(response[2]) ||
      !isValidHeadLED(response[3]) || !isValidHeadLED(response[4])) {
    return MIP_ERROR_BAD_RESPONSE;
  }
  headLEDs.led1 = (MiPHeadLED)response[1];
  headLEDs.led2 = (MiPHeadLED)response[2];
  headLEDs.led3 = (MiPHeadLED)response[3];
  headLEDs.led4 = (MiPHeadLED)response[4];
  return MIP_ERROR_NONE;
}

// This internal protected method sends the set head LEDs command with no error
// checking. The error handling / recovery happens at a higher level of the
// driver.
void MiP::rawSetHeadLEDs(MiPHeadLED led1,
                         MiPHeadLED led2,
                         MiPHeadLED led3,
                         MiPHeadLED led4) {
  uint8_t command[1 + 4] = {MIP_CMD_SET_HEAD_LEDS, led1, led2, led3, led4};
  rawSend(command, sizeof(command));
}

// This internal protected method is called to validate that each head LED value
// returned is within the expected range.
bool MiP::isValidHeadLED(uint8_t led) {
  return led <= MIP_HEAD_LED_BLINK_FAST;
}
