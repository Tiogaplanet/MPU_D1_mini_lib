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

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Game and App modes.
 */
enum MiPGameMode : uint8_t {
  MIP_APP_MODE = 0x01,
  MIP_CAGE_MODE = 0x02,
  MIP_TRACKING_MODE = 0x03,
  MIP_DANCE_MODE = 0x04,
  MIP_DEFAULT_MODE = 0x05,
  MIP_STACK_MODE = 0x06,
  MIP_TRICK_MODE = 0x07,
  MIP_ROAM_MODE = 0x08
};

/**
 * @brief Manages MiP's modes.
 */
class MiP_Mode {
 public:
  /**
   * @brief MiP protocol command bytes used by the mode subsystem.
   *
   * These values are placed in the first byte of requests sent to the MiP
   * (and appear in the corresponding responses).  See the official
   * [MiP BLE
   * Protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md)
   * for the complete list.
   */
  static constexpr uint8_t MIP_CMD_SET_GAME_MODE = 0x76;
  static constexpr uint8_t MIP_CMD_GET_GAME_MODE = 0x82;

  /**
   * @brief Constructs the mode manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Mode(MiP& mip);

  /**
   * @brief Switches MiP into App Mode.
   *
   * Verified operation (command sent + state read back with retry).
   */
  void enableApp();

  /**
   * @brief Switches MiP into Cage Mode.
   *
   * Verified operation (command sent + state read back with retry).
   */
  void enableCage();

  /**
   * @brief Switches MiP into Dance Mode.
   *
   * Verified operation (command sent + state read back with retry).
   */
  void enableDance();

  /**
   * @brief Switches MiP into Stack Mode.
   *
   * Verified operation (command sent + state read back with retry).
   */
  void enableStack();

  /**
   * @brief Switches MiP into Trick Mode.
   *
   * Verified operation (command sent + state read back with retry).
   */
  void enableTrick();

  /**
   * @brief Switches MiP into Roam Mode.
   *
   * Verified operation (command sent + state read back with retry).
   */
  void enableRoam();

  /**
   * @brief Checks if App Mode is currently active.
   *
   * @return true if in App Mode.
   */
  bool isAppEnabled();

  /**
   * @brief Checks if Cage Mode is currently active.
   *
   * @return true if in Cage Mode.
   */
  bool isCageEnabled();

  /**
   * @brief Checks if Dance Mode is currently active.
   *
   * @return true if in Dance Mode.
   */
  bool isDanceEnabled();

  /**
   * @brief Checks if Stack Mode is currently active.
   *
   * @return true if in Stack Mode.
   */
  bool isStackEnabled();

  /**
   * @brief Checks if Trick Mode is currently active.
   *
   * @return true if in Trick Mode.
   */
  bool isTrickEnabled();

  /**
   * @brief Checks if Roam Mode is currently active.
   *
   * @return true if in Roam Mode.
   */
  bool isRoamEnabled();

 private:
  void verifiedSet(MiPGameMode desiredMode);
  bool check(MiPGameMode expectedMode);
  void rawSet(MiPGameMode mode);
  int8_t rawGet(MiPGameMode& mode);

  MiP& m_mip;  // Stores a reference to the main MiP class.
};

#endif  // MPU_MODE_H
