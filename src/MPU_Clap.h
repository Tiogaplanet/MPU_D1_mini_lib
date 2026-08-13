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
 * @brief Manages MiP's clap detection system, from enabling/disabling
 * to delay configuration and event reading.
 */
class MiP_Clap {
public:
  /**
   * @brief Enables MiP's clap event reporting.
   *
   * @details Verified method: sends the enable command and reads back settings
   * to confirm success. Retries automatically on failure.
   */
  void enableEvents();

  /**
   * @brief Disables clap event reporting from MiP.
   *
   * @details Verified method: sends the disable command and reads back settings
   * to confirm success. Retries automatically on failure.
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
   * @details Processes any pending serial data first to update the queue.
   *
   * @return uint8_t Number of available clap events.
   */
  uint8_t availableEvents();

  /**
   * @brief Reads the next available clap event from the queue.
   *
   * @details Processes pending serial data first. If no event is available,
   * sets last error to MIP_ERROR_NO_EVENT.
   *
   * @return uint8_t The clap event code, or 0 if none available.
   */
  uint8_t readEvent();

  /**
   * @brief Reads the current clap delay setting.
   *
   * @return uint16_t The delay in milliseconds between clap events, or 0 on
   * error.
   */
  uint16_t readDelay();

  /**
   * @brief Sets the minimum delay between clap events.
   *
   * @details Verified method: sends the new delay and confirms by reading back
   * settings. Retries automatically on mismatch or error.
   *
   * @param delay Delay in milliseconds between allowed clap reports.
   */
  void writeDelay(uint16_t delay);

protected:
  static constexpr uint8_t MIP_CMD_ENABLE_CLAP = 0x1E;  ///< Protocol command to
                                                        ///< enable/disable clap
                                                        ///< system.
  static constexpr uint8_t MIP_CMD_CLAP_RESPONSE = 0x1D;  ///< Protocol
                                                          ///< notification byte
                                                          ///< reporting
                                                          ///< detected claps.
  static constexpr uint8_t MIP_CMD_SET_CLAP_DELAY = 0x20;  ///< Protocol command
                                                           ///< to set
                                                           ///< inter-clap
                                                           ///< delay.
  static constexpr uint8_t MIP_CMD_GET_CLAP_SETTINGS = 0x1F;  ///< Protocol
                                                              ///< command to
                                                              ///< query clap
                                                              ///< settings.

  void clear();

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   *
   * @param mip Reference to the main MiP object to access core services.
   */
  explicit MiP_Clap(MiP& mip);

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
  friend class MiP_Serial;
};

#endif  // MPU_CLAP_H
