/**
 * @file MPU_Version.h
 * @brief Defines the functions for reporting MiP's software and hardware
 *        versions and library version numbers.
 *
 * @details This header declares the API used to query MiP's firmware release
 * date, hardware revision, and voice sound chip version. It also provides
 * methods for querying the version of this Arduino library.
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

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Stores MiP's firmware build release date and unique version numbers.
 *
 * @details Encapsulates the 4-digit build year, 2-digit month, 2-digit day,
 * and internal unique version identifier returned by MiP's software version
 * query.
 */
class MiPSoftwareVersion {
public:
  /**
   * @brief Constructs a new MiPSoftwareVersion instance.
   *
   * @details Automatically calls clear() to set all version fields to zero.
   */
  MiPSoftwareVersion() {
    clear();
  }

  /**
   * @brief Resets all software version fields to zero.
   */
  void clear() {
    year = 0;
    month = 0;
    day = 0;
    uniqueVersion = 0;
  }

  uint16_t year;          ///< Firmware build year (e.g. 2014).
  uint8_t month;          ///< Firmware build month (1-12).
  uint8_t day;            ///< Firmware build day (1-31).
  uint8_t uniqueVersion;  ///< Internal unique version/revision number.
};

/**
 * @brief Stores MiP's hardware revision and voice chip hardware numbers.
 *
 * @details Encapsulates the voice sound chip revision and robot body
 * hardware revision numbers returned by MiP's hardware info query.
 */
class MiPHardwareInfo {
public:
  /**
   * @brief Constructs a new MiPHardwareInfo instance.
   *
   * @details Automatically calls clear() to set all revision fields to zero.
   */
  MiPHardwareInfo() {
    clear();
  }

  /**
   * @brief Resets hardware revision fields to zero.
   */
  void clear() {
    voiceChip = 0;
    hardware = 0;
  }

  uint8_t voiceChip;  ///< Voice chip hardware revision number.
  uint8_t hardware;   ///< Main body hardware revision number.
};

/**
 * @brief Manages reading MiP's hardware and software versions and library
 * versioning.
 */
class MiP_Version {
public:
  /**
   * @brief Reads MiP's software/firmware version details.
   *
   * @details Sends the get software version request over UART and populates
   * @p software with the year, month, day, and unique version identifier.
   * Performs retries automatically if communication errors occur.
   *
   * @param[out] software Reference to a MiPSoftwareVersion object to populate.
   */
  void readSoftware(MiPSoftwareVersion& software);

  /**
   * @brief Reads MiP's hardware and voice chip revision numbers.
   *
   * @details Sends the get hardware info request over UART and populates
   * @p hardware with the voice chip and main body hardware revision numbers.
   * Performs retries automatically if communication errors occur.
   *
   * @param[out] hardware Reference to a MiPHardwareInfo object to populate.
   */
  void readHardware(MiPHardwareInfo& hardware);

  /**
   * @brief Returns the Semantic Version string of this Arduino library.
   *
   * @return const char* Static C-string representation of the library version
   * (e.g. "2.0.1").
   */
  const char* readMPUString() const;

  /**
   * @brief Returns the encoded integer version number of this Arduino library.
   *
   * @details Encodes major, minor, and patch numbers into a single integer
   * suitable for preprocessor checks (e.g., version 2.0.1 is encoded as 20001).
   *
   * @return uint32_t Encoded library version number.
   */
  uint32_t readMPUNumber() const;

protected:
  /**
   * @brief Protocol command byte to query MiP's firmware software version date.
   */
  static constexpr uint8_t MIP_CMD_GET_SOFTWARE_VERSION = 0x14;

  /**
   * @brief Protocol command byte to query MiP's hardware and voice chip
   * revisions.
   */
  static constexpr uint8_t MIP_CMD_GET_HARDWARE_INFO = 0x19;

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   *
   * @param mip Reference to the main MiP object for core communication
   * services.
   */
  explicit MiP_Version(MiP& mip);

  int8_t rawGetSoftware(MiPSoftwareVersion& software);
  int8_t rawGetHardware(MiPHardwareInfo& hardware);

  MiP& m_mip;  // Stores a reference to the main MiP class.

  /**
   * @brief Allows MiP to call private constructor.
   */
  friend class MiP;
};

#endif  // MPU_VERSION_H
