/**
 * @file MPU_Debug.h
 * @brief Defines the debug functionality and telnet logging interface for the
 * MiP library.
 *
 * @details This header declares the MiPDebug class and associated
 * macros/constants for multi-level logging, execution profiling, and remote
 * telnet debugging on ESP8266.
 *
 * @author Joao Lopes (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_DEBUG_H
#define MPU_DEBUG_H

#include <ESP8266WiFi.h>
#include "Arduino.h"
#include "Print.h"

extern "C" {
/**
 * @brief ESP8266 SDK low-level function to adjust the CPU frequency.
 * @param freq Target CPU frequency in MHz (e.g., 80 or 160).
 * @return true if frequency change succeeded, false otherwise.
 */
bool system_update_cpu_freq(uint8 freq);
}

/**
 * @name Logging Helper Macros
 * @{
 */

/**
 * @brief Logs formatted text at ANY debug level (always output if debugging
 * active).
 * @param ... Format string and arguments (printf-style).
 */
#define mDebug(...)                \
  {                                \
    if (debug.isActive(debug.ANY)) \
      debug.printf(__VA_ARGS__);   \
  }

/**
 * @brief Logs formatted text at the PROFILER debug level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebugP(...)                    \
  {                                     \
    if (debug.isActive(debug.PROFILER)) \
      debug.printf(__VA_ARGS__);        \
  }

/**
 * @brief Logs formatted text at the VERBOSE debug level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebugV(...)                   \
  {                                    \
    if (debug.isActive(debug.VERBOSE)) \
      debug.printf(__VA_ARGS__);       \
  }

/**
 * @brief Logs formatted text at the DEBUG debug level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebugD(...)                 \
  {                                  \
    if (debug.isActive(debug.DEBUG)) \
      debug.printf(__VA_ARGS__);     \
  }

/**
 * @brief Logs formatted text at the INFO debug level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebugI(...)                \
  {                                 \
    if (debug.isActive(debug.INFO)) \
      debug.printf(__VA_ARGS__);    \
  }

/**
 * @brief Logs formatted text at the WARNING debug level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebugW(...)                   \
  {                                    \
    if (debug.isActive(debug.WARNING)) \
      debug.printf(__VA_ARGS__);       \
  }

/**
 * @brief Logs formatted text at the ERROR debug level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebugE(...)                 \
  {                                  \
    if (debug.isActive(debug.ERROR)) \
      debug.printf(__VA_ARGS__);     \
  }

/** @} */

/**
 * @name System Configuration Constants
 * @{
 */

/** @brief Default TCP port for the telnet debug server. */
#define TELNET_PORT 23

/** @brief Maximum client inactivity duration before disconnect (in
 * milliseconds). Default: 1 hour (3600000 ms). Set to 0 to disable. */
#define MAX_TIME_INACTIVE 3600000

/** @brief Size of internal character buffer used for formatted telnet printing.
 */
#define BUFFER_PRINT 150

#ifdef CLIENT_BUFFERING
/** @brief Minimum delay in milliseconds before transmitting buffered client
 * packets. */
#define DELAY_TO_SEND 10

/** @brief Maximum packet payload size in bytes (TCP/IP MSS limit). */
#define MAX_SIZE_SEND 1460
#endif

/** @} */

/**
 * @name ANSI Terminal Escape Codes
 * @{
 */

