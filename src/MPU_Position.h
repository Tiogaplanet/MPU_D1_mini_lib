/**
 * @file MPU_Position.h
 * @brief Defines the public interface for position reporting in the MPU
 * library.
 *
 * @details This header declares the position API used to inspect MiP's
 * orientation.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_POSITION_H
#define MPU_POSITION_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief MiP physical orientation or position states.
 */
enum MiPPosition : uint8_t {
  MIP_POSITION_ON_BACK = 0x00,
  MIP_POSITION_FACE_DOWN = 0x01,
  MIP_POSITION_UPRIGHT = 0x02,
  MIP_POSITION_PICKED_UP = 0x03,
  MIP_POSITION_HAND_STAND = 0x04,
  MIP_POSITION_FACE_DOWN_ON_TRAY = 0x05,
  MIP_POSITION_ON_BACK_WITH_KICKSTAND = 0x06,
};

/**
 * @brief Manages reporitng of MiP's position.
 */
class MiP_Position {
 public:
  /**
   * @brief Constructs the position manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Position(MiP& mip);

  /**
   * @brief Reads the current physical position or orientation of the MiP.
   *
   * Uses cached status data (updated automatically from OOB events).
   * No new serial request is sent to the robot.
   *
   * @return Current position as a MiPPosition enum value.
   */
  MiPPosition read();

  /**
   * @brief Checks if the MiP is lying on its back.
   *
   * @return true if on its back.
   */
  bool isOnBack();

  /**
   * @brief Checks if the MiP is face down.
   *
   * @return true if face down.
   */
  bool isFaceDown();

  /**
   * @brief Checks if the MiP is upright.
   *
   * @return true if upright.
   */
  bool isUpright();

  /**
   * @brief Checks if the MiP has been picked up.
   *
   * @return true if picked up.
   */
  bool isPickedUp();

  /**
   * @brief Checks if the MiP is in a hand-standing position.
   *
   * @return true if hand-standing.
   */
  bool isHandStanding();

  /**
   * @brief Checks if the MiP is face down on its tray.
   *
   * @return true if face down on tray.
   */
  bool isFaceDownOnTray();

  /**
   * @brief Checks if the MiP is on its back with the kickstand deployed.
   *
   * @return true if on back with kickstand.
   */
  bool isOnBackWithKickstand();

 private:
  MiP& m_mip;  // Stores a reference to the main MiP class.
};

#endif  // MPU_POSITION_H
