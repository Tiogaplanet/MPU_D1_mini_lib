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
/* Contains the constructor, destructor, fundamental connection establishment
   to MiP, and functions that are common to multiple subsystems, i.e. radar
   and gesture, position and battery, etc.
 */
#include "MPU_D1_mini.h"

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

// MiP Protocol Commands related to core functions.
// These command codes are placed in the first byte of requests sent to the MiP
// and responses sent back from the MiP. See
// https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
// for the complete list.
#define MIP_CMD_DISCONNECT_APP 0xFE
#define MIP_CMD_SLEEP 0xFA
#define MIP_CMD_GET_STATUS 0x79
#define MIP_CMD_SET_GESTURE_RADAR_MODE 0x0C
#define MIP_CMD_GET_GESTURE_RADAR_MODE 0x0D

MiP::MiP() {
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

  // Roll the timers back so that the first calls can occur immediately.
  m_lastRequestTime =
      millis() - 10;  // MIP_REQUEST_DELAY; // (10) offset slightly
  m_lastContinuousDriveTime =
      millis() - 50;  // MIP_CONTINUOUS_DRIVE_DELAY; // (50)

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
    writeVolume(MIP_VOLUME_7);

    // Send the disconnect command.  If it is successful the app will be
    // disconnected, indicated by a blue chest LED.
    const uint8_t command[] = {MIP_CMD_DISCONNECT_APP};
    rawSend(command, sizeof(command));
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
  rawSend(command, sizeof(command));
}

bool MiP::isInitialized() {
  return (m_flags & MIP_FLAG_INITIALIZED);
}

int8_t MiP:: lastCallResult() {
  return m_lastError;
}

bool MiP:: didLastCallFail() {
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

bool MiP::areGestureAndRadarModesDisabled() {
  return checkGestureRadarMode(MIP_GESTURE_RADAR_DISABLED);
}

// ==========================================================================
// Protected functions.
// ==========================================================================

void MiP::clear() {
  m_lastRequestTime = millis();
  m_lastContinuousDriveTime = millis();
  m_flags = 0;
  memset(m_responseBuffer, 0, sizeof(m_responseBuffer));
  m_expectedResponseCommand = 0;
  m_expectedResponseSize = 0;
  m_lastError = MIP_ERROR_NONE;
  memset(m_playCommand, 0, sizeof(m_playCommand));
  m_soundIndex = -1;
  m_playVolume = MIP_VOLUME_OFF;
  m_lastRadar = MIP_RADAR_INVALID;
  m_lastStatus.clear();
  m_lastWeight = 0;
  m_clapEvents.clear();
  m_gestureEvents.clear();
  m_detectedMiPEvents.clear();
  m_irCodeEvents.clear();
  m_irId = 0x00;
  memset(m_ssid, 0, sizeof(m_ssid));
  memset(m_password, 0, sizeof(m_password));
  memset(m_hostname, 0, sizeof(m_hostname));
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
  rawSend(initMipCommand, sizeof(initMipCommand));

  // The MiP UART documentation indicates that this delay is required after
  // sending 0xFF.
  delay(30);

  // Flush any outstanding junk data in receive buffer.
  discardUnexpectedSerialData();

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

// This internal protected method sends the command to change the radar/gesture
// mode and then sends a request to get the new state. If this request fails or
// the new state isn't as expected, it will retry the command.
void MiP::verifiedSetGestureRadarMode(MiPGestureRadarMode desiredMode) {
  int8_t result;

  // Always mark cached RADAR data as invalid when changing modes.

  m_flags &= ~MIP_FLAG_RADAR_VALID;
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    rawSetGestureRadarMode(desiredMode);

    // Read back and make sure that it was set as expected.
    MiPGestureRadarMode actualMode = MIP_GESTURE_RADAR_DISABLED;
    result = rawGetGestureRadarMode(actualMode);
    if (result == MIP_ERROR_NONE && actualMode == desiredMode) {
      // The set was successful so return immediately.
      m_lastError = MIP_ERROR_NONE;
      return;
    }

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }

  if (result != MIP_ERROR_NONE) {
    // Kept getting an error back from rawGetGestureRadarMode().
    m_lastError = result;
  } else {
    // rawGetGestureRadarMode() was successful but didn't match mode to which we
    // were attempting to change.
    m_lastError = MIP_ERROR_MAX_RETRIES;
  }
}

// This internal protected method requests the current radar/gesture mode and
// then returns whether it matches the passed in value or not. It includes retry
// code incase the request should fail.
bool MiP::checkGestureRadarMode(MiPGestureRadarMode expectedMode) {
  int8_t result;
  for (uint8_t retry = 0; retry < MIP_MAX_RETRIES; retry++) {
    MiPGestureRadarMode currentMode;
    result = rawGetGestureRadarMode(currentMode);
    if (result == MIP_ERROR_NONE)
      return currentMode == expectedMode;

    // An error was encountered so we will loop around and try again.
    // Wait for a bit before the next retry.
    delay(MIP_RETRY_WAIT);
  }
  m_lastError = result;
  return false;
}

// This internal protected method sends the get gesture/radar mode command with
// minimal error handling. The error recovery happens at a higher level of the
// driver.
int8_t MiP::rawGetGestureRadarMode(MiPGestureRadarMode& mode) {
  const uint8_t getGestureRadarMode[1] = {MIP_CMD_GET_GESTURE_RADAR_MODE};
  uint8_t response[1 + 1];
  size_t responseLength;
  int8_t result = rawReceive(getGestureRadarMode,
                             sizeof(getGestureRadarMode),
                             response,
                             sizeof(response),
                             responseLength);
  if (result)
    return result;
  if (responseLength != 2 || response[0] != MIP_CMD_GET_GESTURE_RADAR_MODE ||
      (response[1] != MIP_GESTURE_RADAR_DISABLED &&
       response[1] != MIP_GESTURE && response[1] != MIP_RADAR)) {
    return MIP_ERROR_BAD_RESPONSE;
  }
  mode = (MiPGestureRadarMode)response[1];
  return MIP_ERROR_NONE;
}

// This internal protected method sends the set gesture/radar mode command with
// no error checking. The error handling / recovery happens at a higher level of
// the driver.
void MiP::rawSetGestureRadarMode(MiPGestureRadarMode mode) {
  uint8_t command[1 + 1] = {MIP_CMD_SET_GESTURE_RADAR_MODE, mode};
  rawSend(command, sizeof(command));
}

// This internal protected method sends the get status command with minimal
// error handling. The error recovery happens at a higher level of the driver in
// begin(). All status updates after begin() come from events.
int8_t MiP::rawGetStatus(MiPStatus& status) {
  const uint8_t getStatus[1] = {MIP_CMD_GET_STATUS};
  uint8_t response[1 + 2];
  size_t responseLength;
  int result = rawReceive(
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

void MiP::mipAssert(uint32_t lineNumber) {
  // We can use __FILE__ here to dynamically print whichever of the 24 files
  // triggered the assert!
  MIP_DEBUG_ERROR_PRINTF(
      "MiP: Assert failed in file %s at line: %d\n", __FILE__, lineNumber);
  while (1) {
    delay(100);
  }
}
