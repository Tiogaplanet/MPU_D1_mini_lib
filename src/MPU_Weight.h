/**
 * @file MPU_Weight.h
 * @brief Defines the public interface for reporting MiP's weight.
 *
 * @details This header declares the API used to query payload weight measured
 * by MiP.
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
#include <stddef.h>

// Forward-declare the main MiP class to avoid circular includes.
class MiP;

/**
 * @brief Manages reading weight data measured by MiP.
 */
class MiP_Weight {
public:
  /**
   * @brief Reads the current weight on MiP's weight sensor.
   *
   * @details Uses cached weight data from a recent Out-Of-Band status event if
   * available; otherwise performs a direct query with automatic retries on
   * communication error.
   *
   * @return int8_t Current payload weight in grams (signed), or 0 on error.
   */
  int8_t read();

  /**
   * @brief Calibrates or zeroes out MiP's baseline weight sensor offset (Tare)
   * and verifies the change.
   *
   * @details Verified method: sends the undocumented protocol command over UART to set a signed
   * baseline calibration offset in grams, then reads back the updated sensor
   * reading to confirm the new baseline matches expectations. Retries
   * automatically up to MIP_MAX_RETRIES on mismatch or error.
   *
   * Passing 0 zeroes out the current baseline weight reading (tare), which is
   * useful after attaching tray accessories or custom payloads.
   *
   * @param offset Signed calibration offset in grams (-128 to +127; default = 0).
   */
  void calibrate(int8_t offset = 0);

protected:
  /**
   * @brief Undocumented MiP protocol command byte to set weight sensor calibration offset.
   */
  static constexpr uint8_t MIP_CMD_SET_WEIGHT_CALIBRATION = 0x75;

  /**
   * @brief MiP protocol command byte used to request the current payload
   * weight.
   *
   * @details This value is placed in the first byte of requests sent to MiP,
   * and appears in corresponding responses.
   */
  static constexpr uint8_t MIP_CMD_GET_WEIGHT = 0x81;

  void clear();

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   *
   * @param mip Reference to the main MiP object for core communication
   * services.
   */
  explicit MiP_Weight(MiP& mip);

  int8_t rawGet(int8_t& weight);
  void rawCalibrate(int8_t offset);
  int8_t parse(int8_t& weight, const uint8_t response[], size_t responseLength);

  /**
   * @brief Handles an incoming weight event notification from the transport
   * layer.
   *
   * @details Called by MiP::dispatchEvent() when a MIP_CMD_GET_WEIGHT
   * notification arrives over UART. Caches the weight value and marks the
   * weight data as valid.
   *
   * @param weightValue Raw payload weight value in grams reported by MiP.
   */
  void processEvent(int8_t weightValue);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  int8_t m_lastWeight;

  /**
   * @brief Allows MiP and transport components to access private constructor
   * and protected protocol bytes.
   */
  friend class MiP;
  friend class MiP_Serial;
};

#endif  // MPU_WEIGHT_H
