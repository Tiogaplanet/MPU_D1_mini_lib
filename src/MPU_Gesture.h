/**
 * @file MPU_Gesture.h
 * @brief Defines the public interface for gesture handling in the MiP library.
 *
 * @details This header declares the gesture API used to enable and read gesture
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
#ifndef MPU_GESTURE_H
#define MPU_GESTURE_H

#include <stdint.h>

#include "MPU_Queue.h"

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Gesture or Radar operating mode.
 */
enum MiPGestureMode : uint8_t {
  MIP_GESTURE_RADAR_DISABLED = 0x00,
  MIP_GESTURE = 0x02
};

/**
 * @brief Recognized gesture directions.
 */
enum MiPGesture : uint8_t {
  MIP_GESTURE_LEFT = 0x0A,
  MIP_GESTURE_RIGHT = 0x0B,
  MIP_GESTURE_CENTER_SWEEP_LEFT = 0x0C,
  MIP_GESTURE_CENTER_SWEEP_RIGHT = 0x0D,
  MIP_GESTURE_CENTER_HOLD = 0x0E,
  MIP_GESTURE_FORWARD = 0x0F,
  MIP_GESTURE_BACKWARD = 0x10,
  MIP_GESTURE_INVALID =
      0xFF  // Is set to this value when there are no current gesture events.
};

/**
 * @brief Manages MiP's gesture subsystem.
 */
class MiP_Gesture {
 public:
  /**
   * @brief MiP protocol command bytes used by the gesture subsystem.
   *
   * These values are placed in the first byte of requests sent to the MiP
   * (and appear in the corresponding responses).  See the official
   * [MiP BLE
   * Protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md)
   * for the complete list.
   */
  static constexpr uint8_t MIP_CMD_GET_GESTURE_RADAR_MODE = 0x0D;
  static constexpr uint8_t MIP_CMD_SET_GESTURE_RADAR_MODE = 0x0C;
  static constexpr uint8_t MIP_CMD_GET_GESTURE_RESPONSE = 0x0A;

  /**
   * @brief Constructs the gesture subsystem manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Gesture(MiP& mip);

  /**
   * @brief Enables gesture detection mode on the MiP.
   *
   * Uses verified mode switching (command + read-back confirmation with retry).
   */
  void enable();

  /**
   * @brief Disables gesture detection mode.
   *
   * Uses verified mode switching (command + read-back confirmation with retry).
   */
  void disable();

  /**
   * @brief Checks whether gesture detection mode is currently active.
   *
   * @return true if gesture mode is enabled.
   */
  bool isEnabled();

  /**
   * @brief Returns the number of unread gesture events in the queue.
   *
   * Processes any pending serial data first to update the internal queue.
   *
   * @return Number of available gesture events.
   */
  uint8_t availableEvents();

  /**
   * @brief Reads the next available gesture event from the queue.
   *
   * Processes pending serial data first. Returns MIP_GESTURE_INVALID and sets
   * last error to MIP_ERROR_NO_EVENT if the queue is empty.
   *
   * @return The gesture event code.
   */
  MiPGesture readEvent();

  /**
   * @brief Checks whether both gesture and radar modes are disabled.
   *
   * @return true if both modes are off (i.e., in MIP_GESTURE_RADAR_DISABLED
   * state).
   */
  bool areGestureAndRadarModesDisabled();

  void processEvent(uint8_t gestureCode);

 protected:
  void clear();

 private:
  // Helper utilities for sub-functions
  void verifiedSet(MiPGestureMode desiredMode);
  bool check(MiPGestureMode expectedMode);
  void rawSet(MiPGestureMode mode);
  int8_t rawGet(MiPGestureMode& mode);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  CircularQueue<MiPGesture, 8> m_gestureEvents;

  friend class MiP;
};

#endif  // MPU_GESTURE_H
