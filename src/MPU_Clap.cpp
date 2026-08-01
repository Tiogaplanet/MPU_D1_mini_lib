/**
 * @file MPU_Clap.cpp
 * @brief Implements clap event handling for the MiP library.
 *
 * @details This source file implements clap event parsing and queue management.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include "MPU_Clap.h"
#include "MiP_Power_Up_-_D1_mini.h"

// Implement the constructor to store the MiP reference.
MiP_Clap::MiP_Clap(MiP& mip) : m_mip(mip) {
  clear();
}

void MiP_Clap::enableEvents() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Clap->enableClapEvents()");
  checkedEnableEvents(MIP_CLAP_ENABLED);
}
void MiP_Clap::disableEvents() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Clap->disableEvents()");
  checkedEnableEvents(MIP_CLAP_DISABLED);
}

bool MiP_Clap::areEventsEnabled() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Clap->areEventsEnabled()");
  MiPClapSettings settings;
  int8_t result = readSettings(settings);
  if (result != MiP::MIP_ERROR_NONE) {
    m_mip.m_lastError = result;
    return false;
  }
  m_mip.m_lastError = MiP::MIP_ERROR_NONE;
  return settings.enabled == MIP_CLAP_ENABLED;
}

uint8_t MiP_Clap::availableEvents() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Clap->availableEvents()");
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  m_mip.serial.processAllResponseData();

  m_mip.m_lastError = MiP::MIP_ERROR_NONE;
  return m_clapEvents.available();
}

uint8_t MiP_Clap::readEvent() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Clap->readEvent()");
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  m_mip.serial.processAllResponseData();

  uint8_t clapEvent = 0;
  if (!m_clapEvents.pop(clapEvent)) {
    // No clap event has been received yet.
    m_mip.m_lastError = MiP::MIP_ERROR_NO_EVENT;
    return 0;
  }
  m_mip.m_lastError = MiP::MIP_ERROR_NONE;
  return clapEvent;
}

void MiP_Clap::processEvent(uint8_t clapCode) {
  m_clapEvents.push(clapCode);
}

uint16_t MiP_Clap::readDelay() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Clap->readDelay()");
  MiPClapSettings settings;
  int8_t result = readSettings(settings);
  if (result != MiP::MIP_ERROR_NONE) {
    m_mip.m_lastError = result;
    return 0;
  }
  m_mip.m_lastError = MiP::MIP_ERROR_NONE;
  return settings.delay;
}

void MiP_Clap::writeDelay(uint16_t delayTime) {
  MIP_DEBUG_INFO_PRINTLN("MiP->Clap->writeDelay()");
  int8_t result;

  // Send the set command and then issue the corresponding get command. Retry if
  // the get fails or doesn't return the expected new setting.
  for (uint8_t retry = 0; retry < MiP_Serial::MIP_MAX_RETRIES; retry++) {
    rawSetDelay(delayTime);

    // Read back and make sure that it was set as expected.
    MiPClapSettings settings;
    result = rawGetSettings(settings);
    if (result == MiP::MIP_ERROR_NONE && settings.delay == delayTime) {
      // The set was successful so return immediately.
      m_mip.m_lastError = MiP::MIP_ERROR_NONE;
      return;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MiP_Serial::MIP_RETRY_WAIT);
  }

  if (result != MiP::MIP_ERROR_NONE) {
    // Kept getting an error back from read back routine.
    m_mip.m_lastError = result;
  } else {
    // Read back was successful but write didn't take.
    m_mip.m_lastError = MiP::MIP_ERROR_MAX_RETRIES;
  }
}

void MiP_Clap::clear() {
  m_clapEvents.clear();
}

// ==========================================================================
// Protected functions.
// ==========================================================================

// This internal protected method attempts to enable/disable clap events and
// then reads back the clap settings to see if the new value has taken.
// Retries on errors or mismatches.
void MiP_Clap::checkedEnableEvents(MiPClapEnabled enabled) {
  int8_t result;
  for (uint8_t retry = 0; retry < MiP_Serial::MIP_MAX_RETRIES; retry++) {
    rawEnable(enabled);

    // Read back and make sure that it was set as expected.
    MiPClapSettings settings;
    result = rawGetSettings(settings);
    if (result == MiP::MIP_ERROR_NONE && settings.enabled == enabled) {
      // The set was successful so return immediately.
      m_mip.m_lastError = MiP::MIP_ERROR_NONE;
      return;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MiP_Serial::MIP_RETRY_WAIT);
  }

  if (result != MiP::MIP_ERROR_NONE) {
    // Kept getting an error back from read back routine.
    m_mip.m_lastError = result;
  } else {
    // Read back was successful but write didn't take.
    m_mip.m_lastError = MiP::MIP_ERROR_MAX_RETRIES;
  }
}

// This internal protected method issues the low level get clap settings command
// and retries if an error is encountered.
int8_t MiP_Clap::readSettings(MiPClapSettings& settings) {
  int8_t result;

  // Retry the read if it should fail on the first attempt.
  for (uint8_t retry = 0; retry < MiP_Serial::MIP_MAX_RETRIES; retry++) {
    result = rawGetSettings(settings);
    if (result == MiP::MIP_ERROR_NONE)
      return MiP::MIP_ERROR_NONE;

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MiP_Serial::MIP_RETRY_WAIT);
  }
  settings.clear();
  return result;
}

// This internal protected method sends the enable/disable clap command with no
// error checking. The error handling / recovery happens at a higher level of
// the driver.
void MiP_Clap::rawEnable(MiPClapEnabled enabled) {
  uint8_t command[1 + 1] = {MIP_CMD_ENABLE_CLAP, enabled};
  m_mip.serial.rawSend(command, sizeof(command));
}

// This internal protected method sends the set clap delay command with no error
// checking. The error handling / recovery happens at a higher level of the
// driver.
void MiP_Clap::rawSetDelay(uint16_t delay) {
  uint8_t command[1 + 2] = {
      MIP_CMD_SET_CLAP_DELAY, (uint8_t)(delay >> 8), (uint8_t)(delay & 0xFF)};
  m_mip.serial.rawSend(command, sizeof(command));
}

// This internal protected method sends the get clap settings command with
// minimal error handling. The error recovery happens at a higher level of the
// driver.
int8_t MiP_Clap::rawGetSettings(MiPClapSettings& settings) {
  const uint8_t getClapSettings[1] = {MIP_CMD_GET_CLAP_SETTINGS};
  uint8_t response[1 + 3];
  size_t responseLength;
  int8_t result = m_mip.serial.rawReceive(getClapSettings,
                                          sizeof(getClapSettings),
                                          response,
                                          sizeof(response),
                                          responseLength);

  if (result)
    return result;

  if (responseLength != sizeof(response) ||
      response[0] != MIP_CMD_GET_CLAP_SETTINGS ||
      (response[1] != MIP_CLAP_DISABLED && response[1] != MIP_CLAP_ENABLED)) {
    return MiP::MIP_ERROR_BAD_RESPONSE;
  }

  settings.enabled = (MiPClapEnabled)response[1];
  settings.delay = (uint16_t)response[2] << 8 | response[3];
  return MiP::MIP_ERROR_NONE;
}
