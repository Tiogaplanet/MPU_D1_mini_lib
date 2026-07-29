/* Copyright (C) 2026  Samuel Trassare (https://github.com/Tiogaplanet)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/**
 * @file MPU_EEPROM.h
 * @brief Defines the functions to read and write to MiP's EEPROM.
 */
#ifndef MPU_EEPROM_H
#define MPU_EEPROM_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Manages access to MiP's EEPROM for reading and writing.
 */
class MiP_EEPROM {
 public:
  // MiP Protocol Commands related to MiP's EEPROM.
  // These command codes are placed in the first byte of requests sent to the
  // MiP and responses sent back from the MiP. See
  // https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md
  // for the complete list.
  static constexpr uint8_t MIP_CMD_SET_USER_DATA = 0x12;
  static constexpr uint8_t MIP_CMD_GET_USER_DATA = 0x13;

  // EEPROM base address.
  static constexpr uint8_t BASE_EEPROM_ADDRESS = 0x20;
  // Last addressable address in EEPROM.
  static constexpr uint8_t LAST_EEPROM_ADDRESS = 0x2F;

  /**
   * @brief Constructs the EEPROM manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_EEPROM(MiP& mip);

  /**
   * @brief Reads a byte from the MiP's user EEPROM area.
   *
   * Performs a verified read with retries on communication errors.
   *
   * @param addressOffset Offset from BASE_EEPROM_ADDRESS (0-15).
   * @return The stored byte value, or 0 on error.
   */
  uint8_t read(uint8_t addressOffset);

  /**
   * @brief Writes a byte to the MiP's user EEPROM area and verifies it.
   *
   * This function performs a verified write: it sends the data, reads it back,
   * and retries (up to MIP_MAX_RETRIES) if the value doesn't match or an error
   * occurs.
   *
   * @param addressOffset Offset from BASE_EEPROM_ADDRESS (0-15).
   * @param userData      Byte value to store (0-255).
   */
  void write(uint8_t addressOffset, uint8_t userData);

 private:
  int8_t rawRead(uint8_t address, uint8_t& userData);
  void rawWrite(uint8_t address, uint8_t userData);

  MiP& m_mip;  // Stores a reference to the main MiP class.
};

#endif  // MPU_EEPROM_H
