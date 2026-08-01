/**
 * @file MPU_Motion.h
 * @brief Defines the public interface for motion control in the MiP library.
 *
 * @details This header declares the motion API used to drive, turn, and
 * stabilize MiP.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_MOTION_H
#define MPU_MOTION_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Drive direction for distanceDrive().
 */
enum MiPDriveDirection : uint8_t {
  MIP_DRIVE_FORWARD = 0x00,
  MIP_DRIVE_BACKWARD = 0x01,
};

/**
 * @brief Turn direction for distanceDrive().
 */
enum MiPTurnDirection : uint8_t { MIP_TURN_LEFT = 0x00, MIP_TURN_RIGHT = 0x01 };

/**
 * @brief Fall direction for internal fall commands.
 */
enum MiPFallDirection : uint8_t {
  MIP_FALL_ON_BACK = 0x00,
  MIP_FALL_FACE_DOWN = 0x01
};

/**
 * @brief Get-up direction options.
 */
enum MiPGetUp : uint8_t {
  MIP_GETUP_FROM_FRONT = 0x00,
  MIP_GETUP_FROM_BACK = 0x01,
  MIP_GETUP_FROM_EITHER = 0x02
};

/**
 * @brief Manages MiP's drive system.
 */
class MiP_Motion {
 public:
  /**
   * @brief MiP protocol command bytes used by the motion subsystem.
   *
   * These values are placed in the first byte of requests sent to the MiP
   * (and appear in the corresponding responses).  See the official
   * [MiP BLE
   * Protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md)
   * for the complete list.
   */
  static constexpr uint8_t MIP_CMD_CONTINUOUS_DRIVE = 0x78;
  static constexpr uint8_t MIP_CMD_DISTANCE_DRIVE = 0x70;
  static constexpr uint8_t MIP_CMD_TURN_LEFT = 0x73;
  static constexpr uint8_t MIP_CMD_TURN_RIGHT = 0x74;
  static constexpr uint8_t MIP_CMD_DRIVE_FORWARD = 0x71;
  static constexpr uint8_t MIP_CMD_DRIVE_BACKWARD = 0x72;
  static constexpr uint8_t MIP_CMD_STOP = 0x77;
  static constexpr uint8_t MIP_CMD_SET_POSITION = 0x08;
  static constexpr uint8_t MIP_CMD_GET_UP = 0x23;

  static constexpr uint8_t MIP_CONTINUOUS_DRIVE_DELAY = 50;

  /**
   * @brief Constructs the drive system manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Motion(MiP& mip);

  /**
   * @brief Sends continuous drive command (velocity + turn rate).
   *
   * Rate-limited internally to avoid overwhelming the MiP (~20 Hz max).
   * Fire-and-forget (no verification possible).
   *
   * @param velocity  -32 to +32 (negative = backward).
   * @param turnRate  -32 to +32 (negative = left).
   */
  void continuousDrive(int8_t velocity, int8_t turnRate);

  /**
   * @brief Drives a specific distance then optionally turns.
   *
   * Fire-and-forget command.
   *
   * @param driveDirection Forward or backward.
   * @param cm             Distance in centimeters.
   * @param turnDirection  Turn direction after driving.
   * @param degrees        Turn angle in degrees (0-360).
   */
  void distanceDrive(MiPDriveDirection driveDirection,
                     uint8_t cm,
                     MiPTurnDirection turnDirection,
                     uint16_t degrees);

  /**
   * @brief Drives forward for a limited time at given speed.
   *
   * Fire-and-forget. Time is internally converted to 7ms units.
   *
   * @param speed Drive speed (0-30).
   * @param time  Duration in milliseconds (max ~1.78 seconds).
   */
  void driveForward(uint8_t speed, uint16_t time);

  /**
   * @brief Drives backward for a limited time at given speed.
   *
   * Fire-and-forget. Time is internally converted to 7ms units.
   *
   * @param speed Drive speed (0-30).
   * @param time  Duration in milliseconds (max ~1.78 seconds).
   */
  void driveBackward(uint8_t speed, uint16_t time);

  /**
   * @brief Turns left by the specified angle at given speed.
   *
   * Fire-and-forget. Angle is internally converted to 5° units.
   *
   * @param degrees Turn angle (max 1275°).
   * @param speed   Turn speed (0-24).
   */
  void turnLeft(uint16_t degrees, uint8_t speed);

  /**
   * @brief Turns right by the specified angle at given speed.
   *
   * Fire-and-forget. Angle is internally converted to 5° units.
   *
   * @param degrees Turn angle (max 1275°).
   * @param speed   Turn speed (0-24).
   */
  void turnRight(uint16_t degrees, uint8_t speed);

  /**
   * @brief Stops all motion.
   *
   * Fire-and-forget.
   */
  void stop();

  /**
   * @brief Commands MiP to fall forward (face down).
   */
  void fallForward();

  /**
   * @brief Commands MiP to fall backward (on its back).
   */
  void fallBackward();

  /**
   * @brief Commands MiP to get up from a fallen position.
   *
   * @param getup Which way to attempt getting up (default = either side).
   */
  void getUp(MiPGetUp getup = MIP_GETUP_FROM_EITHER);

 private:
  void fallDown(MiPFallDirection direction);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  uint32_t m_lastContinuousDriveTime;
};

#endif  // MPU_MOTION_H
