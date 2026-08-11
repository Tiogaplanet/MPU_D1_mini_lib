/**
 * @file MPU_WiFi.h
 *
 * @brief Defines the public interface for WiFi management in the MiP library.
 *
 * @details This header declares the WiFi API used to connect MiP to a network.
 *
 * @author Samuel Trassare (Original Author)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may
 * obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_WIFI_H
#define MPU_WIFI_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Manages MiP's WiFi capability.
 */
class MiP_WiFi {
 public:
  /**
   * @brief Wraps WiFi.begin().  Connects to a wireless access point and loads
   * the MPU-D1 mini with OTA programming support.
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

 protected:
  void clear();

 private:
  /**
   * @brief Constructs the wifi manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_WiFi(MiP& mip);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  char m_ssid[32];
  char m_password[64];
  char m_hostname[63];

  /**
   * @brief Allows MiP to call private constructor.
   */
  friend class MiP;
};

#endif  // MPU_WIFI_H
