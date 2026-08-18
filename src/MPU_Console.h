/**
 * @file MPU_Console.h
 * @brief Auto-switching UART console for sending debug text to the PC.
 *
 * @details This header defines the MiP_Console class, which inherits from
 * Arduino's Stream class. It provides transparent redirection of print
 * statements to the PC Serial Monitor over a shared single hardware UART on the
 * Arduino Pro Mini.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_CONSOLE_H
#define MPU_CONSOLE_H

#include <Arduino.h>
#include <Stream.h>

// Forward declaration of the main MiP class to avoid circular includes
class MiP;

/**
 * @brief Redirects Print/Stream writes to the PC Serial Monitor by temporarily
 *        switching the Pro Mini's UART multiplexer away from MiP.
 *
 * @details The MiP_Console class allows debug messages and standard Stream I/O
 *          operations to be sent to and received from an external PC via the
 *          shared HardwareSerial connection without interfering with regular
 *          communication to MiP.
 */
class MiP_Console : public Stream {
public:
  // --- Stream Interface Overrides ---

  /**
   * @brief Queries the number of bytes available for reading from the PC Serial
   * Monitor.
   *
   * @details Lazy-initializes the hardware serial port if necessary before
   * checking the input buffer.
   *
   * @return int Number of bytes available to read, or 0 if no data is present.
   */
  virtual int available() override;

  /**
   * @brief Reads the next available character from the PC Serial Monitor.
   *
   * @details Removes the byte from the incoming serial buffer.
   *
   * @return int The next character read from the stream, or -1 if no data is
   * available.
   */
  virtual int read() override;

  /**
   * @brief Peeks at the next incoming character from the PC Serial Monitor
   * without removing it.
   *
   * @return int The next character in the buffer, or -1 if no data is
   * available.
   */
  virtual int peek() override;

  // --- Print Interface Overrides ---

  using Print::write;

  /**
   * @brief Writes a single byte to the PC Serial Monitor.
   *
   * @details Toggles the multiplexer to PC, writes the byte, flushes the
   * transmit buffer, and restores communication back to MiP if MiP was active
   * beforehand.
   *
   * @param byte The single character or byte value to transmit.
   * @return size_t The number of bytes written (returns 1 on success, 0 on
   * failure).
   */
  virtual size_t write(uint8_t byte) override;

  /**
   * @brief Writes a buffer of bytes to the PC Serial Monitor.
   *
   * @details Toggles the multiplexer to PC, writes the buffer, flushes the
   * transmit buffer, and restores communication back to MiP if MiP was active
   * beforehand.
   *
   * @param pBuffer Pointer to the array of bytes to transmit.
   * @param size    Number of bytes in the buffer to transmit.
   * @return size_t The total number of bytes successfully written to the serial
   * port.
   */
  virtual size_t write(const uint8_t* pBuffer, size_t size) override;

  /**
   * @brief Returns the amount of buffer space available for writing without
   * blocking.
   *
   * @return int Number of bytes that can be written to the serial buffer
   * without blocking.
   */
  virtual int availableForWrite() override;

  /**
   * @brief Waits for any outgoing serial transmission to complete.
   *
   * @details Blocks execution until all buffered outgoing serial bytes have
   * been sent.
   */
  virtual void flush() override;

  // --- HardwareSerial Compatibility Methods ---

  /**
   * @brief Starts serial communication with specified baud rate.
   *
   * @param baud Baud rate parameter in bits per second (defaults to 115200).
   */
  void begin(unsigned long baud = kDefaultBaudRate);

  /**
   * @brief Starts serial communication with specified baud rate and
   * configuration mode.
   *
   * @param baud   Baud rate parameter in bits per second.
   * @param config Serial data configuration (e.g., SERIAL_8N1).
   */
  void begin(unsigned long baud, uint16_t config);

  /**
   * @brief Disables serial communication and frees the serial hardware port.
   */
  void end();

  /**
   * @brief Boolean evaluation operator for checking if the serial stream is
   * ready.
   *
   * @return true Always returns true to indicate readiness.
   */
  explicit operator bool() const {
    return true;
  }

private:
  // --- Constants ---
  static constexpr unsigned long kDefaultBaudRate = 115200;
  static constexpr uint16_t kDefaultConfig = SERIAL_8N1;

  /**
   * @brief Constructs the Console manager instance.
   *
   * @param mip Reference to the main MiP instance for multiplexer control.
   */
  explicit MiP_Console(MiP& mip);

  /**
   * @brief Lazy-initialization helper to ensure HardwareSerial is started prior
   * to I/O operations.
   */
  void initIfNeeded();

  /**
   * @brief Prepares multiplexer for writing to PC.
   *
   * @return true if communication was routed to MiP prior to switching.
   */
  bool prepareForPcWrite();

  /**
   * @brief Flushes TX buffer and restores multiplexer back to MiP if needed.
   *
   * @param needToRestore Whether communication was active with MiP prior to
   * write.
   */
  void restoreAfterPcWrite(bool needToRestore);

  MiP& m_mip;     ///< Reference to the main MiP instance
  bool m_isInit;  ///< Flag tracking HardwareSerial initialization status

  friend class MiP;
};

#endif  // MPU_CONSOLE_H
