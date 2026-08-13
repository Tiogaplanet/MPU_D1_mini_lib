/**
 * @file MPU_EEPROM.h
 * @brief Defines the public interface for EEPROM access in the MiP library.
 *
 * @details This header declares the EEPROM read and write API used by the MiP
 * library.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_EEPROM_H
#define MPU_EEPROM_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Manages access to MiP's non-volatile user EEPROM memory.
 */
class MiP_EEPROM {
public:
  /**
   * @brief Base starting address of MiP's user EEPROM storage area (0x20).
   */
  static constexpr uint8_t BASE_EEPROM_ADDRESS = 0x20;

  /**
   * @brief Last addressable byte location in MiP's user EEPROM storage area
   * (0x2F).
   */
  static constexpr uint8_t LAST_EEPROM_ADDRESS = 0x2F;

  /**
   * @brief Reads a byte from MiP's EEPROM storage area.
   *
   * @details Performs a verified read with automatic retries on communication
   * errors.
   *
   * @param addressOffset Offset from BASE_EEPROM_ADDRESS (0-15).
   * @return uint8_t The stored byte value, or 0 on communication error.
   */
  uint8_t read(uint8_t addressOffset);

  /**
   * @brief Writes a byte to MiP's EEPROM area and verifies it.
   *
   * @details Performs a verified write: sends the byte data, reads it back,
   * and retries automatically if the read-back value fails to match or an error
   * occurs.
   *
   * @param addressOffset Offset from BASE_EEPROM_ADDRESS (0-15).
   * @param userData      Byte value to store (0-255).
   */
  void write(uint8_t addressOffset, uint8_t userData);

protected:
  /**
   * @brief MiP protocol command byte to write user data to EEPROM.
   */
  static constexpr uint8_t MIP_CMD_SET_USER_DATA = 0x12;

  /**
   * @brief MiP protocol command byte to read user data from EEPROM.
   */
  static constexpr uint8_t MIP_CMD_GET_USER_DATA = 0x13;

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   *
   * @param mip A reference to the main MiP object to access core services.
   */
  explicit MiP_EEPROM(MiP& mip);

  int8_t rawRead(uint8_t address, uint8_t& userData);
  void rawWrite(uint8_t address, uint8_t userData);

  MiP& m_mip;  // Stores a reference to the main MiP class.

  /**
   * @brief Allows MiP to call private constructor.
   */
  friend class MiP;
};

#endif  // MPU_EEPROM_H
