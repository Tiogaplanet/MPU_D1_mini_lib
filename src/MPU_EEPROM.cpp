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
 * @file MPU_EEPROM.cpp
 * @brief Implements reading and writing user data to the MiP robot's EEPROM.
 *
 * Provides verified (with read-back confirmation and retry) access to the
 * 16-byte user data area (addresses 0x20 to 0x2F).
 */
#include "MPU_D1_mini.h"

// MiP Protocol Commands related MiP's EEPROM.
// These command codes are placed in the first byte of requests sent to the MiP
// and responses sent back from the MiP. See
// https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
// for the complete list.
#define MIP_CMD_SET_USER_DATA 0x12
#define MIP_CMD_GET_USER_DATA 0x13

// Define an assert mechanism that can be used to log and halt when the user is
// found to be calling the API incorrectly.
#define MIP_ASSERT(EXPRESSION) \
  if (!(EXPRESSION))           \
    mipAssert(__LINE__);

static void mipAssert(uint32_t lineNumber) {
  MIP_DEBUG_ERROR_PRINTF("MiP: Assert: MPU_EEPROM.cpp: %d\n", lineNumber);
  while (1) {
    delay(100);
  }
}

void MiP::setUserData(uint8_t addressOffset, uint8_t userData) {
  MIP_DEBUG_INFO_PRINTLN("MiP->EEPROM->setUserData()");
  uint8_t address = BASE_EEPROM_ADDRESS + addressOffset;

  // Address must be between 0x20 and 0x2F, inclusive.
  MIP_ASSERT(BASE_EEPROM_ADDRESS <= address && address <= LAST_EEPROM_ADDRESS);

  int8_t result;

  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    rawSetUserData(address, userData);

    // Read back and make sure that it was set as expected.
    byte storedData = 0x00;
    result = rawGetUserData(address, storedData);
    if (result == MIP_ERROR_NONE && storedData == userData) {
      // The set was successful so return immediately.
      m_lastError = MIP_ERROR_NONE;
      return;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }

  if (result != MIP_ERROR_NONE) {
    // Kept getting an error back from rawGetUserData().
    m_lastError = result;
  } else {
    // rawGetUserData() was successful but didn't match the data we were
    // expecting.
    m_lastError = MIP_ERROR_MAX_RETRIES;
  }
}

uint8_t MiP::getUserData(uint8_t addressOffset) {
  MIP_DEBUG_INFO_PRINTLN("MiP->EEPROM->getUserData()");
  uint8_t address = BASE_EEPROM_ADDRESS + addressOffset;

  // Address must be between 0x20 and 0x2F, inclusive.
  MIP_ASSERT(BASE_EEPROM_ADDRESS <= address && address <= LAST_EEPROM_ADDRESS);

  int8_t result;

  // Retry the read if it should fail on the first attempt.
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    uint8_t storedData;
    result = rawGetUserData(address, storedData);
    if (result == MIP_ERROR_NONE) {
      m_lastError = MIP_ERROR_NONE;
      return storedData;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }
  m_lastError = result;
  return 0;
}

// This internal protected method sends the set user data command with no error
// checking. The error handling and recovery happens at a higher level of the
// driver.
void MiP::rawSetUserData(uint8_t address, uint8_t userData) {
  uint8_t command[1 + 2] = {MIP_CMD_SET_USER_DATA, address, userData};
  rawSend(command, sizeof(command));
}

// This internal protected method sends the get user data command with minimal
// error handling. The error and recovery happens at a higher level of the
// driver.
int8_t MiP::rawGetUserData(uint8_t address, uint8_t& userData) {
  uint8_t getUserData[1 + 1] = {MIP_CMD_GET_USER_DATA, address};
  uint8_t response[1 + 2];
  size_t responseLength;
  int8_t result = rawReceive(getUserData,
                             sizeof(getUserData),
                             response,
                             sizeof(response),
                             responseLength);
  if (result)
    return result;
  if (responseLength != 3 || response[0] != MIP_CMD_GET_USER_DATA ||
      response[1] != address) {
    return MIP_ERROR_BAD_RESPONSE;
  }
  userData = (uint8_t)response[2];
  return MIP_ERROR_NONE;
}
