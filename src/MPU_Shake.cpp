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
 * @file MPU_Shake.cpp
 * @brief Implements detection of shake events on the WowWee MiP robot.
 *
 * Uses cached flag from OOB (Out-Of-Band) status events. No new serial
 * transmission is sent to the robot when querying.
 */
#include "MPU_D1_mini.h"

bool MiP::hasBeenShaken() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Shake->hasBeenShaken()");
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  processAllResponseData();
  m_lastError = MIP_ERROR_NONE;
  if (m_flags & MIP_FLAG_SHAKE_DETECTED) {
    // A shake event has been received since the last call to this function.
    // Return true and clear the shake detected bit.
    m_flags &= ~MIP_FLAG_SHAKE_DETECTED;
    return true;
  }
  return false;
}
