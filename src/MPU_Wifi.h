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
 * @file MPU_Wifi.h
 * @brief Defines how the MPU, and thus MiP, interfaces with wifi.
 */
#ifndef MPU_WIFI_H
#define MPU_WIFI_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Manages MiP's wifi capability.
 */
class MiP_Wifi {
 public:
  /**
   * @brief Constructs the wifi manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Wifi(MiP& mip);

  /**
   * @brief Wraps WiFi.begin().  Connects to a wireless access point and loads
   * the MPU:D1 mini with OTA programming support.
   *
   * @param ssid The station ID.
   * @param password The access point's connection password.
   * @param hostname MiP's hostname on the wireless network.
   * @return WL_CONNECTED if the connection attempt was successful, else it
   * returns the error code from WiFi.connect().
   */
  uint8_t begin(const char* ssid, const char* password, const char* hostname);

  /**
   * @brief Wraps WiFi.connect(). While attempting to connect, MiP's eyes light
   * up in a Knight Rider-style back-and-forth animation.
   * @return WL_CONNECTED if the connection attempt was successful, else it
   * returns the error code from WiFi.connect().
   */
  uint8_t connect();

  /**
   * @brief Turns off WiFi and Bluetooth. If MiP is in app mode, which requires
   * Bluetooth, MiP is switched to its default gesture mode.
   */
  void enableAirplaneMode();

  /**
   * @brief Turns the WiFi radio on and attempts to connect to the last access
   * point to which MiP was connected.
   @return WL_CONNECTED if the connection attempt was successful, else it
   * returns the error code from WiFi.connect().
   */
  uint8_t disableAirplaneMode();

  void clear();

 private:
  char m_ssid[32];
  char m_password[64];
  char m_hostname[63];

  MiP& m_mip;  // Stores a reference to the main MiP class.
};

#endif  // MPU_WIFI_H
