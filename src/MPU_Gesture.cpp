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
 * @file MPU_Gesture.cpp
 * @brief Implements enabling, disabling, and reading gesture events from the
 *        WowWee MiP robot.
 *
 * Gesture mode shares infrastructure with radar mode. Events are queued and
 * retrieved via the common response processing system.
 */
#include "MPU_D1_mini.h"

// MiP Protocol Commands related to sensors.
// These command codes are placed in the first byte of requests sent to the MiP
// and responses sent back from the MiP. See
// https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
// for the complete list.
#define MIP_CMD_GET_GESTURE_RADAR_MODE 0x0D
#define MIP_CMD_SET_GESTURE_RADAR_MODE 0x0C

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
