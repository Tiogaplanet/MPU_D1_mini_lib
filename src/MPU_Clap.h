/**
 * @file MPU_Clap.h
 * @brief Defines the public interface for clap event handling in the MiP
 * library.
 *
 * @details This header declares the clap-detection API used by the MiP library.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
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
  MIP_CLAP_DISABLED = 0x00,  ///< Clap event detection is disabled.
  MIP_CLAP_ENABLED = 0x01,   ///< Clap event detection is enabled.
};

/**
 * @brief Current clap detection settings.
 *
 * @details Stores the current clap detection status (enabled or disabled)
 * along with the minimum delay threshold configured between clap reports.
 */
class MiPClapSettings {
 public:
  /**
   * @brief Constructs a new MiPClapSettings object and initializes default
   * values.
   *
   * @details Automatically invokes clear() to reset state.
   */
  MiPClapSettings() {
    clear();
  }

  /**
   * @brief Resets settings to default (disabled, 0ms delay).
   */
  void clear() {
    enabled = MIP_CLAP_DISABLED;
    delay = 0;
  }

  MiPClapEnabled enabled;  ///< Current clap detection status.
  uint16_t delay;  ///< Minimum delay in milliseconds between clap reports.
};

/**
 * @brief MiP_Clap manages MiP's clap detection system, from enabling to
 * disabling, configuring, and reporting on detected clap events.
 */
class MiP_Clap {
 public:
  /**
   * @brief The MiP protocol command to enable MiP's clap detection system.
   */
  static constexpr uint8_t MIP_CMD_ENABLE_CLAP = 0x1E;

  /**
   * @brief The MiP protocol response reporting the number of claps detected.
   */
  static constexpr uint8_t MIP_CMD_CLAP_RESPONSE = 0x1D;

  /**
   * @brief The MiP protocol command to set the minimum delay time between
   * claps.
   */
  static constexpr uint8_t MIP_CMD_SET_CLAP_DELAY = 0x20;

  /**
   * @brief The MiP protocol command to read MiP's clap detection settings.
   */
  static constexpr uint8_t MIP_CMD_GET_CLAP_SETTINGS = 0x1F;

  /**
   * @brief Enables MiP's clap event reporting. This verified method sends the
   * enable command and reads back settings to confirm success. It retries on
   * failure.
   */
  void enableEvents();

  /**
   * @brief Disables clap event reporting from MiP. This verified
   * method sends the disable command and reads back settings to confirm
   * success. It retries on failure.
   */
  void disableEvents();

  /**
   * @brief Checks if clap event reporting is currently enabled.
   *
   * @return true if enabled, false otherwise.
   */
  bool areEventsEnabled();

  /**
   * @brief Returns the number of unread clap events in the queue. Processes any
   * pending serial data first to update the queue.
   *
   * @return Number of available clap events.
   */
  uint8_t availableEvents();

  /**
   * @brief Reads the next available clap event from the queue. Processes
   * pending serial data first. If no event is available, sets last error to
   * MIP_ERROR_NO_EVENT.
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
   * @brief Sets the minimum delay between clap events. Verified method: sends
   * the new delay and confirms by reading back the settings. Retries
   * automatically on mismatch or error.
   *
   * @param delay Delay in milliseconds between allowed clap reports.
   */
  void writeDelay(uint16_t delay);

 protected:
  void clear();

 private:
  /**
   * @brief Constructs the clap system manager.
   *
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Clap(MiP& mip);

  void checkedEnableEvents(MiPClapEnabled enabled);
  int8_t readSettings(MiPClapSettings& settings);
  void rawEnable(MiPClapEnabled enabled);
  void rawSetDelay(uint16_t delay);
  int8_t rawGetSettings(MiPClapSettings& settings);

  /**
   * @brief Handles an incoming clap event notification from the transport
   * layer.
   *
   * @param clapCode Raw clap count/code received from MiP.
   */
  void processEvent(uint8_t clapCode);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  mip_detail::CircularQueue<uint8_t, 8> m_clapEvents;

  friend class MiP;
};

#endif  // MPU_CLAP_H
