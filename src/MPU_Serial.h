/**
 * @file MPU_Serial.h
 * @brief Defines the public interface for serial transport in the MiP library.
 *
 * @details This header declares the low-level serial API used for MiP
 * communication.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_SERIAL_H
#define MPU_SERIAL_H

#include <Arduino.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Manages low-level UART transport and OOB event demux.
 */
class MiP_Serial {
 public:
  static constexpr uint8_t MIP_REQUEST_DELAY = 8;
  static constexpr uint8_t MIP_RESPONSE_TIMEOUT = 100;

  // expectResponse parameter values for transportSendRequest() parameter.
  static constexpr uint8_t MIP_EXPECT_NO_RESPONSE = 0;
  static constexpr uint8_t MIP_EXPECT_RESPONSE = 1;

  // Maximum length of MiP request and response buffer lengths.
  static constexpr size_t MIP_REQUEST_MAX_LEN =
      17 + 1;  // Longest request is MIP_CMD_PLAY_SOUND.
  static constexpr size_t MIP_RESPONSE_MAX_LEN =
      5 + 1;  // Longest response is MIP_CMD_REQUEST_CHEST_LED.

  // Maximum number of retries for verified operations (clap, chest LED, etc.).
  static constexpr uint8_t MIP_MAX_RETRIES = 2;

  // Milliseconds to wait between retries.
  static constexpr uint16_t MIP_RETRY_WAIT = 50;

  /**
   * @brief Constructs the serial port manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_Serial(MiP& mip);

  /**
   * @brief Sends a raw command to the MiP (fire-and-forget).
   *
   * Used internally by higher-level verified methods.
   */
  void rawSend(const uint8_t request[], size_t requestLength);

  /**
   * @brief Sends a raw command and waits for the expected response.
   *
   * @param request          Command buffer to send.
   * @param requestLength    Length of the command.
   * @param responseBuffer   Buffer to store the response.
   * @param responseBufferSize Size of the response buffer.
   * @param responseLength   Receives the actual number of bytes read.
   * @return MIP_ERROR_NONE on success, or an error code.
   */
  uint8_t rawReceive(const uint8_t request[],
                     size_t requestLength,
                     uint8_t responseBuffer[],
                     size_t responseBufferSize,
                     size_t& responseLength);

  bool processAllResponseData();

 protected:
  void clear();

 private:
  uint8_t discardUnexpectedSerialData();
  void processOobResponseData(uint8_t commandByte);
  uint8_t transportGetResponse(uint8_t* pResponseBuffer,
                               size_t responseBufferSize,
                               size_t* pResponseLength);
  void transportSendRequest(const uint8_t* pRequest,
                            size_t requestLength,
                            int expectResponse);

  // Hex helpers
  void copyHexTextToBinary(uint8_t* pDest, uint8_t* pSrc, uint8_t length);
  uint8_t parseHexDigit(uint8_t digit);

  // Optional readability helper for the variable-length IR case
  bool readIrLength(size_t& length);

  MiP& m_mip;  // Stores a reference to the main MiP class.
  uint32_t m_lastRequestTime;
  uint8_t m_expectedResponseSize;
  uint8_t m_expectedResponseCommand;
  uint8_t m_responseBuffer[MIP_RESPONSE_MAX_LEN];

  friend class MiP;
};

#endif  // MPU_SERIAL_H
