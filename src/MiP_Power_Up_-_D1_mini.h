/**
 * @file MiP_Power_Up_-_D1_mini.h
 * @brief Defines the core MPU API and subsystem interfaces.
 *
 * @details This header declares the main MiP class and the public interfaces
 * exposed by the library subsystems.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MIP_POWER_UP_D1_MINI_H
#define MIP_POWER_UP_D1_MINI_H

#include <Arduino.h>

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
#include "MPU_WiFi.h"

// MiP Power Up - D1 mini library versioning
#define MPU_D1_MINI_VERSION_MAJOR 2
#define MPU_D1_MINI_VERSION_MINOR 0
#define MPU_D1_MINI_VERSION_PATCH 2

// Combined string representation matching library.properties
#define MPU_D1_MINI_VERSION "2.0.2"

// Combined numerical value for preprocessor version checks (2.0.1 -> 20001)
#define MPU_D1_MINI_VERSION_NUMBER \
  (MPU_D1_MINI_VERSION_MAJOR * 10000 + MPU_D1_MINI_VERSION_MINOR * 100 + MPU_D1_MINI_VERSION_PATCH)

// Setup some debug levels for reporting library status via Serial1.
#define MIP_DEBUG_NONE 0
#define MIP_DEBUG_ERROR 1
#define MIP_DEBUG_WARN 2
#define MIP_DEBUG_INFO 3

// Default to NONE if not defined by the user in the sketch.
#ifndef MIP_DEBUG_LEVEL
#define MIP_DEBUG_LEVEL MIP_DEBUG_NONE
#endif

// ---------------------------------------------------------------------------
// Debug macros — prefix is printed once via *_PREFIX(), then stream with
// *_PRINT / *_PRINTLN.  Never call *_PREFIX more than once for a single
// logical message.  No *PRINTF variants (portable to AVR and ESP cores).
// ---------------------------------------------------------------------------

#if MIP_DEBUG_LEVEL >= MIP_DEBUG_ERROR
#define MIP_DEBUG_ERROR_PREFIX() Serial1.print(F("[ERROR] "))
#define MIP_DEBUG_ERROR_PRINT(...) Serial1.print(__VA_ARGS__)
#define MIP_DEBUG_ERROR_PRINTLN(...) Serial1.println(__VA_ARGS__)
#else
#define MIP_DEBUG_ERROR_PREFIX()
#define MIP_DEBUG_ERROR_PRINT(...)
#define MIP_DEBUG_ERROR_PRINTLN(...)
#endif

#if MIP_DEBUG_LEVEL >= MIP_DEBUG_WARN
#define MIP_DEBUG_WARN_PREFIX() Serial1.print(F("[WARN] "))
#define MIP_DEBUG_WARN_PRINT(...) Serial1.print(__VA_ARGS__)
#define MIP_DEBUG_WARN_PRINTLN(...) Serial1.println(__VA_ARGS__)
#else
#define MIP_DEBUG_WARN_PREFIX()
#define MIP_DEBUG_WARN_PRINT(...)
#define MIP_DEBUG_WARN_PRINTLN(...)
#endif

#if MIP_DEBUG_LEVEL >= MIP_DEBUG_INFO
#define MIP_DEBUG_INFO_PREFIX() Serial1.print(F("[INFO] "))
#define MIP_DEBUG_INFO_PRINT(...) Serial1.print(__VA_ARGS__)
#define MIP_DEBUG_INFO_PRINTLN(...) Serial1.println(__VA_ARGS__)
#else
#define MIP_DEBUG_INFO_PREFIX()
#define MIP_DEBUG_INFO_PRINT(...)
#define MIP_DEBUG_INFO_PRINTLN(...)
#endif

// Define an assert mechanism that can be used to log and halt when the user is
// found to be calling the API incorrectly.
#define MIP_ASSERT(EXPRESSION) mipAssert((EXPRESSION), __LINE__, __FILE__)

/**
 * @brief MiP's current stance position and battery voltage.
 */
class MiPStatus {
public:
  /**
   * @brief Constructs a new MiPStatus object and resets values to default.
   */
  MiPStatus() {
    clear();
  }

  /**
   * @brief Resets battery voltage to 0.0V and position to default stance.
   */
  void clear() {
    battery = 0.0f;
    position = MIP_POSITION_ON_BACK_WITH_KICKSTAND;
  }

  float battery;         ///< Cached battery voltage in Volts (4.0V - 6.4V).
  MiPPosition position;  ///< Cached physical orientation stance.
};

/**
 * @mainpage MiP Power Up - D1 mini library
 *
 * This library provides a complete interface to control WowWee MiP
 * over UART from a WeMos D1 mini (or compatible ESP8266 board).
 */
