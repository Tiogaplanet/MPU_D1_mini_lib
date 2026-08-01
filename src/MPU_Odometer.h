/**
 * @file MPU_Odometer.h
 * @brief Defines the public interface for odometer tracking in the MiP library.
 *
 * @details This header declares the odometer API used to read and reset
 * distance data.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_ODOMETER_H
#define MPU_ODOMETER_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Manages MiP's odometer monitoring.
 */
class MiP_Odometer {
 public:
  /**
   * @brief MiP protocol command bytes used by the odometer subsystem.
   *
   * These values are placed in the first byte of requests sent to the MiP
   * (and appear in the corresponding responses).  See the official
   * [MiP BLE
   * Protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md)
   * for the complete list.
   */
  static constexpr uint8_t MIP_CMD_READ_ODOMETER = 0x85;
  static constexpr uint8_t MIP_CMD_RESET_ODOMETER = 0x86;

  /**
   * @brief Constructs the odometer manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Odometer(MiP& mip);

  /**
   * @brief Reads the total distance travelled by the MiP.
   *
   * Performs a verified read with automatic retries on error.
   *
   * @return Distance in centimeters. Returns 0.0 on failure.
   */
  float read();

  /**
   * @brief Resets the odometer distance to zero.
   */
  void reset();

 private:
  int8_t rawRead(float& distanceInCm);

  MiP& m_mip;  // Stores a reference to the main MiP class.
};

#endif  // MPU_ODOMETER_H
