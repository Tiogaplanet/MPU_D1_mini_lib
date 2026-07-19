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
/* This file implements decoding and fetching for gesture inputs.
*/
#include "MPU_D1_mini.h"

#define MIP_MAX_RETRIES 2
#define MIP_RETRY_WAIT 50

// MiP Protocol Commands related to sensors.
// These command codes are placed in the first byte of requests sent to the MiP
// and responses sent back from the MiP. See
// https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
// for the complete list.
#define MIP_CMD_GET_GESTURE_RADAR_MODE 0x0D
#define MIP_CMD_SET_GESTURE_RADAR_MODE 0x0C

// Define an assert mechanism that can be used to log and halt when the user is
// found to be calling the API incorrectly.
#define MIP_ASSERT(EXPRESSION) \
  if (!(EXPRESSION))           \
    mipAssert(__LINE__);

static void mipAssert(uint32_t lineNumber) {
  MIP_DEBUG_ERROR_PRINTF("MiP: Assert: MPU_Gesture.cpp: %d\n", lineNumber);
  while (1) {
    delay(100);
  }
}

void MiP::enableGestureMode() {
  verifiedSetGestureRadarMode(MIP_GESTURE);
}

void MiP::disableGestureMode() {
  verifiedSetGestureRadarMode(MIP_GESTURE_RADAR_DISABLED);
}

bool MiP::isGestureModeEnabled() {
  return checkGestureRadarMode(MIP_GESTURE);
}

uint8_t MiP::availableGestureEvents() {
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  processAllResponseData();
  m_lastError = MIP_ERROR_NONE;
  return m_gestureEvents.available();
}

MiPGesture MiP::readGestureEvent() {
  processAllResponseData();
  MiPGesture gestureEvent = MIP_GESTURE_INVALID;
  if (!m_gestureEvents.pop(gestureEvent)) {
    m_lastError = MIP_ERROR_NO_EVENT;
    return MIP_GESTURE_INVALID;
  }
  m_lastError = MIP_ERROR_NONE;
  return gestureEvent;
}