#define COLOR_RESET "\x1B[0m"           /**< Reset text attributes to default */
#define COLOR_BLACK "\x1B[0;30m"        /**< Standard Black text */
#define COLOR_RED "\x1B[0;31m"          /**< Standard Red text */
#define COLOR_GREEN "\x1B[0;32m"        /**< Standard Green text */
#define COLOR_YELLOW "\x1B[0;33m"       /**< Standard Yellow text */
#define COLOR_BLUE "\x1B[0;34m"         /**< Standard Blue text */
#define COLOR_MAGENTA "\x1B[0;35m"      /**< Standard Magenta text */
#define COLOR_CYAN "\x1B[0;36m"         /**< Standard Cyan text */
#define COLOR_WHITE "\x1B[0;37m"        /**< Standard White text */
#define COLOR_DARK_BLACK "\x1B[1;30m"   /**< Bold/Dark Black text */
#define COLOR_DARK_RED "\x1B[1;31m"     /**< Bold/Dark Red text */
#define COLOR_DARK_GREEN "\x1B[1;32m"   /**< Bold/Dark Green text */
#define COLOR_DARK_YELLOW "\x1B[1;33m"  /**< Bold/Dark Yellow text */
#define COLOR_DARK_BLUE "\x1B[1;34m"    /**< Bold/Dark Blue text */
#define COLOR_DARK_MAGENTA "\x1B[1;35m" /**< Bold/Dark Magenta text */
#define COLOR_DARK_CYAN "\x1B[1;36m"    /**< Bold/Dark Cyan text */
#define COLOR_DARK_WHITE "\x1B[1;37m"   /**< Bold/Dark White text */
#define COLOR_BACKGROUND_BLACK "\x1B[40m"   /**< Black background color */
#define COLOR_BACKGROUND_RED "\x1B[41m"     /**< Red background color */
#define COLOR_BACKGROUND_GREEN "\x1B[42m"   /**< Green background color */
#define COLOR_BACKGROUND_YELLOW "\x1B[43m"  /**< Yellow background color */
#define COLOR_BACKGROUND_BLUE "\x1B[44m"    /**< Blue background color */
#define COLOR_BACKGROUND_MAGENTA "\x1B[45m" /**< Magenta background color */
#define COLOR_BACKGROUND_CYAN "\x1B[46m"    /**< Cyan background color */
#define COLOR_BACKGROUND_WHITE "\x1B[47m"   /**< White background color */

/** @} */

/**
 * @brief Telnet and Serial debug server for real-time remote diagnostics.
 *
 * @details Inherits from Arduino's Print class to support formatted output
 * methods (print, println, printf) over telnet network connections and optional
 * hardware Serial.
 */
class MiPDebug : public Print {
 public:
  /**
   * @name Logging Severity Levels
   * @{
   */
  static const uint8_t PROFILER =
      0; /**< Used to display execution timing of code sections (profiler). */
  static const uint8_t VERBOSE =
      1; /**< Used to display detailed verbose messages. */
  static const uint8_t DEBUG =
      2; /**< Used to display standard debug messages. */
  static const uint8_t INFO =
      3; /**< Used to display informational status messages. */
  static const uint8_t WARNING = 4; /**< Used to display warning messages. */
  static const uint8_t ERROR =
      5; /**< Used to display critical error messages. */
  static const uint8_t ANY =
      6; /**< Used to display messages unconditionally at any active level. */
  /** @} */

  /**
   * @brief Initializes and starts the telnet debug server.
   * @param hostname Hostname broadcast by the telnet service.
   * @param startingDebugLevel Initial active debug threshold (default:
   * VERBOSE).
   */
  void begin(String hostname, uint8_t startingDebugLevel = VERBOSE);

  /**
   * @brief Stops the telnet debug server and disconnects active clients.
   */
  void stop();

  /**
   * @brief Service loop handler for maintaining telnet client connections and
   * processing commands.
   * @note Must be called periodically inside the sketch loop().
   */
  void handle();

  /**
   * @brief Configures whether debug messages are echoed to HardwareSerial.
   * @param enable true to duplicate output to Serial, false to route to telnet
   * only.
   */
  void setSerialEnabled(bool enable);

  /**
   * @brief Configures whether the telnet server allows remote system reset
   * commands.
   * @param enable true to allow remote ESP8266 reset via telnet command, false
   * to disallow.
   */
  void setResetCmdEnabled(bool enable);

  /**
   * @brief Sets custom project help text displayed when a telnet client sends
   * the help command.
   * @param help Formatting string detailing sketch-specific commands.
   */
  void setHelpProjectsCmds(String help);

  /**
   * @brief Registers a callback handler for custom sketch/project telnet
   * commands.
   * @param callback Function pointer executed when a custom command is
   * received.
   */
  void setCallBackProjectCmds(void (*callback)());

  /**
   * @brief Retrieves the last command string received from the connected telnet
   * client.
   * @return String containing the command text.
   */
  String getLastCommand();

  /**
   * @brief Clears the internal last-command buffer string.
   */
  void clearLastCommand();

  /**
   * @brief Configures whether timestamps (in milliseconds) are prepended to log
   * outputs.
   * @param show true to show timestamps, false to omit.
   */
  void showTime(bool show);

  /**
   * @brief Configures execution profiler logging between consecutive prints.
   * @param show true to show elapsed execution time, false to omit.
   * @param minTime Minimum elapsed time threshold in milliseconds required to
   * trigger profiler output.
   */
  void showProfiler(bool show, uint32_t minTime = 0);

