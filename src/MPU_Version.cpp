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
/* This file implements decoding and fetching for software and hardware version
   information.
*/
#include "MPU_D1_mini.h"

// MiP Protocol Commands to request hardware and software info.
// These command codes are placed in the first byte of requests sent to the MiP
// and responses sent back from the MiP. See
// https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
// for the complete list.
#define MIP_CMD_GET_SOFTWARE_VERSION 0x14
#define MIP_CMD_GET_HARDWARE_INFO 0x19

void MiP::readSoftwareVersion(MiPSoftwareVersion& software) {
  MIP_DEBUG_INFO_PRINTLN("MiP->Version->readSoftwareVersion()");
  int8_t result;

  // Retry the read if it should fail on the first attempt.
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    result = rawGetSoftwareVersion(software);
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

void MiP::readHardwareInfo(MiPHardwareInfo& hardware) {
  MIP_DEBUG_INFO_PRINTLN("MiP->Version->readHardwareInfo()");
  int8_t result;

  // Retry the read if it should fail on the first attempt.
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    result = rawGetHardwareInfo(hardware);
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

// This internal protected method sends the get software version command with
// minimal error handling. The error recovery happens at a higher level of the
// driver.
int8_t MiP::rawGetSoftwareVersion(MiPSoftwareVersion& software) {
  const uint8_t getSoftwareVersion[1] = {MIP_CMD_GET_SOFTWARE_VERSION};
  uint8_t response[1 + 4];
  size_t responseLength;
  int8_t result = rawReceive(getSoftwareVersion,
                             sizeof(getSoftwareVersion),
                             response,
                             sizeof(response),
                             responseLength);
  if (result)
    return result;
  if (responseLength != sizeof(response) ||
      response[0] != MIP_CMD_GET_SOFTWARE_VERSION) {
    return MIP_ERROR_BAD_RESPONSE;
  }
  software.year = 2000 + response[1];
  software.month = response[2];
  software.day = response[3];
  software.uniqueVersion = response[4];
  return result;
}

// This internal protected method sends the get hardware info command with
// minimal error handling. The error recovery happens at a higher level of the
// driver.
int8_t MiP::rawGetHardwareInfo(MiPHardwareInfo& hardware) {
  const uint8_t getHardwareInfo[1] = {MIP_CMD_GET_HARDWARE_INFO};
  uint8_t response[1 + 2];
  size_t responseLength;
  int8_t result = rawReceive(getHardwareInfo,
                             sizeof(getHardwareInfo),
                             response,
                             sizeof(response),
                             responseLength);
  if (result)
    return result;
  if (responseLength != sizeof(response) ||
      response[0] != MIP_CMD_GET_HARDWARE_INFO) {
    return MIP_ERROR_BAD_RESPONSE;
  }
  hardware.voiceChip = response[1];
  hardware.hardware = response[2];
  return result;
}
