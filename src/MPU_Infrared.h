/**
 * @file MPU_Infrared.h
 * @brief Defines the public interface for infrared communication in the MiP
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
 * @brief Represents a 2-, 3-, or 4-byte IR dongle code.
 *
 * @details Encapsulates up to a 32-bit integer IR code payload alongside the
 * number of valid bytes (2, 3, or 4) contained within the code.
 */
struct MiPIRDongleCode {
  uint32_t code;   ///< Up to 32-bit IR code value.
  uint8_t length;  ///< Number of valid code bytes (2, 3, or 4).

  /**
   * @brief Constructs an uninitialized/invalid MiPIRDongleCode instance.
   *
   * @details Sets default code to 0xFFFFFFFF and length to 0.
   */
  MiPIRDongleCode() : code(0xFFFFFFFF), length(0) {}

  /**
   * @brief Constructs a MiPIRDongleCode instance with a specified code value
   * and length.
   *
   * @param c The raw numerical IR code value (up to 32 bits).
   * @param l The number of valid payload bytes (2, 3, or 4).
   */
  MiPIRDongleCode(uint32_t c, uint8_t l) : code(c), length(l) {}

  /**
   * @brief Checks whether the struct holds a valid IR dongle code length.
   *
   * @return true if length is between 2 and 4 bytes inclusive, false otherwise.
   */
  bool isValid() const {
    return length >= 2 && length <= 4;
  }
};

/**
 * @brief Manages MiP's infrared subsystem for robot detection and remote
 * control.
 */
class MiP_Infrared {
public:
  /**
   * @brief Enables MiP detection mode (allows detecting other MiPs via IR).
   *
   * @details Configures MiP to broadcast a unique ID over IR and listen for
   * other nearby MiPs.
   *
   * @param id       Unique ID for this MiP (used in detection events broadcast
   * to others).
   * @param txPower  Transmit power level (1-120).
   */
  void enableMiPDetectionMode(uint8_t id, uint8_t txPower);

  /**
   * @brief Disables MiP detection mode.
   *
   * @details Stops broadcasting MiP's ID over IR.
   */
  void disableMiPDetectionMode();

  /**
   * @brief Checks if MiP detection mode is currently enabled.
   *
   * @return true if detection mode is active (broadcast ID > 0), false
   * otherwise.
   */
  bool isMiPDetectionModeEnabled();

  /**
   * @brief Reads the next detected MiP event from the queue.
   *
   * @details Processes pending serial data first. Pops the oldest detected MiP
   * ID from the internal queue.
   *
   * @return ID of the detected MiP (1-255), or 0 if no event is available.
   */
  uint8_t readDetectedMiP();

  /**
   * @brief Returns the number of unread detected MiP events in the queue.
   *
   * @details Processes pending serial data first to update the queue before
   * checking.
   *
   * @return Number of available MiP detection events in the queue.
   */
  uint8_t availableDetectedMiPEvents();

  /**
   * @brief Enables IR remote control mode.
   *
   * @details Verified operation: sends the enable command and reads back state
   * with retry.
   */
  void enableRemoteControl();

  /**
   * @brief Disables IR remote control mode.
   *
   * @details Verified operation: sends the disable command and reads back state
   * with retry.
   */
  void disableRemoteControl();

  /**
   * @brief Checks if IR remote control mode is currently enabled.
   *
   * @return true if IR remote control mode is active, false otherwise.
   */
  bool isRemoteControlEnabled();

  /**
   * @brief Transmits a 2-, 3-, or 4-byte IR dongle code using a struct (fire
   * and forget).
   *
   * @details Transmits the IR payload stored in @p irCode at the specified
   * transmit power. No read-back verification is performed as there is no
   * feedback mechanism over IR.
   *
   * @param irCode        Struct containing code value and byte length (2, 3, or
   * 4).
   * @param transmitPower Transmit power level (1-120).
   */
  void sendDongleCode(const MiPIRDongleCode& irCode, uint8_t transmitPower);

