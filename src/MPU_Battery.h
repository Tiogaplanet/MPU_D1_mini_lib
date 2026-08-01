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
 * @brief Manages MiP's battery monitoring.
 */
class MiP_Battery {
 public:
  /**
   * @brief Constructs the battery manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Battery(MiP& mip);

  /**
   * @brief Reads the current battery voltage of the MiP robot.
   * @return Battery voltage, typically 4.0V (low) to 6.4V (full).
   */
  float readVoltage();

 private:
  MiP& m_mip;  // Stores a reference to the main MiP class.
};

#endif  // MPU_BATTERY_H
