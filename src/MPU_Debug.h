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
 * @brief ESP8266 SDK low-level function to adjust CPU frequency.
 * @param freq Target CPU frequency in MHz (e.g., 80 or 160).
 * @return true if frequency change succeeded, false otherwise.
 */
bool system_update_cpu_freq(uint8 freq);
}

/**
 * @name Logging Helper Macros
 * @{
 */

#define mDebug(...)                \
  {                                \
    if (debug.isActive(debug.ANY)) \
      debug.printf(__VA_ARGS__);   \
  }

#define mDebugP(...)                    \
  {                                     \
    if (debug.isActive(debug.PROFILER)) \
      debug.printf(__VA_ARGS__);        \
  }

#define mDebugV(...)                   \
  {                                    \
    if (debug.isActive(debug.VERBOSE)) \
      debug.printf(__VA_ARGS__);       \
  }

#define mDebugD(...)                 \
  {                                  \
    if (debug.isActive(debug.DEBUG)) \
      debug.printf(__VA_ARGS__);     \
  }

#define mDebugI(...)                \
  {                                 \
    if (debug.isActive(debug.INFO)) \
      debug.printf(__VA_ARGS__);    \
  }

#define mDebugW(...)                   \
  {                                    \
    if (debug.isActive(debug.WARNING)) \
      debug.printf(__VA_ARGS__);       \
  }

#define mDebugE(...)                 \
  {                                  \
    if (debug.isActive(debug.ERROR)) \
      debug.printf(__VA_ARGS__);     \
  }

/** @} */

/**
 * @brief Telnet and Serial debug server for real-time remote diagnostics.
 */
class MiPDebug : public Print {
 public:
  /**
   * @name Logging Severity Levels
   * @{
   */
  static constexpr uint8_t PROFILER = 0; ///< Execution timing section profiling.
  static constexpr uint8_t VERBOSE  = 1; ///< Detailed verbose messages.
  static constexpr uint8_t DEBUG    = 2; ///< Standard debug messages.
  static constexpr uint8_t INFO     = 3; ///< Informational status messages.
  static constexpr uint8_t WARNING  = 4; ///< Warning messages.
  static constexpr uint8_t ERROR    = 5; ///< Critical error messages.
  static constexpr uint8_t ANY      = 6; ///< Messages output unconditionally.
  /** @} */

  /**
   * @brief Initializes and starts the telnet debug server.
   * @param hostname Hostname broadcast by the telnet service.
   * @param startingDebugLevel Initial active debug threshold (default: VERBOSE).
   */
  void begin(const String& hostname, uint8_t startingDebugLevel = VERBOSE);

  /**
   * @brief Stops the telnet debug server and disconnects active clients.
   */
  void stop();

  /**
   * @brief Service loop handler for maintaining telnet client connections and processing commands.
   * @note Must be called periodically inside the sketch loop().
   */
  void handle();

  /**
   * @brief Configures whether debug messages are echoed to HardwareSerial.
   * @param enable true to duplicate output to Serial, false for telnet only.
   */
  void setSerialEnabled(bool enable);

  /**
   * @brief Configures whether the telnet server allows remote system reset commands.
   * @param enable true to allow remote ESP8266 reset via telnet command.
   */
  void setResetCmdEnabled(bool enable);

  /**
   * @brief Sets custom project help text displayed on telnet help command.
   * @param help Formatting string detailing sketch-specific commands.
   */
  void setHelpProjectsCmds(const String& help);

  /**
   * @brief Registers a callback handler for custom sketch/project telnet commands.
   * @param callback Function pointer executed when a custom command is received.
   */
  void setCallBackProjectCmds(void (*callback)());

  /**
   * @brief Retrieves the last command string received from the connected telnet client.
   * @return String containing command text.
   */
  String getLastCommand() const;

  /**
   * @brief Clears internal last-command buffer string.
   */
  void clearLastCommand();

  /**
   * @brief Configures whether timestamps (in ms) are prepended to log outputs.
   * @param show true to show timestamps, false to omit.
   */
  void showTime(bool show);

  /**
   * @brief Configures execution profiler logging between consecutive prints.
   * @param show true to show elapsed execution time, false to omit.
   * @param minTime Minimum elapsed time threshold in ms required to trigger profiler output.
   */
  void showProfiler(bool show, uint32_t minTime = 0);

  /**
   * @brief Configures whether debug severity tags ([INFO], [DEBUG], etc.) are prepended.
   * @param show true to show level tags, false to omit.
   */
  void showDebugLevel(bool show);