  /**
   * @brief Configures whether debug severity tags ([INFO], [DEBUG], etc.) are
   * prepended to messages.
   * @param show true to show level tags, false to omit.
   */
  void showDebugLevel(bool show);

  /**
   * @brief Configures whether ANSI color codes are included in telnet log
   * output.
   * @param show true to enable color codes, false for plain text.
   */
  void showColors(bool show);

  /**
   * @brief Automatically switches logging level to PROFILER if elapsed time
   * exceeds threshold.
   * @param millisElapsed Threshold duration in milliseconds between loop
   * iterations.
   */
  void autoProfilerLevel(uint32_t millisElapsed);

  /**
   * @brief Applies a text filter string to debug outputs.
   * @param filter Substring filter; only log lines containing this string will
   * be printed.
   */
  void setFilter(String filter);

  /**
   * @brief Removes any active text filter string.
   */
  void setNoFilter();

  /**
   * @brief Checks if logging output is active for the specified debug level.
   * @param debugLevel Target debug level to check (default: DEBUG).
   * @return true if specified level meets or exceeds active threshold; false
   * otherwise.
   */
  bool isActive(uint8_t debugLevel = DEBUG);

  /**
   * @brief Writes a single byte to active debug channels (Stream/Print
   * override).
   * @param byte Character byte to output.
   * @return Number of bytes written (1 on success).
   */
  virtual size_t write(uint8_t byte) override;

  /**
   * @brief Writes a byte buffer to active debug channels (Stream/Print
   * override).
   * @param buffer Pointer to character array to output.
   * @param size Number of bytes in buffer.
   * @return Number of bytes successfully written.
   */
  virtual size_t write(const uint8_t* buffer, size_t size) override;

  /**
   * @brief Helper method to expand CR/LF control characters into literal "\\r"
   * and "\\n" strings.
   * @param string Input string containing raw control characters.
   * @return String with CR/LF replaced by escaped representations.
   */
  String expand(String string);

 protected:
  void showHelp();
  void processCommand();
  String formatNumber(uint32_t value, uint8_t size, char insert = '0');
  bool isCRLF(char character);

  String m_hostname = "";  ///< The user-defined hostname for the telnet server.
  bool m_connected = false;  ///< Is a client connected?
  uint8_t m_clientDebugLevel =
      DEBUG;  ///< The debug level set by the user in telnet.
  uint8_t m_lastDebugLevel = DEBUG;     ///< Last debug level set by active().
  uint32_t m_lastTimePrint = millis();  ///< The last time a line was printed.
  uint8_t m_levelBeforeProfiler =
      DEBUG;  ///< Last level before setting the profiler level.
  uint32_t m_levelProfilerDisable =
      0;  ///< Time in millis to disable the profiler level.
  uint32_t m_autoLevelProfiler =
      0;  ///< Automatic change to profiler level if time between handles is
          ///< greater than n millis
  bool m_showTime = false;             ///< Show time in milliseconds.
  bool m_showProfiler = false;         ///< Show time between messages.
  uint32_t m_minTimeShowProfiler = 0;  ///< Minimum time to show profiler.
  bool m_showDebugLevel = true;  ///< Show debug level on each debug message.
  bool m_showColors = false;     ///< Show colors.
  bool m_serialEnabled = false;  ///< Send debug messages to serial too.
  bool m_resetCommandEnabled =
      false;                  ///< Allow the telnet server to reset the ESP8266.
  bool m_newLine = true;      ///< New line write ?
  String m_command = "";      ///< The current command received from the user.
  String m_lastCommand = "";  ///< The last command received from the user.
  uint32_t m_lastTimeCommand =
      millis();  ///< Time that the last command was received.
  String m_helpProjectCmds =
      "";  ///< Help commands set by the project (sketch).
  void (*m_callbackProjectCmds)();  ///< Callable for project commands.
  String m_filter = "";             ///< The filter string.
  bool m_filterActive = false;      ///< Is the filter active?
  String m_bufferPrint = "";        ///< Print buffer for telnet output.
#ifdef CLIENT_BUFFERING
  String m_bufferSend = "";       ///< Buffer for sending data to telnet client.
  uint16_t m_sizeBufferSend = 0;  ///< The size of the buffer.
  uint32_t m_lastTimeSend = 0;    ///< The last time the command sent data.
#endif
};

#endif  // MPU_DEBUG_H
