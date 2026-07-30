/**
 * @file MPU_Serial.cpp
 * @brief Implements serial transport for the MiP library.
 *
 * @details This source file implements low-level request, response, and event
 * processing.
 *
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may
 * obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include "MPU_Serial.h"
#include "MPU_D1_mini.h"

// Implement the constructor to store the MiP reference.
MiP_Serial::MiP_Serial(MiP& mip) : m_mip(mip) {
  clear();
}

void MiP_Serial::clear() {
  // Roll the timers back so that the first calls can occur immediately.
  m_lastRequestTime =
      millis() - 10;  // MIP_REQUEST_DELAY; // (10) offset slightly
  m_expectedResponseSize = 0;
  m_expectedResponseCommand = 0;
  memset(m_responseBuffer, 0, sizeof(m_responseBuffer));
}

uint8_t MiP_Serial::rawReceive(const uint8_t request[],
                               size_t requestLength,
                               uint8_t responseBuffer[],
                               size_t responseBufferSize,
                               size_t& responseLength) {
  transportSendRequest(request, requestLength, MIP_EXPECT_RESPONSE);
  return transportGetResponse(
      responseBuffer, responseBufferSize, &responseLength);
}

void MiP_Serial::rawSend(const uint8_t request[], size_t requestLength) {
  transportSendRequest(request, requestLength, MIP_EXPECT_NO_RESPONSE);
}

// ==========================================================================
// Protected functions.
// ==========================================================================

uint8_t MiP_Serial::transportGetResponse(uint8_t* pResponseBuffer,
                                         size_t responseBufferSize,
                                         size_t* pResponseLength) {
  // Must call begin() and have it return 'true' before calling sending commands
  // to the MiP.
  m_mip.MIP_ASSERT(m_mip.isInitialized());

  // Caller is attempting to get a response that is larger than support by the
  // MiP and this library.
  m_mip.MIP_ASSERT(responseBufferSize <= MIP_RESPONSE_MAX_LEN);

  // UNDONE: I think it would be my bug if the following assert ever fired.
  m_mip.MIP_ASSERT(m_expectedResponseCommand != 0);

  // Process all received bytes (which might include out of band notifications)
  // until we find the response to the last request made. Will timeout after a
  // second.
  m_expectedResponseSize = (uint8_t)responseBufferSize;
  uint32_t startTime = millis();
  bool responseFound = false;
  do {
    responseFound = processAllResponseData();
  } while (!responseFound &&
           (uint32_t)millis() - startTime < MIP_RESPONSE_TIMEOUT);

  if (!responseFound) {
    // Never received the expected response within the timeout window.
    MIP_DEBUG_WARN_PRINTLN(F("MiP: Response timeout"));
    return MiP::MIP_ERROR_TIMEOUT;
  }

  // Copy reponse data into caller provided buffer and clear state in transport
  // about the expected response.
  memcpy(pResponseBuffer, m_responseBuffer, m_expectedResponseSize);
  *pResponseLength = m_expectedResponseSize;
  m_expectedResponseCommand = 0;
  m_expectedResponseSize = 0;
  m_responseBuffer[0] = 0;

  return MiP::MIP_ERROR_NONE;
}

void MiP_Serial::transportSendRequest(const uint8_t* pRequest,
                                      size_t requestLength,
                                      int expectResponse) {
  // Must call begin() and have it return 'true' before calling sending commands
  // to the MiP.
  m_mip.MIP_ASSERT(m_mip.isInitialized());

  // Let the MiP process the last request before letting another request be
  // issued.
  while (millis() - m_lastRequestTime < MIP_REQUEST_DELAY) {
    delay(1);
  }

  // Remember the command byte (first byte) if expecting a response to this
  // request since the response should start with the same byte.
  if (expectResponse)
    m_expectedResponseCommand = pRequest[0];
  else
    m_expectedResponseCommand = 0;

  m_expectedResponseSize = 0;
  m_responseBuffer[0] = 0;

  // Send the specified bytes to the MiP via the UART.
  while (requestLength-- > 0) {
    Serial.write(*pRequest++);
  }
  m_lastRequestTime = millis();
}

bool MiP_Serial::processAllResponseData() {
  bool responseFound = false;
  uint8_t buffer[(MIP_RESPONSE_MAX_LEN - 1) * 2];
  size_t bytesToRead;
  size_t bytesRead;

  while (Serial.available() >= 2) {
    uint8_t highNibble = Serial.read();
    uint8_t lowNibble = Serial.read();
    uint8_t commandByte =
        (parseHexDigit(highNibble) << 4) | parseHexDigit(lowNibble);

    if (m_expectedResponseCommand != 0 &&
        commandByte == m_expectedResponseCommand) {
      // Store away the command byte that we just read into response buffer so
      // that it isn't lost.
      m_responseBuffer[0] = commandByte;

      // Already read the command byte into element 0 of the response buffer
      // earlier so just need to read in the rest of the expected response bytes
      // now.
      bytesToRead = m_expectedResponseSize - 1;
      bytesRead = Serial.readBytes(buffer, bytesToRead * 2);
      if (bytesRead == bytesToRead * 2) {
        copyHexTextToBinary(&m_responseBuffer[1], buffer, bytesToRead);
        responseFound = true;
        // Continue to process any other bytes in the receive buffer.
        // This would allow something like a rawGetStatus() call to receive the
        // actual data returned for this request and not an older OOB perioidic
        // status notification.
      } else {
        // Timed out waiting for all of the response data.
        m_expectedResponseCommand = 0;
        m_expectedResponseSize = 0;
        m_responseBuffer[0] = 0;
        MIP_DEBUG_ERROR_PRINTF(
            "MiP: Response too short: %d, %d\r\n", bytesRead, bytesToRead * 2);
        break;
      }
    } else {
      processOobResponseData(commandByte);
    }
  }
  return responseFound;
}

void MiP_Serial::processOobResponseData(uint8_t commandByte) {
  size_t length = 0;
  size_t bytesRead;

  // The number of additional bytes to read depends on which notification has
  // been found in serial buffer.
  switch (commandByte) {
    case MIP_CMD_GET_RADAR_RESPONSE:
    case MIP_CMD_GET_GESTURE_RESPONSE:
    case MIP_CMD_CLAP_RESPONSE:
    case MiP_Weight::MIP_CMD_GET_WEIGHT:
    case MiP_Infrared::MIP_CMD_GET_DETECTED_MIP:
      length = 1;
      break;
    case MIP_CMD_SHAKE_RESPONSE:
      length = 0;
      break;
    case MiP::MIP_CMD_GET_STATUS:
      length = 2;
      break;
    case MiP_Infrared::MIP_CMD_RECEIVE_IR_DONGLE_CODE:
      // MIP_CMD_RECEIVE_IR_DONGLE_CODE is the only message delivered by MiP
      // that has a variable length so we need to read the next byte which
      // contains the length.
      uint8_t nibbles[2];
      bytesRead = Serial.readBytes(nibbles, sizeof(nibbles));
      if (bytesRead != sizeof(nibbles)) {
        MIP_DEBUG_ERROR_PRINTLN(F("MiP: Missing IR code length"));
        return;
      }
      length = (parseHexDigit(nibbles[0]) << 4) | parseHexDigit(nibbles[1]);
      if (length < 2 || length > 4) {
        uint8_t discardedBytes = discardUnexpectedSerialData();
        MIP_DEBUG_ERROR_PRINTF(
            "MiP: Bad IR code length: 0x%02x (discarded %d bytes)\n",
            length,
            discardedBytes);
        return;
      }
      break;
    default:
      uint8_t discardedBytes = discardUnexpectedSerialData();
      MIP_DEBUG_ERROR_PRINTF(
          "MiP: Bad OOB command byte: 0x%02x (discarded %d bytes)\n",
          commandByte,
          discardedBytes);
      return;
  }

  // Read in the additional bytes of the notification.  The "4" comes from
  // maximum length which is a response for MIP_CMD_RECEIVE_IR_DONGLE_CODE.
  uint8_t buffer[4 * 2];
  bytesRead = Serial.readBytes(buffer, length * 2);
  if (bytesRead != length * 2) {
    MIP_DEBUG_ERROR_PRINTF("MiP: OOB too short: %d, %d", bytesRead, length * 2);
    return;
  }

  // Convert the hex data to a binary response.
  uint8_t response[MIP_RESPONSE_MAX_LEN];
  response[0] = commandByte;
  copyHexTextToBinary(&response[1], buffer, length);

  // Send the event back to the parent class to be dispatched to the correct
  // handler.
  m_mip.dispatchEvent(commandByte, response, length + 1);
}

uint8_t MiP_Serial::discardUnexpectedSerialData() {
  uint8_t discardedBytes = 0;

  // Unexpected response data encountered. Throw away all data in serial buffer
  // since it is hard to tell where next response begins.
  while (Serial.available() > 0) {
    discardedBytes++;
    Serial.read();

    // Delay long enough for next serial byte to be received if MiP is still
    // actively sending at 115200 baud.
    delayMicroseconds(100);
  }
  return discardedBytes;
}

void MiP_Serial::copyHexTextToBinary(uint8_t* pDest,
                                     uint8_t* pSrc,
                                     uint8_t length) {
  while (length-- > 0) {
    *pDest = (parseHexDigit(pSrc[0]) << 4) | parseHexDigit(pSrc[1]);
    pDest++;
    pSrc += 2;
  }
}

uint8_t MiP_Serial::parseHexDigit(uint8_t digit) {
  if (digit >= '0' && digit <= '9')
    return digit - '0';
  if (digit >= 'a' && digit <= 'f')
    return digit - 'a' + 10;
  if (digit >= 'A' && digit <= 'F')
    return digit - 'A' + 10;
  return 0;
}
