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
 * @file MPU_Battery.cpp
 * @brief Reads MiP's battery voltage from cached status data.
 *
 * No direct serial request is sent to the robot — relies on the latest
 * Out-Of-Band (OOB) status update. The latest update is never more than
 * 30 seconds old.
 */
#include "MPU_Battery.h"
#include "MPU_D1_mini.h"

// Implement the constructor to store the MiP reference.
MiP_Battery::MiP_Battery(MiP& mip) : m_mip(mip) {}

/**
 * @brief Reads the most recent cached value of MiP's battery voltage.
 *
 * This function processes any pending Out-Of-Band status events to keep the
 * cache up to date. It does not transmit a new request to MiP.
 *
 * @return Battery voltage, typically 4.0V (low) to 6.4V (fully charged).
 */
float MiP_Battery::readVoltage() {
  MIP_DEBUG_INFO_PRINTLN("MiP->Battery->readVoltage()");
  // Fetch bytes from the Serial receive buffer and process any event data
  // found within.
  m_mip.serial.processAllResponseData();

  m_mip.m_lastError = MiP::MIP_ERROR_NONE;
  return m_mip.m_lastStatus.battery;
}