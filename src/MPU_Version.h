/**
 * @file MPU_Version.h
 * @brief Defines the functions for reporting MiP's software and hardware
 * versions.
 */
#ifndef MPU_VERSION_H
#define MPU_VERSION_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular includes.
class MiP;

/**
 * @brief Stores the MiP's software firmware version details.
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

 private:
  int8_t rawGetSoftware(MiPSoftwareVersion& software);
  int8_t rawGetHardware(MiPHardwareInfo& hardware);

  MiP& m_mip;  // Stores a reference to the main MiP class.
};

#endif  // MPU_VERSION_H
