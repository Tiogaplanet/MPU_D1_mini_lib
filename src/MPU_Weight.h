/**
 * @file MPU_Weight.h
 *
 * @brief Declares the one and only function for reporting MiP's weight - read().
 *
 * @details This header declares the API used to report MiP's payload weight.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_WEIGHT_H
#define MPU_WEIGHT_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular includes.
class MiP;

/**
 * @brief Manages reading the MiP robot's weight sensor.
 */
class MiP_Weight {
 public:
  /**
   * @brief MiP protocol command bytes used by the weight detection subsystem.
   *
   * These values are placed in the first byte of requests sent to the MiP
   * (and appear in the corresponding responses).  See the official
   * [MiP BLE
   * Protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md)
   * for the complete list.
   */
  static constexpr uint8_t MIP_CMD_GET_WEIGHT = 0x81;

  /**
   * @brief Constructs the Weight manager.
   * @param mip A reference to the main MiP object for core services.
   */
  MiP_Weight(MiP& mip);

  void processEvent(int8_t weightValue);

  /**
   * @brief Reads the current weight on the MiP's weight sensor.
   *
   * Uses cached data from recent OOB events if available; otherwise performs
   * a verified read with automatic retries.
   *
   * @return Current weight in grams (signed), or 0 on error.
   */
  int8_t read();

 protected:
  void clear();

 private:
  int8_t rawGet(int8_t& weight);
  int8_t parse(int8_t& weight, const uint8_t response[], size_t responseLength);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  int8_t m_lastWeight;

  friend class MiP;
};

#endif  // MPU_WEIGHT_H
