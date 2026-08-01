/**
 * @file MPU_Version.h
 *
 * @brief Defines the functions for reporting MiP's software and hardware
 * versions.
 *
 * @details This header declares the API used to report MiP's hardware and
 * software versions. It also declares the functions for reporting the MPU
 * library version.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_VERSION_H
#define MPU_VERSION_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular includes.
class MiP;

/**
 * @brief Stores the MiP's software version details.
 */
class MiPSoftwareVersion {
 public:
  MiPSoftwareVersion() {
    clear();
  }
  void clear() {
    year = 0;
    month = 0;
    day = 0;
    uniqueVersion = 0;
  }
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t uniqueVersion;
};

/**
 * @brief Stores the MiP's hardware and voice chip revision numbers.
 */
class MiPHardwareInfo {
 public:
  MiPHardwareInfo() {
    clear();
  }
  void clear() {
    voiceChip = 0;
    hardware = 0;
  }
  uint8_t voiceChip;
  uint8_t hardware;
};

/**
 * @brief Manages reading the MiP robot's hardware and software versions.
 */
class MiP_Version {
 public:
  /**
   * @brief MiP protocol command bytes used by the version tracking subsystem.
   *
   * These values are placed in the first byte of requests sent to the MiP
   * (and appear in the corresponding responses).  See the official
   * [MiP BLE
   * Protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md)
   * for the complete list.
   */
  static constexpr uint8_t MIP_CMD_GET_SOFTWARE_VERSION = 0x14;
  static constexpr uint8_t MIP_CMD_GET_HARDWARE_INFO = 0x19;

  /**
   * @brief Constructs the Version manager.
   * @param mip A reference to the main MiP object for core services.
   */
  MiP_Version(MiP& mip);

  /**
   * @brief Reads the MiP's software version information.
   * @param software Reference to a struct to be filled with version data.
   */
  void readSoftware(MiPSoftwareVersion& software);

  /**
   * @brief Reads the MiP's hardware information.
   * @param hardware Reference to a struct to be filled with hardware data.
   */
  void readHardware(MiPHardwareInfo& hardware);

  /**
   * @brief Returns the semver string of this Arduino library (e.g. "2.0.0").
   */
  const char* readMPUString() const;

  /**
   * @brief Returns the encoded integer version number (e.g. 20000 for 2.0.0).
   */
  uint32_t readMPUNumber() const;

 private:
  int8_t rawGetSoftware(MiPSoftwareVersion& software);
  int8_t rawGetHardware(MiPHardwareInfo& hardware);

  MiP& m_mip;  // Stores a reference to the main MiP class.
};

#endif  // MPU_VERSION_H
