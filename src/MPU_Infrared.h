/**
 * @file MPU_Infrared.h
 * @brief Defines the public interface for infrared communication in the MPU
 * library.
 *
 * @details This header declares the infrared API used for MiP detection and
 * remote control.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_INFRARED_H
#define MPU_INFRARED_H

#include <Arduino.h>

#include "MPU_Queue.h"

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Manages MiP's infrared system.
 */
class MiP_Infrared {
 public:
  /**
   * @brief MiP protocol command bytes used by the infrared subsystem.
   *
   * These values are placed in the first byte of requests sent to the MiP
   * (and appear in the corresponding responses).  See the official
   * [MiP BLE
   * Protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md)
   * for the complete list.
   */
  static constexpr uint8_t MIP_CMD_RECEIVE_IR_DONGLE_CODE = 0x03;
  static constexpr uint8_t MIP_CMD_GET_DETECTED_MIP = 0x04;
  static constexpr uint8_t MIP_CMD_SET_DETECTION_MODE = 0x0E;
  static constexpr uint8_t MIP_CMD_SET_IR_REMOTE_CONTROL = 0x10;
  static constexpr uint8_t MIP_CMD_GET_IR_REMOTE_CONTROL = 0x11;
  static constexpr uint8_t MIP_CMD_SEND_IR_DONGLE_CODE = 0x8C;

  /**
   * @brief Infrared mode definitions.
   */
  static constexpr uint8_t MIP_IR_DETECTION_MODE_DISABLE = 0;
  static constexpr uint8_t MIP_IR_REMOTE_CONTROL_DISABLE = 0;
  static constexpr uint8_t MIP_IR_REMOTE_CONTROL_ENABLE = 1;

  /**
   * @brief Constructs the infrared manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Infrared(MiP& mip);

  /**
   * @brief Enables MiP detection mode (allows detecting other MiPs via IR).
   *
   * @param id       Unique ID for this MiP (used in detection events).
   * @param txPower  Transmit power level (1-120).
   */
  void enableMiPDetectionMode(uint8_t id, uint8_t txPower);

  /**
   * @brief Disables MiP detection mode.
   */
  void disableMiPDetectionMode();

  /**
   * @brief Checks if MiP detection mode is currently enabled.
   *
   * @return true if detection mode is active.
   */
  bool isMiPDetectionModeEnabled();

  /**
   * @brief Reads the next detected MiP event.
   *
   * Processes pending serial data first.
   *
   * @return ID of the detected MiP, or 0 if none available.
   */
  uint8_t readDetectedMiP();

  /**
   * @brief Returns the number of unread detected MiP events.
   *
   * Processes pending serial data first.
   *
   * @return Number of available detection events.
   */
  uint8_t availableDetectedMiPEvents();

  /**
   * @brief Enables IR remote control mode.
   *
   * Verified operation (command + read-back with retry).
   */
  void enableRemoteControl();

  /**
   * @brief Disables IR remote control mode.
   *
   * Verified operation (command + read-back with retry).
   */
  void disableRemoteControl();

  /**
   * @brief Checks if IR remote control mode is enabled.
   *
   * @return true if IR remote control is active.
   */
  bool isRemoteControlEnabled();

  /**
   * @brief Sends an IR dongle code (fire-and-forget).
   *
   * No verification is performed as there is no reliable feedback mechanism.
   *
   * @param sendCode      16-bit IR code to transmit.
   * @param transmitPower Transmit power level.
   */
  void sendDongleCode(uint16_t sendCode, uint8_t transmitPower);

  /**
   * @brief Reads the next received IR dongle code event.
   *
   * Processes pending serial data first.
   *
   * @return The 32-bit IR code, or 0xFFFFFFFF if none available.
   */
  uint32_t readDongleCode();

  /**
   * @brief Returns the number of unread IR dongle code events.
   *
   * Processes pending serial data first.
   *
   * @return Number of available IR code events.
   */
  uint8_t availableCodeEvents();

  void processEvent(uint8_t command, const uint8_t* payload, size_t length);

 protected:
  void clear();

 private:
  void rawSetMiPDetectionMode(uint8_t id, uint8_t txPower);
  void verifiedRemoteControl(uint8_t desiredRemoteControlMode);
  int8_t rawGetRemoteControl(uint8_t& remoteControl);
  void rawSetRemoteControl(uint8_t remoteControl);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  uint8_t m_irId;
  CircularQueue<uint32_t, 8> m_irCodeEvents;
  CircularQueue<uint8_t, 8> m_detectedMiPEvents;

  friend class MiP;
};

#endif  // MPU_INFRARED_H
