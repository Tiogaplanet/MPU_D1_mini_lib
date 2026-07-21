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
 * @file MPU_Infrared.cpp
 * @brief Implements MiP detection mode, IR remote control, and IR dongle
 *        code sending/receiving.
 *
 * Supports both MiP-to-MiP detection and IR dongle communication. Most
 * operations are verified where possible; send commands are fire-and-forget.
 */
#include "MPU_D1_mini.h"

// MiP Protocol Commands related to MiP's IR capability.
// These command codes are placed in the first byte of requests sent to the MiP
// and responses sent back from the MiP. See
// https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
// for the complete list.
#define MIP_CMD_SET_DETECTION_MODE 0x0E
#define MIP_CMD_SET_IR_REMOTE_CONTROL 0x10
#define MIP_CMD_GET_IR_REMOTE_CONTROL 0x11
#define MIP_CMD_SEND_IR_DONGLE_CODE 0x8C

// IR mode definitions.
#define MIP_IR_DETECTION_MODE_DISABLE 0
#define MIP_IR_REMOTE_CONTROL_DISABLE 0
#define MIP_IR_REMOTE_CONTROL_ENABLE 1

void MiP::enableMiPDetectionMode(uint8_t id, uint8_t txPower) {
  MIP_DEBUG_INFO_PRINTLN("MiP->Infrared->enableMiPDetectionMode()");
  m_irId = id;
  rawSetMiPDetectionMode(id, txPower);
}

void MiP::disableMiPDetectionMode() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Infrared->disableMiPDetectionMode()");
  m_irId = MIP_IR_DETECTION_MODE_DISABLE;

  // According to WowWee documentation, TX power must be between 1 and 120 even
  // when disabling.
  rawSetMiPDetectionMode(MIP_IR_DETECTION_MODE_DISABLE, 0x01);
}

bool MiP::isMiPDetectionModeEnabled() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Infrared->isMiPDetectionModeEnabled()");
  return m_irId > MIP_IR_DETECTION_MODE_DISABLE;
}

uint8_t MiP::readDetectedMiP() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Infrared->readDetectedMiP()");
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  processAllResponseData();
  uint8_t detectedMiPEvent = 0;
  if (!m_detectedMiPEvents.pop(detectedMiPEvent)) {
    m_lastError = MIP_ERROR_NO_EVENT;
    return detectedMiPEvent;
  }
  m_lastError = MIP_ERROR_NONE;
  return detectedMiPEvent;
}

uint8_t MiP::availableDetectedMiPEvents() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Infrared->availableDetectedMiPEvents()");
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  processAllResponseData();
  m_lastError = MIP_ERROR_NONE;
  return m_detectedMiPEvents.available();
}

void MiP::enableIRRemoteControl() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Infrared->enableIRRemoteControl()");
  verifiedIRRemoteControl(MIP_IR_REMOTE_CONTROL_ENABLE);
}
void MiP::disableIRRemoteControl() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Infrared->disableIRRemoteControl()");
  verifiedIRRemoteControl(MIP_IR_REMOTE_CONTROL_DISABLE);
}

bool MiP::isIRRemoteControlEnabled() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Infrared->isIRRemoteControlEnabled()");
  const uint8_t remoteControlEnabled[1] = {MIP_CMD_GET_IR_REMOTE_CONTROL};
  uint8_t response[1 + 1];
  size_t responseLength;
  int8_t result = rawReceive(remoteControlEnabled,
                             sizeof(remoteControlEnabled),
                             response,
                             sizeof(response),
                             responseLength);
  if (result != MIP_ERROR_NONE) {
    m_lastError = result;
    return false;
  }
  if (responseLength != sizeof(response) ||
      response[0] != MIP_CMD_GET_IR_REMOTE_CONTROL) {
    m_lastError = MIP_ERROR_BAD_RESPONSE;
    return false;
  }
  m_lastError = MIP_ERROR_NONE;
  return response[1] == MIP_IR_REMOTE_CONTROL_ENABLE;
}

