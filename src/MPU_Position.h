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
  MIP_POSITION_ON_BACK = 0x00,     ///< MiP is lying on back position.
  MIP_POSITION_FACE_DOWN = 0x01,   ///< MiP is lying face down.
  MIP_POSITION_UPRIGHT = 0x02,     ///< MiP is standing upright in balance
                                   ///< position.
  MIP_POSITION_PICKED_UP = 0x03,   ///< MiP has been picked up off the ground.
  MIP_POSITION_HAND_STAND = 0x04,  ///< MiP is in a hand-standing position.
  MIP_POSITION_FACE_DOWN_ON_TRAY = 0x05,       ///< MiP is face down with tray
                                               ///< accessory attached.
  MIP_POSITION_ON_BACK_WITH_KICKSTAND = 0x06,  ///< MiP is lying on back with
                                               ///< kickstand deployed.
};

/**
 * @brief Manages reporting of MiP's physical orientation and stance position.
 */
class MiP_Position {
public:
  /**
   * @brief Reads MiP's current physical position or orientation.
   *
   * @details Uses cached status data (updated automatically from Out-Of-Band
   * status events). No new serial request is sent to the robot hardware.
   *
   * @return MiPPosition Current physical position as a MiPPosition enum value.
   *         Returns MIP_POSITION_ON_BACK_WITH_KICKSTAND if uninitialized.
   */
  MiPPosition read();

  /**
   * @brief Checks if MiP is lying on back position.
   *
   * @return true if MiP is lying on back, false otherwise.
   */
  bool isOnBack();

  /**
   * @brief Checks if MiP is face down.
   *
   * @return true if MiP is lying face down, false otherwise.
   */
  bool isFaceDown();

  /**
   * @brief Checks if MiP is upright.
   *
   * @return true if MiP is standing upright, false otherwise.
   */
  bool isUpright();

  /**
   * @brief Checks if MiP has been picked up.
   *
   * @return true if MiP is currently picked up off the surface, false
   * otherwise.
   */
  bool isPickedUp();

  /**
   * @brief Checks if MiP is in a hand-standing position.
   *
   * @return true if MiP is in a hand-stand position, false otherwise.
   */
  bool isHandStanding();

  /**
   * @brief Checks if MiP is face down on tray position.
   *
   * @return true if MiP is face down on tray, false otherwise.
   */
  bool isFaceDownOnTray();

  /**
   * @brief Checks if MiP is on back with the kickstand deployed.
   *
   * @return true if MiP is on back with kickstand deployed, false
   * otherwise.
   */
  bool isOnBackWithKickstand();

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   *
   * @param mip A reference to the main MiP object to access core services.
   */
  explicit MiP_Position(MiP& mip);

  MiP& m_mip;  // Stores a reference to the main MiP class.

  /**
   * @brief Allows MiP to call private constructor.
   */
  friend class MiP;
};

#endif  // MPU_POSITION_H