  /**
   * @brief Configures whether ANSI color codes are included in log output.
   * @param show true to enable color codes, false for plain text.
   */
  void showColors(bool show);

  /**
   * @brief Automatically switches logging level to PROFILER if elapsed time exceeds threshold.
   * @param millisElapsed Threshold duration in ms between loop iterations.
   */
  void autoProfilerLevel(uint32_t millisElapsed);

  /**
   * @brief Applies a text filter string to debug outputs.
   * @param filter Substring filter; only log lines containing this string will print.
   */
  void setFilter(const String& filter);

  /**
   * @brief Removes any active text filter string.
   */
  void setNoFilter();

  /**
   * @brief Checks if logging output is active for the specified debug level.
   * @param debugLevel Target debug level to check (default: DEBUG).
   * @return true if level meets or exceeds active threshold; false otherwise.
   */
  bool isActive(uint8_t debugLevel = DEBUG);

  virtual size_t write(uint8_t byte) override;
  virtual size_t write(const uint8_t* buffer, size_t size) override;

  /**
   * @brief Helper method to expand CR/LF control characters into literal "\\r" and "\\n".
   * @param string Input string containing raw control characters.
   * @return String with CR/LF replaced by escaped representations.
   */
  String expand(const String& string);

 protected:
  // --- System Configuration Constants ---
  static constexpr uint16_t TELNET_PORT = 23;            ///< Default TCP telnet port.
  static constexpr uint32_t MAX_TIME_INACTIVE = 3600000;  ///< Inactivity disconnect timeout (1 hour).
  static constexpr size_t BUFFER_PRINT = 150;            ///< Print character buffer size.

#ifdef CLIENT_BUFFERING
  static constexpr uint32_t DELAY_TO_SEND = 10;          ///< Packet send delay in ms.
  static constexpr size_t MAX_SIZE_SEND = 1460;          ///< TCP MSS limit.
#endif

  // --- ANSI Terminal Escape Codes ---
  static constexpr const char* COLOR_RESET = "\x1B[0m";
  static constexpr const char* COLOR_BLACK = "\x1B[0;30m";
  static constexpr const char* COLOR_RED = "\x1B[0;31m";
  static constexpr const char* COLOR_GREEN = "\x1B[0;32m";
  static constexpr const char* COLOR_YELLOW = "\x1B[0;33m";
  static constexpr const char* COLOR_BLUE = "\x1B[0;34m";
  static constexpr const char* COLOR_MAGENTA = "\x1B[0;35m";
  static constexpr const char* COLOR_CYAN = "\x1B[0;36m";
  static constexpr const char* COLOR_WHITE = "\x1B[0;37m";
  static constexpr const char* COLOR_BACKGROUND_RED = "\x1B[41m";
  static constexpr const char* COLOR_BACKGROUND_GREEN = "\x1B[42m";
  static constexpr const char* COLOR_BACKGROUND_YELLOW = "\x1B[43m";
  static constexpr const char* COLOR_BACKGROUND_MAGENTA = "\x1B[45m";
  static constexpr const char* COLOR_BACKGROUND_CYAN = "\x1B[46m";
  static constexpr const char* COLOR_BACKGROUND_WHITE = "\x1B[47m";

 private:
  void showHelp();
  void processCommand();
  String formatNumber(uint32_t value, uint8_t size, char insert = '0');
  bool isCRLF(char character);

  String m_hostname = "";
  bool m_connected = false;
  uint8_t m_clientDebugLevel = DEBUG;
  uint8_t m_lastDebugLevel = DEBUG;
  uint32_t m_lastTimePrint = millis();
  uint8_t m_levelBeforeProfiler = DEBUG;
  uint32_t m_levelProfilerDisable = 0;
  uint32_t m_autoLevelProfiler = 0;
  bool m_showTime = false;
  bool m_showProfiler = false;
  uint32_t m_minTimeShowProfiler = 0;
  bool m_showDebugLevel = true;
  bool m_showColors = false;
  bool m_serialEnabled = false;
  bool m_resetCommandEnabled = false;
  bool m_newLine = true;
  String m_command = "";
  String m_lastCommand = "";
  uint32_t m_lastTimeCommand = millis();
  String m_helpProjectCmds = "";
  void (*m_callbackProjectCmds)() = nullptr;
  String m_filter = "";
  bool m_filterActive = false;
  String m_bufferPrint = "";

#ifdef CLIENT_BUFFERING
  String m_bufferSend = "";
  uint16_t m_sizeBufferSend = 0;
  uint32_t m_lastTimeSend = 0;
#endif

  friend class MiP_WiFi;
};

#endif  // MPU_DEBUG_H
