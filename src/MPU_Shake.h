/**
 * @file MPU_Shake.h
 * @brief Defines the public interface for shake detection in the MiP library.
 *
 * @details This header declares the shake-detection API used by the MPU
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
#ifndef MPU_SHAKE_H
#define MPU_SHAKE_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Manages MiP's shake event detection system.
 *
 * @details Monitors physical shake events detected by MiP's internal
 * accelerometers and maintains internal state flags for one-shot event reading.
 */
class MiP_Shake {
public:
  /**
   * @brief Checks whether MiP has been shaken since the last call.
   *
   * @details Flushes pending serial transport data, inspects the internal shake
   * flag, and clears the flag after returning true (one-shot detection).
   *
   * @return true if a shake was detected since the last call, false otherwise.
   */
  bool read();

protected:
  /**
   * @brief MiP protocol notification byte received when a physical shake event
   * occurs.
   *
   * @details Dispatched asynchronously by MiP over UART as an Out-Of-Band (OOB)
   * notification when the robot experiences a physical shake motion.
   */
  static constexpr uint8_t MIP_CMD_SHAKE_RESPONSE = 0x1A;

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   *
   * @param mip A reference to the main MiP object to access core services and
   * status flags.
   */
  explicit MiP_Shake(MiP& mip);

  MiP& m_mip;  // Stores a reference to the main MiP class.

  /**
   * @brief Allows MiP and transport components to access private constructor
   * and protected protocol bytes.
   */
  friend class MiP;
  friend class MiP_Serial;
};

#endif  // MPU_SHAKE_H
