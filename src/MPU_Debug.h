/**
 * @file MPU_Debug.h
 * @brief Defines the debug logging and telnet remote console interface for the MiP library.
 *
 * @details This header declares the MiP_Debug class, severity constants, and global helper
 * macros (`mDebug`, `mDebugI`, `mDebugE`, etc.) used for multi-level logging, execution profiling,
 * text filtering, and wireless telnet debugging on ESP8266 controllers.
 *
 * It allows developers to log messages remotely over WiFi via standard Telnet clients (e.g., PuTTY,
 * Terminal) without interrupting physical UART hardware links connected to MiP.
 *
 * @author Joao Lopes (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the MIT License
 * (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * https://opensource.org/licenses/MIT
 */
#ifndef MPU_DEBUG_H
#define MPU_DEBUG_H

#include <ESP8266WiFi.h>
#include "Arduino.h"
#include "Print.h"

extern "C" {
  /**
   * @brief ESP8266 SDK low-level function to adjust system CPU clock frequency.
   *
   * @details Adjusts the ESP8266 CPU clock frequency dynamically at runtime.
   *
   * @param freq Target CPU clock frequency in MHz (typically 80 or 160).
   * @return **true** if the clock frequency change succeeded; **false** otherwise.
   */
  bool system_update_cpu_freq(uint8 freq);
}

/**
 * @name Global Logging Helper Macros
 * @{
 *
 * @details Convenience macros that evaluate active logging severity thresholds before
 * formatting and emitting output over active debug channels (Telnet and/or Serial).
 */

/**
 * @brief Logs formatted text unconditionally at ANY debug severity level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebug(...) \
  { \
    if (debug.isActive(debug.ANY)) debug.printf(__VA_ARGS__); \
  }

/**
 * @brief Logs formatted text at the PROFILER debug severity level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebugP(...) \
  { \
    if (debug.isActive(debug.PROFILER)) debug.printf(__VA_ARGS__); \
  }

/**
 * @brief Logs formatted text at the VERBOSE debug severity level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebugV(...) \
  { \
    if (debug.isActive(debug.VERBOSE)) debug.printf(__VA_ARGS__); \
  }

/**
 * @brief Logs formatted text at the DEBUG debug severity level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebugD(...) \
  { \
    if (debug.isActive(debug.DEBUG)) debug.printf(__VA_ARGS__); \
  }

/**
 * @brief Logs formatted text at the INFO debug severity level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebugI(...) \
  { \
    if (debug.isActive(debug.INFO)) debug.printf(__VA_ARGS__); \
  }

/**
 * @brief Logs formatted text at the WARNING debug severity level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebugW(...) \
  { \
    if (debug.isActive(debug.WARNING)) debug.printf(__VA_ARGS__); \
  }

/**
 * @brief Logs formatted text at the ERROR debug severity level.
 * @param ... Format string and arguments (printf-style).
 */
#define mDebugE(...) \
  { \
    if (debug.isActive(debug.ERROR)) debug.printf(__VA_ARGS__); \
  }

/** @} */

