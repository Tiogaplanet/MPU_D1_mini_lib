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
 * @file MPU_Odometer.h
 * @brief Manages MiP's odometer, reading and reseting.
 */
#ifndef MPU_ODOMETER_H
#define MPU_ODOMETER_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Manages MiP's battery monitoring.
 */
class MiP_Odometer {
 public:
  // MiP Protocol Commands related to the odometer.
  // These command codes are placed in the first byte of requests sent to the
  // MiP and responses sent back from the MiP. See
  // https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
  // for the complete list.
  static constexpr uint8_t MIP_CMD_READ_ODOMETER = 0x85;
  static constexpr uint8_t MIP_CMD_RESET_ODOMETER = 0x86;
  /**
   * @brief Constructs the Battery manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Odometer(MiP& mip);

  /**
   * @brief Reads the total distance travelled by the MiP.
   *
   * Performs a verified read with automatic retries on error.
   *
   * @return Distance in centimeters. Returns 0.0 on failure.
   */
  float read();

  /**
   * @brief Resets the odometer distance to zero.
   */
  void reset();

 private:
  int8_t rawRead(float& distanceInCm);

  MiP& m_mip;  // Stores a reference to the main MiP class.
};

#endif  // MPU_ODOMETER_H
