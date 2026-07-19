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
 * @file MPU_Position.cpp
 * @brief Implements reading the MiP robot's physical orientation and position.
 *
 * All functions rely on cached status data (updated via OOB events) and do not
 * transmit new requests to the robot.
 */
#include "MPU_D1_mini.h"

MiPPosition MiP::readPosition() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->readPosition()");
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  processAllResponseData();

  m_lastError = MIP_ERROR_NONE;
  return m_lastStatus.position;
}

bool MiP::isOnBack() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isOnBack()");
  return readPosition() == MIP_POSITION_ON_BACK;
}
bool MiP::isFaceDown() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isFaceDown()");
  return readPosition() == MIP_POSITION_FACE_DOWN;
}
bool MiP::isUpright() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isUpright()");
  return readPosition() == MIP_POSITION_UPRIGHT;
}
bool MiP::isPickedUp() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isPickedUp()");
  return readPosition() == MIP_POSITION_PICKED_UP;
}
bool MiP::isHandStanding() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isHandStanding()");
  return readPosition() == MIP_POSITION_HAND_STAND;
}
bool MiP::isFaceDownOnTray() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isFaceDownOnTray()");
  return readPosition() == MIP_POSITION_FACE_DOWN_ON_TRAY;
}
bool MiP::isOnBackWithKickstand() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isOnBackWithKickstand()");
  return readPosition() == MIP_POSITION_ON_BACK_WITH_KICKSTAND;
}
