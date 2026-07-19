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
 * @file MPU_Odometer.cpp
 * @brief Implements reading and resetting the MiP robot's odometer (distance
 *        travelled).
 *
 * The odometer is read with verification and retry logic. Reset is
 * fire-and-forget as there is no reliable confirmation mechanism.
 */
#include "MPU_D1_mini.h"

// MiP Protocol Commands related to the odometer.
// These command codes are placed in the first byte of requests sent to the MiP
// and responses sent back from the MiP. See
// https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
// for the complete list.
#define MIP_CMD_READ_ODOMETER 0x85
#define MIP_CMD_RESET_ODOMETER 0x86

float MiP::readDistanceTravelled() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Odometer->readDistanceTravelled()");
  int8_t result;

  // Retry the read if it should fail on the first attempt.
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    float distance;
    result = rawReadOdometer(distance);
    if (result == MIP_ERROR_NONE) {
      m_lastError = MIP_ERROR_NONE;
      return distance;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }
  m_lastError = result;
  return 0.0f;
}

void MiP::resetDistanceTravelled() {
  uint8_t command[1] = {MIP_CMD_RESET_ODOMETER};

  // Send this command blindly with no error checking since there is no robust
  // way to determine if it has failed.
  rawSend(command, sizeof(command));
}

// This internal protected method sends the read odometer command with minimal
// error handling. The error recovery happens at a higher level of the driver.
int8_t MiP::rawReadOdometer(float& distanceInCm) {
  const uint8_t readOdometer[1] = {MIP_CMD_READ_ODOMETER};
  uint8_t response[1 + 4];
  size_t responseLength;
  int result = rawReceive(readOdometer,
                          sizeof(readOdometer),
                          response,
                          sizeof(response),
                          responseLength);
  if (result)
    return result;
  if (responseLength != sizeof(response) ||
      response[0] != MIP_CMD_READ_ODOMETER) {
    return MIP_ERROR_BAD_RESPONSE;
  }

  // Tick count is stored as big-endian in response buffer.
  uint32_t ticks = (uint32_t)response[1] << 24 | (uint32_t)response[2] << 16 |
                   (uint32_t)response[3] << 8 | response[4];

  // Odometer has 48.5 ticks / cm.
  distanceInCm = (float)((double)ticks / 48.5);
  return result;
}