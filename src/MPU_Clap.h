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
 * @file MPU_Clap.h
 * @brief Defines the one and only function related to MiP's battery - read the
 * voltage.
 */
#ifndef MPU_CLAP_H
#define MPU_CLAP_H

#include <stdint.h>

#include "MPU_Queue.h"

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Clap event reporting state.
 */
enum MiPClapEnabled : uint8_t {
  MIP_CLAP_DISABLED = 0x00,
  MIP_CLAP_ENABLED = 0x01,
};

/**
 * @brief Current clap detection settings.
 */
class MiPClapSettings {
 public:
  MiPClapSettings() {
    clear();
  }
  void clear() {
    enabled = MIP_CLAP_DISABLED;
    delay = 0;
  }
  MiPClapEnabled enabled;
  uint16_t delay;
};

/**
 * @brief Manages MiP's clap detection system.
 */
class MiP_Clap {
 public:
  // MiP Protocol Commands related to clap detection.
  // These command codes are placed in the first byte of requests sent to the
  // MiP and responses sent back from the MiP. See
  // https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
  // for the complete list.
  static constexpr uint8_t MIP_CMD_ENABLE_CLAP = 0x1E;
  static constexpr uint8_t MIP_CMD_SET_CLAP_DELAY = 0x20;
  static constexpr uint8_t MIP_CMD_GET_CLAP_SETTINGS = 0x1F;

  /**
   * @brief Constructs the Clap system manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Clap(MiP& mip);

  void clear();

  void processEvent(uint8_t clapCode);

  /**
   * @brief Enables clap event reporting from the MiP robot.
   *
   * This verified method sends the enable command and reads back settings
   * to confirm success. It retries on failure.
   */
  void enableEvents();

  /**
   * @brief Disables clap event reporting from the MiP robot.
   *
   * This verified method sends the disable command and reads back settings
   * to confirm success. It retries on failure.
   */
  void disableEvents();

  /**
   * @brief Checks if clap event reporting is currently enabled.
   *
   * @return true if enabled, false otherwise.
   */
  bool areEventsEnabled();

  /**
   * @brief Returns the number of unread clap events in the queue.
   *
   * Processes any pending serial data first to update the queue.
   *
   * @return Number of available clap events.
   */
  uint8_t availableEvents();

  /**
   * @brief Reads the next available clap event from the queue.
   *
   * Processes pending serial data first. If no event is available,
   * sets last error to MIP_ERROR_NO_EVENT.
   *
   * @return The clap event code, or 0 if none available.
   */
  uint8_t readEvent();

  /**
   * @brief Reads the current clap delay setting.
   *
   * @return The delay in milliseconds between clap events.
   *         Returns 0 on error.
   */
  uint16_t readDelay();

  /**
   * @brief Sets the minimum delay between clap events.
   *
   * Verified method: sends the new delay and confirms by reading back
   * the settings. Retries automatically on mismatch or error.
   *
   * @param delay Delay in milliseconds between allowed clap reports.
   */
  void writeDelay(uint16_t delay);

 private:
  void checkedEnableEvents(MiPClapEnabled enabled);
  int8_t readSettings(MiPClapSettings& settings);
  void rawEnable(MiPClapEnabled enabled);
  void rawSetDelay(uint16_t delay);
  int8_t rawGetSettings(MiPClapSettings& settings);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  CircularQueue<uint8_t, 8> m_clapEvents;
};

#endif  // MPU_CLAP_H
