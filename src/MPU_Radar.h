/**
 * @file MPU_Radar.h
 * @brief Defines the public interface for radar tracking in the MiP library.
 *
 * @details This header declares the radar API used to enable and read radar
 * events.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
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
  /**
   * @brief MiP protocol command bytes used by the radar subsystem.
   *
   * These values are placed in the first byte of requests sent to the MiP
   * (and appear in the corresponding responses).  See the official
   * [MiP BLE
   * Protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md)
   * for the complete list.
   */
  static constexpr uint8_t MIP_CMD_GET_GESTURE_RADAR_MODE = 0x0D;
  static constexpr uint8_t MIP_CMD_SET_GESTURE_RADAR_MODE = 0x0C;
  static constexpr uint8_t MIP_CMD_GET_RADAR_RESPONSE = 0x0C;

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

  /**
   * @brief Handles an incoming radar OOB event from the transport layer.
   *
   * Called by MiP::dispatchEvent() when a MIP_CMD_GET_RADAR_RESPONSE
   * notification is received. Updates the cached distance and marks the
   * radar data as valid.
   *
   * @param radarCode Raw distance code from the MiP (MIP_RADAR_NONE ..
   *                  MIP_RADAR_0CM_10CM).
   */
  void processEvent(uint8_t radarCode);

 private:
  void verifiedSet(MiPRadarMode desiredMode);
  bool check(MiPRadarMode expectedMode);
  int8_t rawGet(MiPRadarMode& mode);
  void rawSet(MiPRadarMode mode);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  MiPRadar m_lastRadar;
  
  friend class MiP;
};

#endif  // MPU_RADAR_H
