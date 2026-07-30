/**
 * @file MPU_Shake.h
 * @brief Defines the public interface for shake detection in the MiP library.
 *
 * @details This header declares the shake-detection API used by the MiP
 * library.
 *
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may
 * obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_SHAKE_H
#define MPU_SHAKE_H

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Manages whether MiP has been shaken.
 */
class MiP_Shake {
 public:
  /**
   * @brief Constructs the shake manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Shake(MiP& mip);

  /**
   * @brief Checks whether the MiP has been shaken since the last call.
   *
   * Uses cached data from status events. The shake flag is cleared after
   * returning true (one-shot detection).
   *
   * @return true if a shake was detected since the last call to this function.
   */
  bool read();

 private:
  MiP& m_mip;  // Stores a reference to the main MiP class.
};

#endif  // MPU_SHAKE_H
