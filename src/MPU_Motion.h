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
  MIP_DRIVE_FORWARD = 0x00,   ///< Drive in the forward direction.
  MIP_DRIVE_BACKWARD = 0x01,  ///< Drive in the backward direction.
};

/**
 * @brief Turn direction for distanceDrive().
 */
enum MiPTurnDirection : uint8_t {
  MIP_TURN_LEFT = 0x00,  ///< Turn toward the left.
  MIP_TURN_RIGHT = 0x01  ///< Turn toward the right.
};

/**
 * @brief Fall direction for internal fall commands.
 */
enum MiPFallDirection : uint8_t {
  MIP_FALL_ON_BACK = 0x00,   ///< Command MiP to fall onto back position.
  MIP_FALL_FACE_DOWN = 0x01  ///< Command MiP to fall face down.
};

/**
 * @brief Get-up direction options.
 */
enum MiPGetUp : uint8_t {
  MIP_GETUP_FROM_FRONT = 0x00,  ///< Attempt self-righting stand from face-down
                                ///< position.
  MIP_GETUP_FROM_BACK = 0x01,   ///< Attempt self-righting stand from back
                                ///< position.
  MIP_GETUP_FROM_EITHER = 0x02  ///< Automatically detect orientation and
                                ///< attempt stand from either side.
};

/**
 * @brief Manages MiP's drive, turn, posture, and balance systems.
 */
class MiP_Motion {
public:
  /**
   * @brief Sends continuous drive command (velocity + turn rate).
   *
   * @details Rate-limited internally to avoid overwhelming MiP's UART
   * interface (~20 Hz max). Fire-and-forget command (no read-back verification
   * possible).
   *
   * @param velocity  Speed/direction (-32 to +32; negative = backward, positive
   * = forward).
   * @param turnRate  Turn rate/direction (-32 to +32; negative = left, positive
   * = right).
   */
  void continuousDrive(int8_t velocity, int8_t turnRate);

  /**
   * @brief Drives a specific distance then optionally turns.
   *
   * @details Fire-and-forget command.
   *
   * @param driveDirection Direction to drive (MIP_DRIVE_FORWARD or
   * MIP_DRIVE_BACKWARD).
   * @param cm             Distance to travel in centimeters (0-255 cm).
   * @param turnDirection  Turn direction after driving (MIP_TURN_LEFT or
   * MIP_TURN_RIGHT).
   * @param degrees        Turn angle in degrees (0-360 degrees).
   */
  void distanceDrive(MiPDriveDirection driveDirection, uint8_t cm,
                     MiPTurnDirection turnDirection, uint16_t degrees);

  /**
   * @brief Drives forward for a limited time at given speed.
   *
   * @details Fire-and-forget command. Duration parameter is internally
   * converted to 7ms hardware ticks (max duration ~1.78 seconds / 1785 ms).
   *
   * @param speed Drive speed (0-30).
   * @param time  Duration in milliseconds (max 1785 ms).
   */
  void driveForward(uint8_t speed, uint16_t time);

  /**
   * @brief Drives backward for a limited time at given speed.
   *
   * @details Fire-and-forget command. Duration parameter is internally
   * converted to 7ms hardware ticks (max duration ~1.78 seconds / 1785 ms).
   *
   * @param speed Drive speed (0-30).
   * @param time  Duration in milliseconds (max 1785 ms).
   */
  void driveBackward(uint8_t speed, uint16_t time);

  /**
   * @brief Turns left by the specified angle at given speed.
   *
   * @details Fire-and-forget command. Angle parameter is internally converted
   * to 5° hardware units (max angle 1275°).
   *
   * @param degrees Turn angle in degrees (max 1275°).
   * @param speed   Turn speed (0-24).
   */
  void turnLeft(uint16_t degrees, uint8_t speed);

  /**
   * @brief Turns right by the specified angle at given speed.
   *
   * @details Fire-and-forget command. Angle parameter is internally converted
   * to 5° hardware units (max angle 1275°).
   *
   * @param degrees Turn angle in degrees (max 1275°).
   * @param speed   Turn speed (0-24).
   */
  void turnRight(uint16_t degrees, uint8_t speed);

  /**
   * @brief Stops all motor motion immediately.
   *
   * @details Fire-and-forget command.
   */
  void stop();

  /**
   * @brief Commands MiP to fall forward onto face position.
   */
  void fallForward();

  /**
   * @brief Commands MiP to fall backward onto back position.
   */
  void fallBackward();

  /**
   * @brief Commands MiP to stand up from a fallen position.
   *
   * @param getup Which direction/side to attempt getting up from (default =
   * MIP_GETUP_FROM_EITHER).
   */
  void getUp(MiPGetUp getup = MIP_GETUP_FROM_EITHER);

protected:
  /**
   * @brief MiP protocol command byte for continuous drive (velocity + turn
   * rate).
   */
  static constexpr uint8_t MIP_CMD_CONTINUOUS_DRIVE = 0x78;

  /**
   * @brief MiP protocol command byte to drive a specific distance and angle.
   */
  static constexpr uint8_t MIP_CMD_DISTANCE_DRIVE = 0x70;

  /**
   * @brief MiP protocol command byte to turn left by a specific angle and
   * speed.
   */
  static constexpr uint8_t MIP_CMD_TURN_LEFT = 0x73;

  /**
   * @brief MiP protocol command byte to turn right by a specific angle and
   * speed.
   */
  static constexpr uint8_t MIP_CMD_TURN_RIGHT = 0x74;

  /**
   * @brief MiP protocol command byte to drive forward for a timed duration.
   */
  static constexpr uint8_t MIP_CMD_DRIVE_FORWARD = 0x71;

  /**
   * @brief MiP protocol command byte to drive backward for a timed duration.
   */
  static constexpr uint8_t MIP_CMD_DRIVE_BACKWARD = 0x72;

  /**
   * @brief MiP protocol command byte to immediately stop all motor motion.
   */
  static constexpr uint8_t MIP_CMD_STOP = 0x77;

  /**
   * @brief MiP protocol command byte to force MiP to fall down in a specified
   * direction.
   */
  static constexpr uint8_t MIP_CMD_SET_POSITION = 0x08;

  /**
   * @brief MiP protocol command byte to command MiP to stand up from a fallen
   * position.
   */
  static constexpr uint8_t MIP_CMD_GET_UP = 0x23;

  /**
   * @brief Minimum inter-command delay in milliseconds for continuous drive
   * commands (~20 Hz).
   */
  static constexpr uint8_t MIP_CONTINUOUS_DRIVE_DELAY = 50;

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   *
   * @param mip A reference to the main MiP object to access core communication
   * services.
   */
  explicit MiP_Motion(MiP& mip);

  void fallDown(MiPFallDirection direction);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  uint32_t m_lastContinuousDriveTime;

  /**
   * @brief Allows MiP to call private constructor.
   */
  friend class MiP;
};

#endif  // MPU_MOTION_H