class MiP {
public:
  static constexpr uint8_t MIP_ERROR_NONE = 0;      ///< Operation succeeded.
  static constexpr uint8_t MIP_ERROR_TIMEOUT = 1;   ///< Timed out waiting for
                                                    ///< response from MiP.
  static constexpr uint8_t MIP_ERROR_NO_EVENT = 2;  ///< No event has arrived
                                                    ///< from MiP yet.
  static constexpr uint8_t MIP_ERROR_BAD_RESPONSE = 3;  ///< Unexpected response
                                                        ///< received from MiP.
  static constexpr uint8_t MIP_ERROR_MAX_RETRIES = 4;   ///< Exceeded maximum
                                                        ///< retries
                                                        ///< communicating with
                                                        ///< MiP.

  // Core lifecycle functions.

  /**
   * @brief Constructs the core MiP orchestrator object and binds subsystem
   * references.
   *
   * @details Initializes internal flags and resets error tracking states.
   */
  MiP();

  /**
   * @brief Destructs the MiP orchestrator object and terminates connections.
   *
   * @details Calls end() to restore defaults and close transport services.
   */
  ~MiP();

  /**
   * @brief Initializes the core UART connection to MiP.
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
   * Restores default volume, sends disconnect command to MiP, and ends
   * Serial/WiFi/OTA services.
   */
  void end();

  /**
   * @brief Puts MiP to sleep.
   *
   * MiP will need to be physically reset before another `begin()` call.
   */
  void sleep();

  /**
   * @brief Check whether the MiP library has completed initialization.
   *
   * Returns true when the internal initialization flag (MIP_FLAG_INITIALIZED)
   * is set, indicating that `begin()` completed successfully and the MiP
   * instance is ready for use. Returns false if initialization failed or
   * has not yet been performed.
   *
   * This accessor does not modify object state and has no side effects.
   * Consider calling this before invoking API methods that require an active
   * connection to MiP.
   *
   * @return **true** if the MiP instance is initialized and ready; **false**
   *         otherwise.
   *
   * @see MiP::begin()
   */
  bool isInitialized() const;

  // Error Handling.

  /**
   * @brief Retrieves the error code from the most recently executed MiP API
   * function.
   *
   * The library automatically attempts to handle communication errors by
   * retrying read/write operations behind the scenes. If an operation
   * ultimately fails to recover, this function provides the specific reason for
   * the failure.
   *
   * @return int8_t The error code generated by the last operation (e.g.,
   * MIP_ERROR_NONE, MIP_ERROR_TIMEOUT, MIP_ERROR_BAD_RESPONSE).
   */
  int8_t lastCallResult() const;

  /**
   * @brief Checks if the most recently executed MiP API function encountered a
   * critical error.
   *
   * A quick convenience method to determine if a command was successful
   * without needing to evaluate the specific error code.
   *
   * @return true If the last operation failed (m_lastError is not
   * MIP_ERROR_NONE).
   * @return false If the last operation succeeded.
   */
  bool didLastCallFail() const;

  /**
   * @brief Prints a human-readable description of the last error to the debug
   *        channel (Serial1).
   */
  void printLastCallResult();

  /**
   * @brief Returns the UART baud rate currently in use with MiP.
   *
   * After a successful @ref begin(), this reports either 115200 or 9600
   * depending on which rate the library negotiated with MiP.
   * Returns 0 if the connection has not been established (or after
   * @ref end() / a failed @ref begin()).
   *
   * Useful for diagnostics and for sketches that need to know the link
   * speed of the connected MiP hardware revision.
   *
   * @return Baud rate in bits per second (115200, 9600, or 0).
   *
   * @see begin()
   */
  uint32_t getBaudRate() const;