/**
 * @brief Telnet and Serial debug server for real-time remote diagnostics.
 *
 * @details The `MiP_Debug` class inherits from Arduino's `Print` class, enabling full support for
 * standard output formatting methods (`print`, `println`, `printf`, `write`). It acts as a lightweight
 * TCP Telnet server running on port 23 that streams log output wirelessly to connected Telnet terminals.
 *
 * ### Key Features:
 * - **Multi-Level Filtering:** Filters output dynamically by severity (`PROFILER`, `VERBOSE`, `DEBUG`, `INFO`, `WARNING`, `ERROR`).
 * - **Interactive Terminal Commands:** Allows changing logging levels, viewing free heap, switching CPU clock speeds (80/160 MHz), or resetting the controller remotely over Telnet.
 * - **Performance Profiling:** Tracks execution time delta (in milliseconds) between log messages to identify slow code blocks or blocking loops.
 * - **Dual Output Mode:** Can route log output simultaneously over WiFi Telnet and Hardware Serial (`Serial1`).
 *
 * ### Typical Usage Example:
 * @code```
 * #include <MiP_Power_Up_-_D1_mini.h>
 * #include <MPU_Debug.h>
 *
 * MiP_Debug debug;
 *
 * void setup() {
 *   // Connect to WiFi first, then start debug server
 *   WiFi.begin("SSID", "PASSWORD");
 *   while (WiFi.status() != WL_CONNECTED) { delay(500); }
 *
 *   debug.begin("MiP-DebugTerminal", MiP_Debug::INFO);
 *   debug.setResetCmdEnabled(true);
 *   mDebugI("System started successfully on IP: %s\n", WiFi.localIP().toString().c_str());
 * }
 *
 * void loop() {
 *   debug.handle(); // Must be called frequently in loop() to process Telnet connections
 *   mDebugD("Loop execution active...\n");
 *   delay(1000);
 * }```
 * @endcode
 */
class MiP_Debug : public Print {
public:
  /**
   * @brief Default TCP port used by the Telnet debug server (Port 23).
   */
  static constexpr uint16_t TELNET_PORT = 23;

  /**
   * @name Logging Severity Levels
   * @{
   * @details Severity thresholds used with begin(), isActive(), and level selection terminal commands.
   */
  static constexpr uint8_t PROFILER = 0;  ///< Profiler mode: records execution timing between consecutive log calls.
  static constexpr uint8_t VERBOSE  = 1;  ///< Verbose level: outputs all granular diagnostic messages.
  static constexpr uint8_t DEBUG    = 2;  ///< Debug level: outputs standard debugging statements.
  static constexpr uint8_t INFO     = 3;  ///< Info level: outputs general status and informational messages.
  static constexpr uint8_t WARNING  = 4;  ///< Warning level: outputs non-fatal warnings and unexpected states.
  static constexpr uint8_t ERROR    = 5;  ///< Error level: outputs critical error reports only.
  static constexpr uint8_t ANY      = 6;  ///< Any level: forces output unconditionally regardless of active threshold.
  /** @} */

  /**
   * @brief Initializes and starts the Telnet debug server.
   *
   * @details Binds the TCP Telnet server to port 23, configures internal socket transmit buffers,
   * sets the device broadcast hostname, and establishes the initial minimum severity threshold.
   *
   * @param hostname Broadcast hostname string displayed in the Telnet welcome header.
   * @param startingDebugLevel Minimum active severity threshold required for messages to print (default: `VERBOSE`).
   *
   * @see handle(), stop(), isActive()
   */
  void begin(const String& hostname, uint8_t startingDebugLevel = VERBOSE);

  /**
   * @brief Stops the Telnet debug server and closes all active client connections.
   *
   * @details Disconnects any currently attached Telnet terminal client and shuts down the background
   * TCP server listener socket.
   */
  void stop();

  /**
   * @brief Maintains Telnet network sessions and processes incoming terminal commands.
   *
   * @details Handles incoming client connection requests, monitors client inactivity timeouts,
   * parses interactive Telnet terminal commands (e.g., `?`, `v`, `d`, `i`, `w`, `e`, `m`, `reset`, `cpu160`),
   * and flushes buffered transmit packets over the network.
   *
   * @note **Mandatory:** Must be called frequently inside the main sketch `loop()` function.
   */
  void handle();

  /**
   * @brief Enables or disables echoing debug output to Hardware Serial (`Serial1`).
   *
   * @details When enabled, log output is printed simultaneously to the Telnet network terminal
   * and Hardware Serial (`Serial1`). When disabled, output routes to Telnet clients only.
   *
   * @param enable Set to **true** to mirror log output to Serial; **false** for Telnet only.
   * @note Disables ANSI terminal color formatting automatically when Serial mirroring is active.
   */
  void setSerialEnabled(bool enable);

