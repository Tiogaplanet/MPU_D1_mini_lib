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
/* This file implements decoding and fetching for the physical weight sensor.
 */
#include "MPU_D1_mini.h"

// MiP Protocol Command to request weight.
// These command codes are placed in the first byte of requests sent to the MiP
// and responses sent back from the MiP. See
// https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
// for the complete list.
#define MIP_CMD_GET_WEIGHT 0x81

int8_t MiP::readWeight() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Weight->readWeight()");
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  processAllResponseData();
  if ((m_flags & MIP_FLAG_WEIGHT_VALID)) {
    // Have a cached weight event already, so just return it.
    m_lastError = MIP_ERROR_NONE;
    return m_lastWeight;
  }

  // Haven't seen a weight event yet so request the weight explicitly.
  // Retry the read if it should fail on the first attempt.
  int8_t result;
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    int8_t weight;
    result = rawGetWeight(weight);
    if (result == MIP_ERROR_NONE) {  // Error codes defined in MPU_D1_mini.h.
      // Cache the returned value and return it to the caller.
      m_lastError = MIP_ERROR_NONE;
      m_lastWeight = weight;
      m_flags |=
          MIP_FLAG_WEIGHT_VALID;  // From the enum FlagBits in MPU_D1_mini.h.
      return weight;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }
  m_lastError = result;
  return 0;
}

// This internal protected method sends the get weight command with minimal
// error handling. The error recovery happens at a higher level of the driver.
int8_t MiP::rawGetWeight(int8_t& weight) {
  const uint8_t getWeight[1] = {MIP_CMD_GET_WEIGHT};
  uint8_t response[1 + 1];
  size_t responseLength;
  int8_t result = rawReceive(
      getWeight, sizeof(getWeight), response, sizeof(response), responseLength);
  if (result)
    return result;
  return parseWeight(weight, response, responseLength);
}

// This internal protected method takes the weight response and validates it.
int8_t MiP::parseWeight(int8_t& weight,
                        const uint8_t response[],
                        size_t responseLength) {
  if (responseLength != 2 || response[0] != MIP_CMD_GET_WEIGHT) {
    return MIP_ERROR_BAD_RESPONSE;  // Error codes defined in MPU_D1_mini.h.
  }
  weight = response[1];
  return MIP_ERROR_NONE;  // Error codes defined in MPU_D1_mini.h.
}
