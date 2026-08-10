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
 * @brief Manages low-level UART transport and Out-Of-Band (OOB) event demultiplexing.
 *
 * @details Handles serial transmission delays, command-response pairing, hex-ASCII parsing,
 * and dispatching asynchronous event notifications received from the MiP robot.
 */
class MiP_Serial {
 public:
  /**
   * @brief Minimum inter-command delay in milliseconds required between consecutive UART transmissions.
   */
  static constexpr uint8_t MIP_REQUEST_DELAY = 8;

  /**
   * @brief Maximum duration in milliseconds to wait for a synchronous command response from MiP.
   */
  static constexpr uint8_t MIP_RESPONSE_TIMEOUT = 100;

  /**
   * @brief Flag passed to transportSendRequest() indicating no response is expected (fire-and-forget).
   */
  static constexpr uint8_t MIP_EXPECT_NO_RESPONSE = 0;

  /**
   * @brief Flag passed to transportSendRequest() indicating a synchronous response is expected.
   */
  static constexpr uint8_t MIP_EXPECT_RESPONSE = 1;

  /**
   * @brief Maximum buffer size in bytes required to store the longest MiP protocol request.
   */
  static constexpr size_t MIP_REQUEST_MAX_LEN = 17 + 1;

  /**
   * @brief Maximum buffer size in bytes required to store the longest MiP protocol response.
   */
  static constexpr size_t MIP_RESPONSE_MAX_LEN = 5 + 1;

  /**
   * @brief Maximum number of retry attempts for verified read and write operations.
   */
  static constexpr uint8_t MIP_MAX_RETRIES = 2;

  /**
   * @brief Duration in milliseconds to wait between retry attempts.
   */
  static constexpr uint16_t MIP_RETRY_WAIT = 50;

  /**
   * @brief Sends a raw command buffer to MiP without waiting for a response (fire-and-forget).
   *
   * @details Used internally by non-verified commands and higher-level driver methods. Enforces
   * the minimum MIP_REQUEST_DELAY before writing data to the hardware serial port.
   *
   * @param request       Pointer to the array of command bytes to transmit.
   * @param requestLength Number of bytes in the request array.
   */
  void rawSend(const uint8_t request[], size_t requestLength);

  /**
   * @brief Sends a raw command and blocks until the expected response is received or times out.
   *
   * @details Transmits the request buffer over UART, waits for the corresponding command response byte,
   * converts incoming hex-ASCII payload characters to binary, and populates the response buffer.
   *
   * @param[in]  request          Pointer to the array of command bytes to transmit.
   * @param[in]  requestLength    Number of bytes in the request array.
   * @param[out] responseBuffer   Pointer to the buffer where the received response will be stored.
   * @param[in]  responseBufferSize Maximum capacity of the response buffer in bytes.
   * @param[out] responseLength   Receives the actual number of bytes written into responseBuffer.
   *
   * @return uint8_t MIP_ERROR_NONE on success, or MIP_ERROR_TIMEOUT on response timeout.
   */
  uint8_t rawReceive(const uint8_t request[],
                     size_t requestLength,
                     uint8_t responseBuffer[],
                     size_t responseBufferSize,
                     size_t& responseLength);

  /**
   * @brief Reads and processes all incoming data available in the hardware serial receive buffer.
   *
   * @details Decodes incoming hex-ASCII command pairs. If the command byte matches an expected
   * synchronous response, it populates the response buffer. Otherwise, it treats the data as an
   * Out-Of-Band (OOB) notification and dispatches it to MiP::dispatchEvent().
   *
   * @return true if the expected synchronous response was found during buffer processing, false otherwise.
   */
  bool processAllResponseData();

 protected:
  void clear();

 private:
  /**
   * @brief Constructs the serial port transport manager.
   *
   * @param mip A reference to the main MiP object to access core communication services.
   */
  MiP_Serial(MiP& mip);

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

  /**
   * @brief Allows MiP to call private constructor.
   */
  friend class MiP;
};

#endif  // MPU_SERIAL_H
