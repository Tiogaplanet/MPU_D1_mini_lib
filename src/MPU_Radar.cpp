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
/* This file implements decoding and fetching for all radar tracking steps.
 */
#include "MPU_D1_mini.h"

// Define an assert mechanism that can be used to log and halt when the user is
// found to be calling the API incorrectly.
#define MIP_ASSERT(EXPRESSION) \
  if (!(EXPRESSION))           \
    mipAssert(__LINE__);

static void mipAssert(uint32_t lineNumber) {
  MIP_DEBUG_ERROR_PRINTF("MiP: Assert: MPU_Radar.cpp: %d\n", lineNumber);
  while (1) {
    delay(100);
  }
}

void MiP::enableRadarMode() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Radar->enableRadarMode()");
  verifiedSetGestureRadarMode(MIP_RADAR);
}

void MiP::disableRadarMode() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Radar->disableRadarMode()");
  verifiedSetGestureRadarMode(MIP_GESTURE_RADAR_DISABLED);
}

bool MiP::isRadarModeEnabled() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Radar->isRadarModeEnabled()");
  return checkGestureRadarMode(MIP_RADAR);
}

MiPRadar MiP::readRadar() {
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  processAllResponseData();
  if ((m_flags & MIP_FLAG_RADAR_VALID) == 0) {
    // Haven't received a radar event yet.
    m_lastError = MIP_ERROR_NO_EVENT;
    return MIP_RADAR_INVALID;
  }
  m_lastError = MIP_ERROR_NONE;
  return m_lastRadar;
}