  MiP_Battery battery;    ///< Interface for battery voltage queries (see
                          ///< MPU_Battery.h).
  MiP_ChestLED chestLED;  ///< Interface for chest LED RGB/flash control (see
                          ///< MPU_ChestLED.h).
  MiP_Clap clap;  ///< Interface for clap detection and delay configuration (see
                  ///< MPU_Clap.h).
  MiP_EEPROM eeprom;      ///< Interface for reading/writing non-volatile EEPROM
                          ///< (see MPU_EEPROM.h).
  MiP_Gesture gesture;    ///< Interface for gesture detection mode and events
                          ///< (see MPU_Gesture.h).
  MiP_HeadLEDs headLEDs;  ///< Interface for controlling head/eye LED patterns
                          ///< (see MPU_HeadLEDs.h).
  MiP_Infrared infrared;  ///< Interface for IR remote control and MiP detection
                          ///< (see MPU_Infrared.h).
  MiP_Mode mode;          ///< Interface for game and app mode selection (see
                          ///< MPU_Mode.h).
  MiP_Motion motion;  ///< Interface for drive, turn, and posture control (see
                      ///< MPU_Motion.h).
  MiP_Odometer odometer;  ///< Interface for reading and resetting distance
                          ///< odometer (see MPU_Odometer.h).
  MiP_Position position;  ///< Interface for position and orientation state
                          ///< checks (see MPU_Position.h).
  MiP_Radar radar;        ///< Interface for IR radar proximity tracking (see
                          ///< MPU_Radar.h).
  MiP_Serial serial;      ///< Interface for low-level UART transport and event
                          ///< parsing (see MPU_Serial.h).
  MiP_Shake shake;  ///< Interface for shake event detection (see MPU_Shake.h).
  MiP_Sound sound;  ///< Interface for sound effects and volume control (see
                    ///< MPU_Sound.h).
  MiP_Version version;  ///< Interface for querying hardware/software versions
                        ///< (see MPU_Version.h).
  MiP_Weight weight;    ///< Interface for payload weight sensor queries (see
                        ///< MPU_Weight.h).
  MiP_WiFi wifi;  ///< Interface for WiFi, OTA, and network management (see
                  ///< MPU_WiFi.h).

protected:
  static constexpr uint8_t MIP_CMD_DISCONNECT_APP = 0xFE;  ///< Disconnect
                                                           ///< command byte.
  static constexpr uint8_t MIP_CMD_SLEEP = 0xFA;       ///< Sleep command byte.
  static constexpr uint8_t MIP_CMD_GET_STATUS = 0x79;  ///< Status query command
                                                       ///< byte.

  static constexpr uint8_t MIP_MAX_BEGIN_RETRIES = 5;    ///< Max retries in
                                                         ///< begin().
  static constexpr uint16_t MIP_BEGIN_RETRY_WAIT = 500;  ///< Delay between
                                                         ///< retries in begin()
                                                         ///< (ms).
  static constexpr uint32_t ESP8266_DEBUG_BAUD_RATE = 74880;  ///< ESP8266
                                                              ///< bootloader
                                                              ///< debug rate.
  static constexpr uint32_t MIP_FAST_BAUD_RATE = 115200;  ///< High-speed UART
                                                          ///< link rate.
  static constexpr uint32_t MIP_SLOW_BAUD_RATE = 9600;  ///< Low-speed UART link
                                                        ///< rate.

  void clear();

  int8_t attemptMiPConnection(uint32_t baudRate);

  /**
   * @brief Central dispatcher for all Out-of-Band events from the transport
   * layer.
   *
   * This method receives raw event data and routes it to the appropriate
   * subsystem component (Clap, Gesture, Status, etc.).
   */
  void dispatchEvent(uint8_t command, const uint8_t* payload, size_t length);

  /**
   * @brief Assert mechanism that logs the failure location and then halts.
   *
   * Call it like this (no macro needed):
   *   mipAssert(condition, __LINE__, __FILE__);
   *
   * The original macro version always printed the file that *defined*
   * mipAssert (MPU_Core.cpp). Passing __FILE__ from the call site
   * fixes that.
   */
  void mipAssert(bool condition, uint32_t lineNumber, const char* fileName);

  int8_t rawGetStatus(MiPStatus& status);
  int8_t parseStatus(MiPStatus& status, const uint8_t response[], size_t responseLength);

  friend class MiP_Battery;
  friend class MiP_ChestLED;
  friend class MiP_Clap;
  friend class MiP_EEPROM;
  friend class MiP_Gesture;
  friend class MiP_HeadLEDs;
  friend class MiP_Infrared;
  friend class MiP_Mode;
  friend class MiP_Motion;
  friend class MiP_Odometer;
  friend class MiP_Position;
  friend class MiP_Radar;
  friend class MiP_Serial;
  friend class MiP_Shake;
  friend class MiP_Sound;
  friend class MiP_Version;
  friend class MiP_Weight;
  friend class MiP_WiFi;

  // Bits that can be set in m_flags bitfield.
  enum FlagBits : uint8_t {
    MIP_FLAG_RADAR_VALID = (1 << 0),
    MIP_FLAG_SHAKE_DETECTED = (1 << 1),
    MIP_FLAG_WEIGHT_VALID = (1 << 2),
    MIP_FLAG_INITIALIZED = (1 << 3)
  };

  uint32_t m_baudRate;
  uint8_t m_flags;
  int8_t m_lastError;
  MiPStatus m_lastStatus;
};

#endif  // MIP_POWER_UP_D1_MINI_H
