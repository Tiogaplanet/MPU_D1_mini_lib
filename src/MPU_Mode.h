/**
 * @file MPU_Mode.h
 * @brief Defines the public interface for mode switching in the MiP library.
 *
 * @details This header declares the mode-selection API used to change MiP
 * behavior.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_MODE_H
#define MPU_MODE_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Game and App operating modes for MiP.
 */
enum MiPGameMode : uint8_t {
  MIP_APP_MODE = 0x01,       ///< App Mode (default for external control via
                             ///< UART/Bluetooth).
  MIP_CAGE_MODE = 0x02,      ///< Cage Game Mode (robot stays inside an enclosed
                             ///< virtual cage).
  MIP_TRACKING_MODE = 0x03,  ///< Tracking Mode (robot tracks and follows
                             ///< objects/hands).
  MIP_DANCE_MODE = 0x04,     ///< Dance Mode (robot performs built-in dance
                             ///< routines).
  MIP_DEFAULT_MODE = 0x05,   ///< Default Operating Mode.
  MIP_STACK_MODE = 0x06,     ///< Stack Game Mode (balancing objects on MiP's
                             ///< tray).
  MIP_TRICK_MODE = 0x07,  ///< Trick Programming Mode (executing learned gesture
                          ///< sequences).
  MIP_ROAM_MODE = 0x08,   ///< Roam Mode (autonomous obstacle avoidance
                          ///< navigation).
};

/**
 * @brief Manages MiP's built-in game and application operating modes.
 */
class MiP_Mode {
public:
  /**
   * @brief Switches MiP into App Mode.
   *
   * @details Verified operation: sends the set game mode command and reads back
   * state with retry.
   */
  void enableApp();

  /**
   * @brief Switches MiP into Cage Mode.
   *
   * @details Verified operation: sends the set game mode command and reads back
   * state with retry.
   */
  void enableCage();

  /**
   * @brief Switches MiP into Dance Mode.
   *
   * @details Verified operation: sends the set game mode command and reads back
   * state with retry.
   */
  void enableDance();

  /**
   * @brief Switches MiP into Stack Mode.
   *
   * @details Verified operation: sends the set game mode command and reads back
   * state with retry.
   */
  void enableStack();

  /**
   * @brief Switches MiP into Trick Mode.
   *
   * @details Verified operation: sends the set game mode command and reads back
   * state with retry.
   */
  void enableTrick();

  /**
   * @brief Switches MiP into Roam Mode.
   *
   * @details Verified operation: sends the set game mode command and reads back
   * state with retry.
   */
  void enableRoam();

  /**
   * @brief Checks if App Mode is currently active.
   *
   * @return true if MiP is in App Mode, false otherwise.
   */
  bool isAppEnabled();

  /**
   * @brief Checks if Cage Mode is currently active.
   *
   * @return true if MiP is in Cage Mode, false otherwise.
   */
  bool isCageEnabled();

  /**
   * @brief Checks if Dance Mode is currently active.
   *
   * @return true if MiP is in Dance Mode, false otherwise.
   */
  bool isDanceEnabled();

  /**
   * @brief Checks if Stack Mode is currently active.
   *
   * @return true if MiP is in Stack Mode, false otherwise.
   */
  bool isStackEnabled();

  /**
   * @brief Checks if Trick Mode is currently active.
   *
   * @return true if MiP is in Trick Mode, false otherwise.
   */
  bool isTrickEnabled();

  /**
   * @brief Checks if Roam Mode is currently active.
   *
   * @return true if MiP is in Roam Mode, false otherwise.
   */
  bool isRoamEnabled();

protected:
  /**
   * @brief MiP protocol command byte to configure MiP's active game/app
   * operating mode.
   */
  static constexpr uint8_t MIP_CMD_SET_GAME_MODE = 0x76;

  /**
   * @brief MiP protocol command byte to query MiP's active game/app operating
   * mode.
   */
  static constexpr uint8_t MIP_CMD_GET_GAME_MODE = 0x82;

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   *
   * @param mip A reference to the main MiP object to access core communication
   * services.
   */
  explicit MiP_Mode(MiP& mip);

  void verifiedSet(MiPGameMode desiredMode);
  bool check(MiPGameMode expectedMode);
  void rawSet(MiPGameMode mode);
  int8_t rawGet(MiPGameMode& mode);

  MiP& m_mip;  // Stores a reference to the main MiP class.

  /**
   * @brief Allows MiP to call private constructor.
   */
  friend class MiP;
};

#endif  // MPU_MODE_H
