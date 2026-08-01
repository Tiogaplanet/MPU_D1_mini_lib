/**
 * @file MiP_Power_Up_-_D1_mini.cpp
 * @brief Implements the core MiP API and library initialization.
 *
 * @details This source file implements the main MiP object lifecycle and shared
 * transport logic.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include "MiP_Power_Up_-_D1_mini.h"

// Number of times that begin() method should try to initialize the MiP.
#define MIP_MAX_BEGIN_RETRIES 5

// Number of milliseconds to wait between retries in begin().
#define MIP_BEGIN_RETRY_WAIT 500

// Baud rate used for the ESP8266 debug channel.
#define ESP8266_DEBUG_BAUD_RATE 74880

// Fast baud rate for MiP communications.
#define MIP_FAST_BAUD_RATE 115200

// Slow baud rate for MiP communications.  MiPs support one or the other.
#define MIP_SLOW_BAUD_RATE 9600

MiP::MiP()
    : battery(*this),
      chestLED(*this),
      clap(*this),
      eeprom(*this),
      gesture(*this),
      headLEDs(*this),
      infrared(*this),
      mode(*this),
      motion(*this),
      odometer(*this),
      position(*this),
      radar(*this),
      serial(*this),
      shake(*this),
      sound(*this),
      version(*this),
      weight(*this),
      wifi(*this) {
  clear();
}

MiP::~MiP() {
  end();
}

bool MiP::begin() {
  // Setup the debugging channel.
  Serial1.begin(ESP8266_DEBUG_BAUD_RATE);

  // Initialize the class members.
  clear();

  // Assume that the connection to MiP will be successfully initialized. Will
  // clear the flag if a connection error is detected. If this wasn't done then
  // the calls to rawSend() and rawGetStatus() below would fail.
  m_flags |= MIP_FLAG_INITIALIZED;

  // Sometimes the init fails. It seems to happen when the MiP is busy at
  // power-up doing other things like attempting to balance.
  int8_t retry;
  for (retry = 0; retry < MIP_MAX_BEGIN_RETRIES; retry++) {
    // Try to connect at 115200 baud, the rate used by some MiPs.
    int8_t result = attemptMiPConnection(MIP_FAST_BAUD_RATE);
    if (result == MIP_ERROR_NONE)
      return true;

    // Try to connect at 9600 baud if the fast attempt failed.
    result = attemptMiPConnection(MIP_SLOW_BAUD_RATE);
    if (result == MIP_ERROR_NONE)
      return true;
  }

  // Get here if the connection attempt to MiP never succeeds.
  m_flags &= ~MIP_FLAG_INITIALIZED;
  end();
  return false;
}

void MiP::end() {
  if (isInitialized()) {
    // Restore MiP's default volume in case it was changed by the user.
    sound.end();

    // Send the disconnect command.  If it is successful the app will be
    // disconnected, indicated by a blue chest LED.
    const uint8_t command[] = {MIP_CMD_DISCONNECT_APP};
    serial.rawSend(command, sizeof(command));
  }

  clear();

  // Swap the UART on the D1 mini back to the default RX/TX pair.
  Serial.swap();
  Serial.end();

  // Shutdown the debugging channel.
  Serial1.end();
}

void MiP::sleep() {
  // Put the MiP to sleep.
  // The MiP will need to be reset before another begin() will succeed.
  const uint8_t command[] = {MIP_CMD_SLEEP};
  serial.rawSend(command, sizeof(command));
}

bool MiP::isInitialized() {
  return (m_flags & MIP_FLAG_INITIALIZED);
}

int8_t MiP::lastCallResult() {
  return m_lastError;
}

bool MiP::didLastCallFail() {
  return m_lastError != MIP_ERROR_NONE;
}

void MiP::printLastCallResult() {
  if (m_lastError != MIP_ERROR_NONE) {
    MIP_DEBUG_ERROR_PRINT(F("MiP: API returned "));
    switch (m_lastError) {
      case MIP_ERROR_TIMEOUT:
        MIP_DEBUG_ERROR_PRINTLN(
            F("MIP_ERROR_TIMEOUT (Timed out waiting for response)"));
        break;
      case MIP_ERROR_NO_EVENT:
        MIP_DEBUG_ERROR_PRINTLN(
            F("MIP_ERROR_NO_EVENT (No event has arrived from MiP yet)"));
        break;
      case MIP_ERROR_BAD_RESPONSE:
        MIP_DEBUG_ERROR_PRINTLN(
            F("MIP_ERROR_BAD_RESPONSE (Unexpected response from MiP)"));
        break;
      case MIP_ERROR_MAX_RETRIES:
        MIP_DEBUG_ERROR_PRINTLN(
            F("MIP_ERROR_MAX_RETRIES (Exceeded maximum number of retries)"));
        break;
      default:
        MIP_DEBUG_ERROR_PRINTLN(F("unknown error"));
        break;
    }
  }
}

void MiP::dispatchEvent(uint8_t command,
                        const uint8_t* payload,
                        size_t length) {
  switch (command) {
    case MiP_Clap::MIP_CMD_CLAP_RESPONSE:
      // A clap event was found. Dispatch it to the Clap component.
      if (length >= 2) {
        clap.processEvent(payload[1]);
      }
      break;
    case MiP_Weight::MIP_CMD_GET_WEIGHT:
      // A weight event was found. Dispatch it to the Weight component.
      MIP_DEBUG_INFO_PRINT(
          "MiP->Core->dispatchEvent(), in weight case. payload[1]: ");
      MIP_DEBUG_INFO_PRINTLN(payload[1]);
      if (length >= 2) {
        weight.processEvent(payload[1]);
      }
      break;
    case MiP_Gesture::MIP_CMD_GET_GESTURE_RESPONSE:
      // A gesture was found. Dispatch to the Gesture component.
      if (length >= 2) {
        gesture.processEvent(payload[1]);
      }
      break;
    case MIP_CMD_GET_STATUS:
      // A status update arrived. Parse it and update the core status.
      parseStatus(this->m_lastStatus, payload, length);
      break;
    case MiP_Infrared::MIP_CMD_GET_DETECTED_MIP:
    case MiP_Infrared::MIP_CMD_RECEIVE_IR_DONGLE_CODE:
      // Both of these commands are handled by the Infrared component.
      // We pass the entire event (command, payload, and length) to its
      // flexible processEvent() method, which will use a switch statement
      // internally to tell them apart.
      infrared.processEvent(command, payload, length);
      break;
    case MiP_Shake::MIP_CMD_SHAKE_RESPONSE:
      m_flags |= MIP_FLAG_SHAKE_DETECTED;
      break;
    case MiP_Radar::MIP_CMD_GET_RADAR_RESPONSE:
      if (length >= 2) {
        radar.processEvent(payload[1]);
      }
      break;
    default:
      // An unknown OOB event was received.
      MIP_DEBUG_WARN_PRINTF("MiP: Unknown OOB Event: 0x%02X\n", command);
      break;
  }
}

// ==========================================================================
// Protected functions.
// ==========================================================================

void MiP::clear() {
  m_flags = 0;
  m_lastError = MIP_ERROR_NONE;
  m_lastStatus.clear();
  clap.clear();
  gesture.clear();
  infrared.clear();
  radar.clear();
  serial.clear();
  weight.clear();
  wifi.clear();
}

// This internal protected method provides the common code for connection
// attempts at baud rates of 115200 or 9600.
int8_t MiP::attemptMiPConnection(uint32_t baudRate) {
  // Set baud rate to specified rate.
  Serial.begin(baudRate);

  // Swap the UART of the D1 mini to the alternate pins.
  Serial.swap();

  // Send 0xFF to the MiP via UART to enable the UART communication channel in
  // the MiP.
  const uint8_t initMipCommand[] = {0xFF};
  serial.rawSend(initMipCommand, sizeof(initMipCommand));

  // The MiP UART documentation indicates that this delay is required after
  // sending 0xFF.
  delay(30);

  // Flush any outstanding junk data in receive buffer.
  serial.discardUnexpectedSerialData();

  // Attempt to get MiP's latest status to see if the connection was successful
  // or not.
  int8_t result = rawGetStatus(m_lastStatus);
  if (result == MIP_ERROR_NONE) {
    // Let the user know at which baud rate the connection to MiP was made.
    MIP_DEBUG_INFO_PRINTF("MiP: Connected at %d baud\n\r", baudRate);
  } else {
    // Sleep a bit before returning to code which will retry connection at
    // alternate baud rate.
    delay(MIP_BEGIN_RETRY_WAIT);
  }
  return result;
}

// This internal protected method sends the get status command with minimal
// error handling. The error recovery happens at a higher level of the driver in
// begin(). All status updates after begin() come from events.
int8_t MiP::rawGetStatus(MiPStatus& status) {
  const uint8_t getStatus[1] = {MIP_CMD_GET_STATUS};
  uint8_t response[1 + 2];
  size_t responseLength;
  int result = serial.rawReceive(
      getStatus, sizeof(getStatus), response, sizeof(response), responseLength);
  if (result)
    return result;
  return parseStatus(status, response, responseLength);
}

// This internal protected method takes the status response, validates it,
// converts it into convenient units and packs the result into a MiPStatus
// class.
int8_t MiP::parseStatus(MiPStatus& status,
                        const uint8_t response[],
                        size_t responseLength) {
  if (responseLength != 3 || response[0] != MIP_CMD_GET_STATUS ||
      response[2] > MIP_POSITION_ON_BACK_WITH_KICKSTAND) {
    return MIP_ERROR_BAD_RESPONSE;
  }

  // Convert battery integer value to floating point voltage value.
  status.battery =
      (float)(((response[1] - 0x4D) / (float)(0x7C - 0x4D)) * (6.4f - 4.0f)) +
      4.0f;
  status.position = (MiPPosition)response[2];
  return MIP_ERROR_NONE;
}

void MiP::mipAssert(bool condition, uint32_t lineNumber, const char* fileName) {
  (void)lineNumber;
  (void)fileName;
  if (!condition) {
    MIP_DEBUG_ERROR_PRINTF(
        "MiP: Assert failed in file %s at line: %d\n", fileName, lineNumber);
    while (true) {
      delay(100);
    }
  }
}
