/**
 * @file MPU_WiFi.h
 * @brief Defines the public interface for WiFi networking, telnet console,
 *        and Over-The-Air (OTA) firmware updates in the MiP library.
 *
 * @details This header declares the MiP_Wifi class used to manage network
 * connectivity, OTA updates, mDNS host registration, and telnet debug logging
 * for ESP8266-based controllers communicating with MiP.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_WIFI_H
#define MPU_WIFI_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include "MPU_Debug.h"

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Manages WiFi network connections, mDNS hostname broadcasting,
 *        Over-The-Air (OTA) updates, and telnet debug streaming for MiP.
 */
class MiP_Wifi {
 public:
  /**
   * @brief Initializes WiFi network connection and starts network background services.
   *
   * @details Connects to the specified Access Point using @p ssid and @p password,
   * configures mDNS host broadcasting using @p hostname, initializes ArduinoOTA
   * handlers for wireless firmware flashing, and starts the telnet debug server.
   *
   * @param ssid     Name of the target WiFi Access Point network.
   * @param password Password for the target WiFi network.
   * @param hostname Hostname broadcast on the network via mDNS and telnet (default: "MiP").
   */
  void begin(const char* ssid, const char* password, const char* hostname = "MiP");

  /**
   * @brief Disconnects from the WiFi network and shuts down network services.
   *
   * @details Stops the telnet debug server, terminates active connections,
   * and shuts down the ESP8266 WiFi radio hardware.
   */
  void end();

  /**
   * @brief Processes pending network events, OTA updates, and telnet communications.
   *
   * @details Must be called periodically inside the sketch loop() function to maintain
   * network background tasks, process incoming OTA flash requests, and handle telnet client input.
   */
  void handle();

  /**
   * @brief Checks whether MiP's network controller is currently connected to a WiFi Access Point.
   *
   * @return true if connected and assigned an IP address, false otherwise.
   */
  bool isConnected() const;

  /**
   * @brief Returns the local IP address assigned to MiP's network controller.
   *
   * @return IPAddress Local network IP address.
   */
  IPAddress localIP() const;

 private:
  /**
   * @brief Constructs the WiFi manager.
   *
   * @param mip Reference to the main MiP object for core communication services.
   */
  explicit MiP_Wifi(MiP& mip);

  void clear();
 
  MiP& m_mip;        // Stores a reference to the main MiP class.
  bool m_isConnected; // Track active WiFi connection state.

  /**
   * @brief Allows MiP to call private constructor.
   */
  friend class MiP;
};

#endif  // MPU_WIFI_H
