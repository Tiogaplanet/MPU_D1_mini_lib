/**
 * @file MPU_Battery.h
 * @brief Defines the public interface for battery monitoring in the MPU
 * library.
 *
 * @details This header declares the battery-monitoring API used by the MPU
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
#ifndef MPU_BATTERY_H
#define MPU_BATTERY_H

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief MiP_Battery provides one function only, to report MiP's cached battery
 * voltage.
 */
class MiP_Battery {
public:
  /**
   * @brief Reads MiP's cached battery voltage.
   *
   * @details Processes any pending Out-Of-Band status events to update internal
   * cached voltage data without transmitting a new request to MiP. The cached
   * value is updated from periodic status updates.
   *
   * @return float Battery voltage in Volts, typically 4.0V (low) to 6.4V (fully
   * charged). Returns 0.0f if MiP is uninitialized.
   */
  float readVoltage();

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   * @param mip A reference to the main MiP object to access core services.
   */
  explicit MiP_Battery(MiP& mip);

  /**
   * @brief A private variable that stores a reference to the main MiP class.
   */
  MiP& m_mip;

  /**
   * @brief Allows MiP to call private constructor.
   */
  friend class MiP;
};

#endif  // MPU_BATTERY_H
