/* Copyright (C) 2026  Samuel Trassare (https://github.com/Tiogaplanet)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/**
 * @file MPU_Shake.h
 * @brief Defines the one and only function to report whether MiP has been
 * shaken.
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