  /**
   * @brief Transmits a variable-length IR dongle code using direct values (fire
   * and forget).
   *
   * @details Transmits the specified numerical @p code value at the given @p
   * length and @p transmitPower. No read-back verification is performed as
   * there is no feedback mechanism over IR.
   *
   * @param code          Up to 32-bit numerical IR code value.
   * @param length        Code length in bytes (2, 3, or 4).
   * @param transmitPower Transmit power level (1-120).
   */
  void sendDongleCode(uint32_t code, uint8_t length, uint8_t transmitPower);

  /**
   * @brief Reads the next received IR dongle code from the queue.
   *
   * @details Processes pending serial data first. Pops the oldest IR dongle
   * code from the queue.
   *
   * @return MiPIRDongleCode struct containing the code value and its byte
   * length. Returns an empty/invalid struct (length = 0, code = 0xFFFFFFFF) if
   * no event is available.
   */
  MiPIRDongleCode readDongleCode();

  /**
   * @brief Returns the number of unread IR dongle code events in the queue.
   *
   * @details Processes pending serial data first to update the queue before
   * checking.
   *
   * @return Number of available IR code events in the queue.
   */
  uint8_t availableCodeEvents();

  /**
   * @brief Handles incoming infrared OOB event notifications from the transport
   * layer.
   *
   * @details Called by MiP::dispatchEvent() when an IR detection or received
   * dongle code event arrives over UART. Parses the payload and pushes the
   * event into the appropriate queue.
   *
   * @param command Command byte indicating the specific IR notification type.
   * @param payload Pointer to the raw response buffer.
   * @param length  Length of the payload in bytes.
   */
  void processEvent(uint8_t command, const uint8_t* payload, size_t length);

protected:
  /**
   * @brief Protocol command byte received when an IR dongle code is detected.
   */
  static constexpr uint8_t MIP_CMD_RECEIVE_IR_DONGLE_CODE = 0x03;

  /**
   * @brief Protocol command byte received when a nearby MiP is detected via IR.
   */
  static constexpr uint8_t MIP_CMD_GET_DETECTED_MIP = 0x04;

  /**
   * @brief Protocol command byte to configure MiP detection transmission and
   * power.
   */
  static constexpr uint8_t MIP_CMD_SET_DETECTION_MODE = 0x0E;

  /**
   * @brief Protocol command byte to enable or disable IR remote control mode.
   */
  static constexpr uint8_t MIP_CMD_SET_IR_REMOTE_CONTROL = 0x10;

  /**
   * @brief Protocol command byte to query if IR remote control mode is active.
   */
  static constexpr uint8_t MIP_CMD_GET_IR_REMOTE_CONTROL = 0x11;

  /**
   * @brief Protocol command byte to transmit an IR dongle code.
   */
  static constexpr uint8_t MIP_CMD_SEND_IR_DONGLE_CODE = 0x8C;

  /**
   * @brief Special ID value used to disable MiP detection mode.
   */
  static constexpr uint8_t MIP_IR_DETECTION_MODE_DISABLE = 0;

  /**
   * @brief State value representing disabled IR remote control mode.
   */
  static constexpr uint8_t MIP_IR_REMOTE_CONTROL_DISABLE = 0;

  /**
   * @brief State value representing enabled IR remote control mode.
   */
  static constexpr uint8_t MIP_IR_REMOTE_CONTROL_ENABLE = 1;

  void clear();

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   *
   * @param mip Reference to the main MiP object to access core communication
   * services.
   */
  explicit MiP_Infrared(MiP& mip);

  void rawSetMiPDetectionMode(uint8_t id, uint8_t txPower);
  void verifiedRemoteControl(uint8_t desiredRemoteControlMode);
  int8_t rawGetRemoteControl(uint8_t& remoteControl);
  void rawSetRemoteControl(uint8_t remoteControl);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  uint8_t m_irId;
  mip_detail::CircularQueue<MiPIRDongleCode, 8> m_irCodeEvents;
  mip_detail::CircularQueue<uint8_t, 8> m_detectedMiPEvents;

  /**
   * @brief Allows MiP and transport components to access private constructor
   * and protected protocol bytes.
   */
  friend class MiP;
  friend class MiP_Serial;
};

#endif  // MPU_INFRARED_H
