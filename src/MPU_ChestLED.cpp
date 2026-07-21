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
 * @file MPU_ChestLED.cpp
 * @brief Implements solid color and flashing control for the MiP chest LED.
 *
 * Provides both verified (with read-back confirmation) and unverified
 * (fire-and-forget) methods.
 */
#include "MPU_D1_mini.h"

// MiP Protocol Commands related to the chest LED.
// These command codes are placed in the first byte of requests sent to the MiP
// and responses sent back from the MiP. See
// https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
// for the complete list.
#define MIP_CMD_GET_CHEST_LED 0x83
#define MIP_CMD_SET_CHEST_LED 0x84
#define MIP_CMD_FLASH_CHEST_LED 0x89

void MiP::readChestLED(MiPChestLED& chestLED) {
  MIP_DEBUG_INFO_PRINTLN("MiP->ChestLED->readChestLED()");
  int8_t result;

  // Retry the read if it should fail on the first attempt.
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    result = rawGetChestLED(chestLED);
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

void MiP::writeChestLED(uint8_t red, uint8_t green, uint8_t blue) {
  MIP_DEBUG_INFO_PRINTLN("MiP->ChestLED->writeChestLED()");
  int8_t result;

  // The blue channel is actually only 6-bit and not a full 8-bit so zero out
  // lower 2 bits (the MiP does this too).
  blue &= ~3;

  // Send the set command and then issue the corresponding get command. Retry if
  // the get fails or doesn't return the expected new setting.
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    rawSetChestLED(red, green, blue);

    // Read back and make sure that it was set as expected.
    MiPChestLED actualChestLED;
    result = rawGetChestLED(actualChestLED);
    if (result == MIP_ERROR_NONE && actualChestLED.red == red &&
        actualChestLED.green == green && actualChestLED.blue == blue) {
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

void MiP::writeChestLED(uint8_t red,
                        uint8_t green,
                        uint8_t blue,
                        uint16_t onTime,
                        uint16_t offTime) {
  MIP_DEBUG_INFO_PRINTLN("MiP->ChestLED->writeChestLED()");
  int8_t result;

  // on/off time are in units of 20 msecs.
  MIP_ASSERT(onTime / 20 <= 255 && offTime / 20 <= 255);
  onTime = (onTime + 10) / 20;
  offTime = (offTime + 10) / 20;

  // The blue channel is actually only 6-bit and not a full 8-bit so zero out
  // lower 2 bits (the MiP does this too).
  blue &= ~3;

  // Send the set command and then issue the corresponding get command. Retry if
  // the get fails or doesn't return the expected new setting.
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    rawFlashChestLED(red, green, blue, onTime, offTime);

    // Read back and make sure that it was set as expected.
    MiPChestLED actualChestLED;
    result = rawGetChestLED(actualChestLED);
    if (result == MIP_ERROR_NONE && actualChestLED.red == red &&
        actualChestLED.green == green && actualChestLED.blue == blue &&
        actualChestLED.onTime == onTime && actualChestLED.offTime == offTime) {
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

void MiP::writeChestLED(const MiPChestLED& chestLED) {
  MIP_DEBUG_INFO_PRINTLN("MiP->ChestLED->writeChestLED()");
  writeChestLED(chestLED.red,
                chestLED.green,
                chestLED.blue,
                chestLED.onTime,
                chestLED.offTime);
}

void MiP::unverifiedWriteChestLED(uint8_t red, uint8_t green, uint8_t blue) {
  MIP_DEBUG_INFO_PRINTLN("MiP->ChestLED->unverifiedWriteChestLED()");
  rawSetChestLED(red, green, blue);
}

void MiP::unverifiedWriteChestLED(uint8_t red,
                                  uint8_t green,
                                  uint8_t blue,
                                  uint16_t onTime,
                                  uint16_t offTime) {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->unverifiedWriteChestLED()");
  // on/off time are in units of 20 msecs.
  MIP_ASSERT(onTime / 20 <= 255 && offTime / 20 <= 255);
  onTime = (onTime + 10) / 20;
  offTime = (offTime + 10) / 20;
  rawFlashChestLED(red, green, blue, onTime, offTime);
}

void MiP::unverifiedWriteChestLED(const MiPChestLED& chestLED) {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->unverifiedWriteChestLED()");
  unverifiedWriteChestLED(chestLED.red,
                          chestLED.green,
                          chestLED.blue,
                          chestLED.onTime,
                          chestLED.offTime);
}

// ==========================================================================
// Protected functions.
// ==========================================================================

// This internal protected method sends the get chest LED command with minimal
// error handling. The error recovery happens at a higher level of the driver.
int8_t MiP::rawGetChestLED(MiPChestLED& chestLED) {
  const uint8_t getChestLED[1] = {MIP_CMD_GET_CHEST_LED};
  uint8_t response[1 + 5];
  size_t responseLength;
  uint8_t result = rawReceive(getChestLED,
                              sizeof(getChestLED),
                              response,
                              sizeof(response),
                              responseLength);
  if (result)
    return result;
  if (responseLength != sizeof(response) ||
      response[0] != MIP_CMD_GET_CHEST_LED) {
    return MIP_ERROR_BAD_RESPONSE;
  }
  chestLED.red = response[1];
  chestLED.green = response[2];
  chestLED.blue = response[3];

  // on/off time are in units of 20 msecs.
  chestLED.onTime = (uint16_t)response[4] * 20;
  chestLED.offTime = (uint16_t)response[5] * 20;
  return MIP_ERROR_NONE;
}

// This internal protected method sends the set chest LED command with no error
// checking. The error handling / recovery happens at a higher level of the
// driver.
void MiP::rawSetChestLED(uint8_t red, uint8_t green, uint8_t blue) {
  uint8_t command[1 + 3] = {MIP_CMD_SET_CHEST_LED, red, green, blue};
  rawSend(command, sizeof(command));
}

// This internal protected method sends the flash chest LED command with no
// error checking. The error handling / recovery happens at a higher level of
// the driver.
void MiP::rawFlashChestLED(uint8_t red,
                           uint8_t green,
                           uint8_t blue,
                           uint8_t onTime,
                           uint8_t offTime) {
  uint8_t command[1 + 5] = {
      MIP_CMD_FLASH_CHEST_LED, red, green, blue, onTime, offTime};
  rawSend(command, sizeof(command));
}
