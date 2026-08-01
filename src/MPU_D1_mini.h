/**
 * @file MPU_D1_mini.h
 * @brief Defines the core MPU API and subsystem interfaces.
 *
 * @details This header declares the main MiP class and the public interfaces
 * exposed by the library subsystems.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_D1_MINI_H
#define MPU_D1_MINI_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <stdint.h>
#include <stdlib.h>

#include "MPU_Battery.h"
#include "MPU_ChestLED.h"
#include "MPU_Clap.h"
#include "MPU_EEPROM.h"
#include "MPU_Gesture.h"
#include "MPU_HeadLEDs.h"
#include "MPU_Infrared.h"
#include "MPU_Mode.h"
#include "MPU_Motion.h"
#include "MPU_Odometer.h"
#include "MPU_Position.h"
#include "MPU_Queue.h"
#include "MPU_Radar.h"
#include "MPU_Serial.h"
#include "MPU_Shake.h"
#include "MPU_Sound.h"
#include "MPU_Version.h"
#include "MPU_Weight.h"
#include "MPU_Wifi.h"

// MiP Power Up: D1 mini library versioning
#define MPU_D1_MINI_VERSION_MAJOR 1
#define MPU_D1_MINI_VERSION_MINOR 1
#define MPU_D1_MINI_VERSION_PATCH 0

// Combined string representation matching library.properties
#define MPU_D1_MINI_VERSION "1.1.0"

// Combined numerical value for preprocessor version checks (1.1.0 -> 10100)
#define MPU_D1_MINI_VERSION_NUMBER                                       \
  (MPU_D1_MINI_VERSION_MAJOR * 10000 + MPU_D1_MINI_VERSION_MINOR * 100 + \
   MPU_D1_MINI_VERSION_PATCH)

// Setup some debug levels for reporting library status via Serial1.
#define MIP_DEBUG_NONE 0
#define MIP_DEBUG_ERROR 1
#define MIP_DEBUG_WARN 2
#define MIP_DEBUG_INFO 3

// Default to NONE if not defined by the user in the sketch.
#ifndef MIP_DEBUG_LEVEL
#define MIP_DEBUG_LEVEL MIP_DEBUG_NONE
#endif

// Create the macros for conditional printing of debug messages via Serial1.
#if MIP_DEBUG_LEVEL >= MIP_DEBUG_ERROR
#define MIP_DEBUG_ERROR_PRINT(...) \
  Serial1.print(F("[ERROR] "));    \
  Serial1.print(__VA_ARGS__)
#define MIP_DEBUG_ERROR_PRINTLN(...) \
  Serial1.print(F("[ERROR] "));      \
  Serial1.println(__VA_ARGS__)
#define MIP_DEBUG_ERROR_PRINTF(...) \
  Serial1.print(F("[ERROR] "));     \
  Serial1.printf(__VA_ARGS__)
#else
#define MIP_DEBUG_ERROR_PRINT(...)
#define MIP_DEBUG_ERROR_PRINTLN(...)
#define MIP_DEBUG_ERROR_PRINTF(...)
#endif
#if MIP_DEBUG_LEVEL >= MIP_DEBUG_WARN
#define MIP_DEBUG_WARN_PRINT(...) \
  Serial1.print(F("[WARN] "));    \
  Serial1.print(__VA_ARGS__)
#define MIP_DEBUG_WARN_PRINTLN(...) \
  Serial1.print(F("[WARN] "));      \
  Serial1.println(__VA_ARGS__)
#define MIP_DEBUG_WARN_PRINTF(...) \
  Serial1.print(F("[WARN] "));     \
  Serial1.printf(__VA_ARGS__)
#else
#define MIP_DEBUG_WARN_PRINT(...)
#define MIP_DEBUG_WARN_PRINTLN(...)
#define MIP_DEBUG_WARN_PRINTF(...)
#endif
#if MIP_DEBUG_LEVEL >= MIP_DEBUG_INFO
#define MIP_DEBUG_INFO_PRINT(...) \
  Serial1.print(F("[INFO] "));    \
  Serial1.print(__VA_ARGS__)
#define MIP_DEBUG_INFO_PRINTLN(...) \
  Serial1.print(F("[INFO] "));      \
  Serial1.println(__VA_ARGS__)
#define MIP_DEBUG_INFO_PRINTF(...) \
  Serial1.print(F("[INFO] "));     \
  Serial1.printf(__VA_ARGS__)
#else
#define MIP_DEBUG_INFO_PRINT(...)
#define MIP_DEBUG_INFO_PRINTLN(...)
#define MIP_DEBUG_INFO_PRINTF(...)
#endif

// Define an assert mechanism that can be used to log and halt when the user is
// found to be calling the API incorrectly.
#define MIP_ASSERT(EXPRESSION) mipAssert((EXPRESSION), __LINE__, __FILE__)

/**
 * @brief MiP's current position and battery voltage.
 */