  /**
   * @brief Configures whether Telnet clients can remotely reboot the controller.
   *
   * @details Controls whether issuing the `reset` command in a connected Telnet terminal
   * triggers an immediate hardware restart (`ESP.restart()`).
   *
   * @param enable Set to **true** to allow remote reboot commands; **false** to disallow (default).
   */
  void setResetCmdEnabled(bool enable);

  /**
   * @brief Registers custom help documentation for sketch-specific Telnet commands.
   *
   * @details Defines custom project command help descriptions displayed when a user types `?` or `help`
   * in the Telnet terminal.
   *
   * @param help Formatting string detailing project-specific commands and descriptions.
   * @see setCallBackProjectCmds()
   */
  void setHelpProjectsCmds(const String& help);

  /**
   * @brief Registers a callback function to handle custom sketch/project Telnet commands.
   *
   * @details Registers a user-defined function executed when an unrecognized command string is received
   * from a connected Telnet client. Use getLastCommand() inside the callback to inspect the received command.
   *
   * @param callback Function pointer to a `void functionName()` callback.
   * @see getLastCommand(), clearLastCommand()
   */
  void setCallBackProjectCmds(void (*callback)());

  /**
   * @brief Retrieves the last command string received from the connected Telnet client.
   *
   * @return String containing the raw command string typed by the user.
   * @see clearLastCommand(), setCallBackProjectCmds()
   */
  String getLastCommand() const;

  /**
   * @brief Clears the internal last-command buffer string.
   *
   * @details Resets the stored command buffer to an empty string. Recommended after handling
   * a custom command in a callback.
   */
  void clearLastCommand();

  /**
   * @brief Toggles prepending timestamp prefixes to log lines.
   *
   * @details When enabled, prepends `(t:XXXXms)` containing the current `millis()` timestamp
   * to the start of each log line.
   *
   * @param show Set to **true** to prepend timestamps; **false** to omit.
   */
  void showTime(bool show);

  /**
   * @brief Configures execution profiler timing prefixes between log messages.
   *
   * @details When enabled, measures and prepends the elapsed time delta `(p:^XXXXms)` between consecutive
   * log calls, making it easy to identify blocking operations or slow code sections.
   *
   * @param show Set to **true** to display profiler time deltas; **false** to omit.
   * @param minTime Minimum elapsed duration in milliseconds required to trigger profiler display (default: 0).
   */
  void showProfiler(bool show, uint32_t minTime = 0);

  /**
   * @brief Toggles prepending severity level indicators to log outputs.
   *
   * @details When enabled, prepends severity tags (`(v)`, `(d)`, `(i)`, `(w)`, `(e)`) to each log line.
   *
   * @param show Set to **true** to show level tags (default); **false** to omit.
   */
  void showDebugLevel(bool show);

  /**
   * @brief Configures ANSI color formatting in Telnet terminals.
   *
   * @details Prepends ANSI color escape codes to highlight log tags based on severity
   * (e.g., Red background for ERROR, Yellow for WARNING, Green for DEBUG).
   *
   * @param show Set to **true** to enable ANSI terminal colors; **false** for plain text.
   * @note Colors are automatically disabled if Hardware Serial mirroring is active.
   */
  void showColors(bool show);

  /**
   * @brief Sets an automatic threshold to temporarily switch to PROFILER mode if loop time exceeds a limit.
   *
   * @param millisElapsed Threshold duration in milliseconds between `handle()` calls that triggers PROFILER mode.
   */
  void autoProfilerLevel(uint32_t millisElapsed);

  /**
   * @brief Filters log outputs to lines containing a specific substring.
   *
   * @details Applies a case-insensitive text filter. Only log lines containing the specified filter string
   * will be printed to active debug outputs.
   *
   * @param filter Substring filter text to match.
   * @see setNoFilter()
   */
  void setFilter(const String& filter);

