/**
 * @file MPU_Console.cpp
 * @brief Auto-switching UART console for sending debug text to the PC.
 *
 * @details This component handles sending text and debug messages to the PC /
 * FTDI Serial Monitor over the Arduino Pro Mini's single hardware UART. It
 * automatically toggles the hardware UART multiplexer to PC before writing
 * and restores it back to MiP afterward.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include "MPU_Console.h"
#include "MiP_Power_Up_-_Pro_Mini.h"

/**
 * @brief Constructs the MiP_Console component.
 * @param mip Reference to the main MiP object for access to UART multiplexer
 * controls.
 */
MiP_Console::MiP_Console(MiP& mip) : m_mip(mip), m_isInit(false) {}

/**
 * @brief Internal helper to lazily initialize the HardwareSerial port if
 * begin() wasn't called.
 */
void MiP_Console::initIfNeeded() {
  if (!m_isInit) {
    Serial.begin(kDefaultBaudRate, kDefaultConfig);
    m_isInit = true;
  }
}

/**
 * @brief Switches the multiplexer to PC and returns whether MiP was active
 * beforehand.
 * @return true if communication was active with MiP prior to switching.
 */
bool MiP_Console::prepareForPcWrite() {
  bool needToRestore = m_mip.isSerialGoingToMiP();
  m_mip.switchSerialToPC();
  return needToRestore;
}

/**
 * @brief Flushes the Serial output buffer and restores the multiplexer to MiP
 * if required.
 * @param needToRestore Whether communication was active with MiP prior to
 * writing.
 */
void MiP_Console::restoreAfterPcWrite(bool needToRestore) {
  Serial.flush();
  if (needToRestore) { m_mip.switchSerialToMiP(); }
}

/**
 * @brief Initializes the HardwareSerial port at the specified baud rate.
 * @param baud Baud rate parameter (defaults to 115200).
 */
void MiP_Console::begin(unsigned long baud) {
  begin(baud, kDefaultConfig);
}

/**
 * @brief Initializes the HardwareSerial port with specified baud rate and
 * config.
 * @param baud Baud rate parameter.
 * @param config Serial protocol mode (e.g., SERIAL_8N1).
 */
void MiP_Console::begin(unsigned long baud, uint16_t config) {
  if (m_isInit) { return; }
  Serial.begin(baud, config);
  m_isInit = true;
}

/**
 * @brief Shuts down the HardwareSerial interface.
 */
void MiP_Console::end() {
  if (!m_isInit) { return; }
  Serial.end();
  m_isInit = false;
}

/**
 * @brief Returns the number of bytes available to read from the PC Serial
 * Monitor.
 */
int MiP_Console::available() {
  initIfNeeded();
  return Serial.available();
}

/**
 * @brief Reads the next incoming byte from the PC Serial Monitor.
 */
int MiP_Console::read() {
  initIfNeeded();
  return Serial.read();
}

/**
 * @brief Peeks at the next incoming byte from the PC Serial Monitor without
 * removing it.
 */
int MiP_Console::peek() {
  initIfNeeded();
  return Serial.peek();
}

/**
 * @brief Returns the space available in the HardwareSerial transmit buffer.
 */
int MiP_Console::availableForWrite() {
  initIfNeeded();
  return Serial.availableForWrite();
}

/**
 * @brief Flushes the HardwareSerial transmit buffer.
 */
void MiP_Console::flush() {
  initIfNeeded();
  Serial.flush();
}

/**
 * @brief Writes a single byte to the PC Serial Monitor.
 * @param byte The byte to send.
 * @return size_t Number of bytes written (1 on success).
 */
size_t MiP_Console::write(uint8_t byte) {
  return write(&byte, 1);
}

/**
 * @brief Writes a buffer of bytes to the PC Serial Monitor.
 *
 * Automatically toggles the hardware UART multiplexer to the PC before writing
 * and restores it back to MiP if it was previously active.
 *
 * @param pBuffer Pointer to the data array.
 * @param size    Number of bytes to write.
 * @return size_t Number of bytes successfully written.
 */
size_t MiP_Console::write(const uint8_t* pBuffer, size_t size) {
  if (pBuffer == nullptr || size == 0) { return 0; }

  initIfNeeded();

  bool needToRestore = prepareForPcWrite();
  size_t result = Serial.write(pBuffer, size);
  restoreAfterPcWrite(needToRestore);

  return result;
}