class MiPStatus {
 public:
  MiPStatus() {
    clear();
  }

  void clear() {
    battery = 0.0f;
    position = MIP_POSITION_ON_BACK_WITH_KICKSTAND;
  }

  float battery;
  MiPPosition position;
};

/**
 * @mainpage MiP Power Up: D1 mini library
 *
 * This library provides a complete interface to control the WowWee MiP robot
 * over UART from a WeMos D1 mini (or compatible ESP8266 board).
 */
class MiP {
 public:
  /**
   * @brief MiP protocol command bytes related to core functions.
   *
   * These values are placed in the first byte of requests sent to the MiP
   * (and appear in the corresponding responses).  See the official
   * [MiP BLE
   * Protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md)
   * for the complete list.
   */
  static constexpr uint8_t MIP_CMD_DISCONNECT_APP = 0xFE;
  static constexpr uint8_t MIP_CMD_SLEEP = 0xFA;
  static constexpr uint8_t MIP_CMD_GET_STATUS = 0x79;

  /**
   * @brief Integer error codes that can be encountered by the MPU library.
   */
  static constexpr uint8_t MIP_ERROR_NONE = 0;  // Success
  static constexpr uint8_t MIP_ERROR_TIMEOUT =
      1;  // Timed out waiting for response.
  static constexpr uint8_t MIP_ERROR_NO_EVENT =
      2;  // No event has arrived from MiP yet.
  static constexpr uint8_t MIP_ERROR_BAD_RESPONSE =
      3;  // Unexpected response from MiP.
  static constexpr uint8_t MIP_ERROR_MAX_RETRIES =
      4;  // Exceeded maximum number of retries to get this operation to
          // succeed.

  // Core lifecycle functions.
  MiP();

  ~MiP();

  /**
   * @brief Initializes the core UART connection to the MiP robot.
   *
   * Attempts connection at both 115200 and 9600 baud rates with retries.
   * Sets up debug output on Serial1 and prepares internal state.
   *
   * @return true if successfully connected to MiP, false otherwise.
   */
  bool begin();

  /**
   * @brief Cleans up the connection to MiP and shuts down network services.
   *
   * Restores default volume, sends disconnect command, and ends
   * Serial/WiFi/OTA.
   */
  void end();

  /**
   * @brief Puts the MiP robot to sleep.
   *
   * The robot will need to be physically reset before another `begin()` call.
   */
  void sleep();

  /**
   * @brief Check whether the MiP library has completed initialization.
   *
   * Returns true when the internal initialization flag (MRI_FLAG_INITIALIZED)
   * is set, indicating that `begin()` completed successfully and the MiP
   * instance is ready for use. Returns false if initialization failed or
   * has not yet been performed.
   *
   * This accessor does not modify object state and has no side effects.
   * Consider calling this before invoking API methods that require an active
   * connection to the robot.
   *
   * @return **true** if the MiP instance is initialized and ready; **false**
   *         otherwise.
   *
   * @see MiP::begin()
   */
  bool isInitialized();

  // Error Handling.
  /**
   * @brief Retrieves the error code from the most recently executed MiP API
   * function.
   * * The library automatically attempts to handle communication errors by
   * retrying read/write operations behind the scenes. If an operation
   * ultimately fails to recover, this function provides the specific reason for
   * the failure.
   * * @return int8_t The error code generated by the last operation (e.g.,
   * MIP_ERROR_NONE, MIP_ERROR_TIMEOUT, MIP_ERROR_BAD_RESPONSE).
   */
  int8_t lastCallResult();