  /**
   * @brief Disables any active text filter string.
   *
   * @details Removes the filter applied by setFilter(), allowing all messages meeting the severity threshold to print.
   */
  void setNoFilter();

  /**
   * @brief Evaluates whether logging is active for a given severity level.
   *
   * @details Evaluates if @p debugLevel meets or exceeds the current minimum severity threshold
   * AND at least one output destination (Telnet client or Hardware Serial) is connected/enabled.
   *
   * @param debugLevel Target severity level to test (default: `DEBUG`).
   * @return **true** if output will be printed for the specified level; **false** if suppressed.
   */
  bool isActive(uint8_t debugLevel = DEBUG);

  /**
   * @brief Writes a single character byte to active debug outputs.
   *
   * @param byte Character byte to output.
   * @return size_t Number of bytes written (1 on success).
   */
  virtual size_t write(uint8_t byte) override;

  /**
   * @brief Writes a character array buffer to active debug outputs.
   *
   * @param buffer Pointer to character array buffer.
   * @param size Number of bytes in buffer to write.
   * @return size_t Number of bytes successfully written.
   */
  virtual size_t write(const uint8_t* buffer, size_t size) override;

  /**
   * @brief Utility function to expand carriage return and line feed control characters into visible string literals.
   *
   * @details Converts raw `\r` and `\n` characters into literal `"\r"` and `"\n"` text representations for clean terminal display.
   *
   * @param string Input string containing raw control characters.
   * @return String with expanded control character representations.
   */
  String expand(const String& string);

protected:
  // --- System Configuration Constants ---
  static constexpr uint32_t MAX_TIME_INACTIVE = 3600000;  ///< Client inactivity timeout in milliseconds (1 hour).
  static constexpr size_t BUFFER_PRINT = 150;            ///< Maximum internal character buffer size before flushing.

#ifdef CLIENT_BUFFERING
  static constexpr uint32_t DELAY_TO_SEND = 10;          ///< Minimum packet send buffering delay in milliseconds.
  static constexpr size_t MAX_SIZE_SEND = 1460;          ///< Maximum TCP packet payload size (MSS limit).
#endif

  // --- ANSI Terminal Escape Codes ---
  static constexpr const char* COLOR_RESET = "\x1B[0m";              ///< Reset terminal text formatting.
  static constexpr const char* COLOR_BLACK = "\x1B[0;30m";           ///< Black text color.
  static constexpr const char* COLOR_RED = "\x1B[0;31m";             ///< Red text color.
  static constexpr const char* COLOR_GREEN = "\x1B[0;32m";           ///< Green text color.
  static constexpr const char* COLOR_YELLOW = "\x1B[0;33m";          ///< Yellow text color.
  static constexpr const char* COLOR_BLUE = "\x1B[0;34m";            ///< Blue text color.
  static constexpr const char* COLOR_MAGENTA = "\x1B[0;35m";         ///< Magenta text color.
  static constexpr const char* COLOR_CYAN = "\x1B[0;36m";            ///< Cyan text color.
  static constexpr const char* COLOR_WHITE = "\x1B[0;37m";           ///< White text color.
  static constexpr const char* COLOR_BACKGROUND_RED = "\x1B[41m";    ///< Red background color.
  static constexpr const char* COLOR_BACKGROUND_GREEN = "\x1B[42m";  ///< Green background color.
  static constexpr const char* COLOR_BACKGROUND_YELLOW = "\x1B[43m"; ///< Yellow background color.
  static constexpr const char* COLOR_BACKGROUND_MAGENTA = "\x1B[45m";///< Magenta background color.
  static constexpr const char* COLOR_BACKGROUND_CYAN = "\x1B[46m";   ///< Cyan background color.
  static constexpr const char* COLOR_BACKGROUND_WHITE = "\x1B[47m";  ///< White background color.

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
};

#endif  // MPU_DEBUG_H
