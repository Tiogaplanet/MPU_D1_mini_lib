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
 * @file MPU_Clap.cpp
 * @brief This file implements clap event enabling, delay configuration,
 *        and fetching for the WowWee MiP robot.
 *
 * It provides both verified (with read-back) and raw low-level methods
 * for reliable clap detection control.
*/
#include "MPU_D1_mini.h"

// MiP Protocol Commands related to clap detection.
// These command codes are placed in the first byte of requests sent to the MiP
// and responses sent back from the MiP. See
// https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
// for the complete list.
#define MIP_CMD_ENABLE_CLAP 0x1E
#define MIP_CMD_SET_CLAP_DELAY 0x20
#define MIP_CMD_GET_CLAP_SETTINGS 0x1F

void MiP::enableClapEvents() {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->enableClapEvents()");
  checkedEnableClapEvents(MIP_CLAP_ENABLED);
}
void MiP::disableClapEvents() {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->disableClapEvents()");
  checkedEnableClapEvents(MIP_CLAP_DISABLED);
}

// This internal protected method attempts to enable/disable clap events and
// then reads back the clap settings to see if the new value has taken. Retries
// on errors or mismatches.
void MiP::checkedEnableClapEvents(MiPClapEnabled enabled) {
  int8_t result;
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    rawEnableClap(enabled);

    // Read back and make sure that it was set as expected.
    MiPClapSettings setting;
    result = rawGetClapSettings(setting);
    if (result == MIP_ERROR_NONE && setting.enabled == enabled) {
      // The set was successful so return immediately.
      m_lastError = MIP_ERROR_NONE;
      return;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }

  if (result != MIP_ERROR_NONE) {
    // Kept getting an error back from read back routine.
    m_lastError = result;
  } else {
    // Read back was successful but write didn't take.
    m_lastError = MIP_ERROR_MAX_RETRIES;
  }
}

void MiP::writeClapDelay(uint16_t delayTime) {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->writeClapDelay()");
  int8_t result;

  // Send the set command and then issue the corresponding get command. Retry if
  // the get fails or doesn't return the expected new setting.
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    rawSetClapDelay(delayTime);

    // Read back and make sure that it was set as expected.
    MiPClapSettings setting;
    result = rawGetClapSettings(setting);
    if (result == MIP_ERROR_NONE && setting.delay == delayTime) {
      // The set was successful so return immediately.
      m_lastError = MIP_ERROR_NONE;
      return;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }

  if (result != MIP_ERROR_NONE) {
    // Kept getting an error back from read back routine.
    m_lastError = result;
  } else {
    // Read back was successful but write didn't take.
    m_lastError = MIP_ERROR_MAX_RETRIES;
  }
}

// This internal protected method sends the enable/disable clap command with no
// error checking. The error handling / recovery happens at a higher level of
// the driver.
void MiP::rawEnableClap(MiPClapEnabled enabled) {
  uint8_t command[1 + 1] = {MIP_CMD_ENABLE_CLAP, enabled};
  rawSend(command, sizeof(command));
}

// This internal protected method sends the set clap delay command with no error
// checking. The error handling / recovery happens at a higher level of the
// driver.
void MiP::rawSetClapDelay(uint16_t delay) {
  uint8_t command[1 + 2] = {
      MIP_CMD_SET_CLAP_DELAY, (uint8_t)(delay >> 8), (uint8_t)(delay & 0xFF)};
  rawSend(command, sizeof(command));
}

bool MiP::areClapEventsEnabled() {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->areClapEventsEnabled()");
  MiPClapSettings settings;
  int8_t result = readClapSettings(settings);
  if (result != MIP_ERROR_NONE) {
    m_lastError = result;
    return false;
  }
  m_lastError = MIP_ERROR_NONE;
  return settings.enabled == MIP_CLAP_ENABLED;
}

uint16_t MiP::readClapDelay() {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->readClapDelay()");
  MiPClapSettings settings;
  int8_t result = readClapSettings(settings);
  if (result != MIP_ERROR_NONE) {
    m_lastError = result;
    return 0;
  }
  m_lastError = MIP_ERROR_NONE;
  return settings.delay;
}

// This internal protected method issues the low level get clap settings command
// and retries if an error is encountered.
int8_t MiP::readClapSettings(MiPClapSettings& settings) {
  int8_t result;

  // Retry the read if it should fail on the first attempt.
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    result = rawGetClapSettings(settings);
    if (result == MIP_ERROR_NONE)
      return MIP_ERROR_NONE;

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }
  settings.clear();
  return result;
}

uint8_t MiP::availableClapEvents() {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->availableClapEvents()");
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  processAllResponseData();

  m_lastError = MIP_ERROR_NONE;
  return m_clapEvents.available();
}

uint8_t MiP::readClapEvent() {
  MIP_DEBUG_INFO_PRINTLN("MiP->HeadLEDs->readClapEvent()");
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  processAllResponseData();

  uint8_t clapEvent = 0;
  if (!m_clapEvents.pop(clapEvent)) {
    // No clap event has been received yet.
    m_lastError = MIP_ERROR_NO_EVENT;
    return 0;
  }
  m_lastError = MIP_ERROR_NONE;
  return clapEvent;
}

// This internal protected method sends the get clap settings command with
// minimal error handling. The error recovery happens at a higher level of the
// driver.
int8_t MiP::rawGetClapSettings(MiPClapSettings& settings) {
  const uint8_t getClapSettings[1] = {MIP_CMD_GET_CLAP_SETTINGS};
  uint8_t response[1 + 3];
  size_t responseLength;
  int8_t result = rawReceive(getClapSettings,
                             sizeof(getClapSettings),
                             response,
                             sizeof(response),
                             responseLength);

  if (result)
    return result;

  if (responseLength != sizeof(response) ||
      response[0] != MIP_CMD_GET_CLAP_SETTINGS ||
      (response[1] != MIP_CLAP_DISABLED && response[1] != MIP_CLAP_ENABLED)) {
    return MIP_ERROR_BAD_RESPONSE;
  }

  settings.enabled = (MiPClapEnabled)response[1];
  settings.delay = (uint16_t)response[2] << 8 | response[3];
  return MIP_ERROR_NONE;
}