void MiP::sendIRDongleCode(uint16_t sendCode, uint8_t transmitPower) {
  MIP_DEBUG_INFO_PRINTLN("MiP->Infrared->sendIRDongleCode()");
  uint8_t command[1 + 6] = {MIP_CMD_SEND_IR_DONGLE_CODE,
                            0x00,
                            0x00,
                            (uint8_t)((sendCode >> 8) & 0xFF),
                            (uint8_t)(sendCode & 0xFF),
                            0x10,
                            transmitPower};

  // Send this command blindly with no error checking since there is no robust
  // way to determine if it has failed.
  rawSend(command, sizeof(command));
}

uint32_t MiP::readIRDongleCode() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Infrared->readIRDongleCode()");
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  processAllResponseData();
  uint32_t irCodeEvent = 0xFFFFFFFF;
  if (!m_irCodeEvents.pop(irCodeEvent)) {
    m_lastError = MIP_ERROR_NO_EVENT;
    return irCodeEvent;
  }
  m_lastError = MIP_ERROR_NONE;
  return irCodeEvent;
}

uint8_t MiP::availableIRCodeEvents() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Infrared->availableIRCodeEvents()");
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  processAllResponseData();
  m_lastError = MIP_ERROR_NONE;
  return m_irCodeEvents.available();
}

// ==========================================================================
// Protected functions.
// ==========================================================================

// This internal protected method sends the set detection mode command with
// minimal error handling. The error recovery happens at a higher level of the
// driver.
void MiP::rawSetMiPDetectionMode(uint8_t id, uint8_t txPower) {
  MIP_ASSERT(0x01 <= txPower && txPower <= 0x78);
  uint8_t command[1 + 2] = {MIP_CMD_SET_DETECTION_MODE, id, txPower};
  rawSend(command, sizeof(command));
}

// This internal protected method verifies that IR remote control is enabled.
void MiP::verifiedIRRemoteControl(uint8_t desiredRemoteControlMode) {
  int8_t result;
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    rawSetIRRemoteControl(desiredRemoteControlMode);
    uint8_t actualMode;

    // Read back and make sure that it was set as expected.
    result = rawGetIRRemoteControl(actualMode);
    if (result == MIP_ERROR_NONE && actualMode == desiredRemoteControlMode) {
      // The set was successful so return immediately.
      m_lastError = MIP_ERROR_NONE;
      return;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }

  if (result != MIP_ERROR_NONE) {
    // Kept getting an error back from rawGetIRRemoteControl().
    m_lastError = result;
  } else {
    // rawGetIRRemoteControl() was successful but didn't match mode to which we
    // were attempting to change.
    m_lastError = MIP_ERROR_MAX_RETRIES;
  }
}

// This internal protected method sends the get IR remote control status command
// with minimal error handling. The error recovery happens at a higher level of
// the driver.
int8_t MiP::rawGetIRRemoteControl(uint8_t& remoteControl) {
  const uint8_t getIRRemoteControl[1] = {MIP_CMD_GET_IR_REMOTE_CONTROL};
  uint8_t response[1 + 1];
  size_t responseLength;
  int8_t result = rawReceive(getIRRemoteControl,
                             sizeof(getIRRemoteControl),
                             response,
                             sizeof(response),
                             responseLength);
  if (result != MIP_ERROR_NONE)
    return result;
  if (responseLength != sizeof(response) ||
      response[0] != MIP_CMD_GET_IR_REMOTE_CONTROL) {
    return MIP_ERROR_BAD_RESPONSE;
  }
  remoteControl = response[1];
  return result;
}

// This internal protected method sends the set IR remote control command with
// minimal error handling. The error recovery happens at a higher level of the
// driver.
void MiP::rawSetIRRemoteControl(uint8_t remoteControl) {
  MIP_ASSERT(remoteControl == MIP_IR_REMOTE_CONTROL_ENABLE ||
             remoteControl == MIP_IR_REMOTE_CONTROL_DISABLE);
  uint8_t command[1 + 1] = {MIP_CMD_SET_IR_REMOTE_CONTROL, remoteControl};
  rawSend(command, sizeof(command));
}
