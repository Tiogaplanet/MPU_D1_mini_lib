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
 * @file MPU_Radar.h
 * @brief Defines the functions for interfacing with MiP's radar system.
 */
#ifndef MPU_RADAR_H
#define MPU_RADAR_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Radar distance ranges.
 */
enum MiPRadar : uint8_t {
  MIP_RADAR_NONE = 0x01,
  MIP_RADAR_10CM_30CM = 0x02,
  MIP_RADAR_0CM_10CM = 0x03,
  MIP_RADAR_INVALID =
      0xFF  // Is set to this value when there are no current radar events.
};

/**
 * @brief Gesture or Radar operating mode.
 */
enum MiPRadarMode : uint8_t {
  MIP_RADAR_DISABLED = 0x00,
  // MIP_GESTURE = 0x02,
  MIP_RADAR = 0x04,
};

/**
 * @brief Manages MiP's radar system.
 */
class MiP_Radar {
 public:
  // MiP Protocol Commands related to the radat system.
  // These command codes are placed in the first byte of requests sent to the
  // MiP and responses sent back from the MiP. See
  // https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
  // for the complete list.
  static constexpr uint8_t MIP_CMD_GET_GESTURE_RADAR_MODE = 0x0D;
  static constexpr uint8_t MIP_CMD_SET_GESTURE_RADAR_MODE = 0x0C;

  /**
   * @brief Constructs the radar manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Radar(MiP& mip);

  void clear();

  /**
   * @brief Enables radar tracking mode on the MiP.
   *
   * Uses verified mode switching (command + read-back confirmation with retry).
   */
  void enable();

  /**
   * @brief Disables radar tracking mode.
   *
   * Uses verified mode switching (command + read-back confirmation with retry).
   */
  void disable();

  /**
   * @brief Checks whether radar tracking mode is currently active.
   *
   * @return true if radar mode is enabled.
   */
  bool isEnabled();

  /**
   * @brief Reads the most recent radar tracking data.
   *
   * Uses cached value from the latest OOB status event. Processes pending
   * serial data first.
   *
   * @return Current radar value or MIP_RADAR_INVALID if no data received yet.
   */
  MiPRadar read();

 private:
  void verifiedSet(MiPRadarMode desiredMode);
  bool check(MiPRadarMode expectedMode);
  int8_t rawGet(MiPRadarMode& mode);
  void rawSet(MiPRadarMode mode);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  MiPRadar m_lastRadar;
};

#endif  // MPU_RADAR_H
