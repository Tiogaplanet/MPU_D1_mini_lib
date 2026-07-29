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
#include "MPU_Position.h"
#include "MPU_D1_mini.h"

// Implement the constructor to store the MiP reference.
MiP_Position::MiP_Position(MiP& mip) : m_mip(mip) {}

MiPPosition MiP_Position::read() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->read()");
  // Fetch bytes from the Serial receive buffer and process any event data found
  // within.
  m_mip.serial.processAllResponseData();

  m_mip.m_lastError = MiP::MIP_ERROR_NONE;
  return m_mip.m_lastStatus.position;
}

bool MiP_Position::isOnBack() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isOnBack()");
  return read() == MIP_POSITION_ON_BACK;
}
bool MiP_Position::isFaceDown() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isFaceDown()");
  return read() == MIP_POSITION_FACE_DOWN;
}
bool MiP_Position::isUpright() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isUpright()");
  return read() == MIP_POSITION_UPRIGHT;
}
bool MiP_Position::isPickedUp() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isPickedUp()");
  return read() == MIP_POSITION_PICKED_UP;
}
bool MiP_Position::isHandStanding() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isHandStanding()");
  return read() == MIP_POSITION_HAND_STAND;
}
bool MiP_Position::isFaceDownOnTray() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isFaceDownOnTray()");
  return read() == MIP_POSITION_FACE_DOWN_ON_TRAY;
}
bool MiP_Position::isOnBackWithKickstand() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Position->isOnBackWithKickstand()");
  return read() == MIP_POSITION_ON_BACK_WITH_KICKSTAND;
}