  /**
   * @brief Checks if the most recently executed MiP API function encountered a
   * critical error.
   * * A quick convenience method to determine if a command was successful
   * without needing to evaluate the specific error code.
   * * @return true If the last operation failed (m_lastError is not
   * MIP_ERROR_NONE).
   * @return false If the last operation succeeded.
   */
  bool didLastCallFail();

  /**
   * @brief Central dispatcher for all Out-of-Band events from the transport
   * layer.
   *
   * This method receives raw event data and routes it to the appropriate
   * subsystem component (Clap, Gesture, Status, etc.).
   */
  void dispatchEvent(uint8_t command, const uint8_t* payload, size_t);

  /**
   * @brief Prints a human-readable description of the last error to the debug
   *        channel (Serial1).
   */
  void printLastCallResult();

  // See MPU_Battery.h for interfacing with the battery.
  MiP_Battery battery;

  // See MPU_ChestLED.h for interfacing with the chest LED.
  MiP_ChestLED chestLED;

  // See MPU_Clap.h for interfacing with the clap detection system.
  MiP_Clap clap;

  // See MPU_EEPROM.h for reading from and writing to MiP's EEPROM.
  MiP_EEPROM eeprom;

  // See MPU_Gesture.h for interfacing with the gesture detection system.
  MiP_Gesture gesture;

  // See MPU_HeadLEDs.h for interfacing with MiP's head LEDs.
  MiP_HeadLEDs headLEDs;

  // See MPU_Infrared.h for interfacing with the infrared system.
  MiP_Infrared infrared;

  // See MPU_Mode.h for selecting MiP's modes.
  MiP_Mode mode;

  // See MPU_Motion.h for interfacing with the drive system.
  MiP_Motion motion;

  // See MPU_Odometer.h for reading and resetting the odometer.
  MiP_Odometer odometer;

  // See MPU_Position.h for reading the position detection system.
  MiP_Position position;

  // See MPU_Radar.h for interfacing with the radar.
  MiP_Radar radar;

  // See MPU_Serial.h for interfacing with MiP's serial port.
  MiP_Serial serial;

  // See MPU_Shake.h for reading the shake detection system.
  MiP_Shake shake;

  // See MPU_Sound.h for interfacing with the sound system.
  MiP_Sound sound;

  // See MPU_Version.h for reading MiP's hardware and software versions.
  MiP_Version version;

  // See MPU_Weight.h for reading MiP's weight.
  MiP_Weight weight;

  // See MPU_Wifi.h for interfacing with the MPU's wifi system.
  MiP_Wifi wifi;

 protected:
  void clear();

  int8_t attemptMiPConnection(uint32_t baudRate);

  /**
   * @brief Assert mechanism that logs the failure location and then halts.
   *
   * Call it like this (no macro needed):
   *   mipAssert(condition, __LINE__, __FILE__);
   *
   * The original macro version always printed the file that *defined*
   * mipAssert (MPU_Core.cpp).  Passing __FILE__ from the call site
   * fixes that.
   */
  void mipAssert(bool condition, uint32_t lineNumber, const char* fileName);

  int8_t rawGetStatus(MiPStatus& status);
  int8_t parseStatus(MiPStatus& status,
                     const uint8_t response[],
                     size_t responseLength);

  friend class MiP_Battery;
  friend class MiP_ChestLED;
  friend class MiP_Clap;
  friend class MiP_EEPROM;
  friend class MiP_Gesture;
  friend class MiP_HeadLEDs;
  friend class MiP_Infrared;
  friend class MiP_Mode;
  friend class MiP_Motion;
  friend class MiP_Wifi;
  friend class MiP_Odometer;
  friend class MiP_Position;
  friend class MiP_Radar;
  friend class MiP_Serial;
  friend class MiP_Shake;
  friend class MiP_Sound;
  friend class MiP_Version;
  friend class MiP_Weight;

  // Bits that can be set in m_flags bitfield.
  enum FlagBits : uint8_t {
    MIP_FLAG_RADAR_VALID = (1 << 0),
    MIP_FLAG_SHAKE_DETECTED = (1 << 1),
    MIP_FLAG_WEIGHT_VALID = (1 << 2),
    MIP_FLAG_INITIALIZED = (1 << 3)
  };

  uint8_t m_flags;
  int8_t m_lastError;
  MiPStatus m_lastStatus;
};

#endif  // MPU_D1_MINI_H
