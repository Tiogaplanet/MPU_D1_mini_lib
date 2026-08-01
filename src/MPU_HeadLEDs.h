/**
 * @file MPU_HeadLEDs.h
 * @brief Defines the public interface for head LED control in the MiP library.
 *
 * @details This header declares the head LED API used to configure MiP's eye
 * patterns.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_HEADLEDS_H
#define MPU_HEADLEDS_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Individual head LED patterns.
 */
enum MiPHeadLED : uint8_t {
  MIP_HEAD_LED_OFF = 0,
  MIP_HEAD_LED_ON = 1,
  MIP_HEAD_LED_BLINK_SLOW = 2,
  MIP_HEAD_LED_BLINK_FAST = 3,
};

/**
 * @brief State of all four head LEDs.
 */
class MiPHeadLEDs {
 public:
  MiPHeadLEDs() {
    clear();
  }
  void clear() {
    led1 = MIP_HEAD_LED_OFF;
    led2 = MIP_HEAD_LED_OFF;
    led3 = MIP_HEAD_LED_OFF;
    led4 = MIP_HEAD_LED_OFF;
  }
  MiPHeadLED led1;
  MiPHeadLED led2;
  MiPHeadLED led3;
  MiPHeadLED led4;
};

/**
 * @brief Manages MiP's eye LEDs.
 */
class MiP_HeadLEDs {
 public:
  /**
   * @brief MiP protocol command bytes used by the head LED subsystem.
   *
   * These values are placed in the first byte of requests sent to the MiP
   * (and appear in the corresponding responses).  See the official
   * [MiP BLE
   * Protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md)
   * for the complete list.
   */
  static constexpr uint8_t MIP_CMD_SET_HEAD_LEDS = 0x8A;
  static constexpr uint8_t MIP_CMD_GET_HEAD_LEDS = 0x8B;

  /**
   * @brief Constructs the eye LED manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_HeadLEDs(MiP& mip);

  /**
   * @brief Reads the current state of all four head LEDs.
   *
   * Performs a verified read with retries on communication failure.
   *
   * @param headLEDs Reference to a MiPHeadLEDs struct to fill.
   */
  void read(MiPHeadLEDs& headLEDs);

  /**
   * @brief Sets all four head LEDs and verifies the change.
   *
   * Sends the command and reads back the state to confirm success.
   * Retries automatically on mismatch or error.
   *
   * @param led1 Head LED 1 pattern.
   * @param led2 Head LED 2 pattern.
   * @param led3 Head LED 3 pattern.
   * @param led4 Head LED 4 pattern.
   */
  void write(MiPHeadLED led1,
             MiPHeadLED led2,
             MiPHeadLED led3,
             MiPHeadLED led4);

  /**
   * @brief Sets all four head LEDs using a struct and verifies the change.
   *
   * @param headLEDs Struct containing the four LED patterns.
   */
  void write(const MiPHeadLEDs& headLEDs);

  /**
   * @brief Sets all four head LEDs without verification (fire-and-forget).
   *
   * Faster than the verified version but provides no confirmation that the
   * command succeeded.
   *
   * @param led1 Head LED 1 pattern.
   * @param led2 Head LED 2 pattern.
   * @param led3 Head LED 3 pattern.
   * @param led4 Head LED 4 pattern.
   */
  void unverifiedWrite(MiPHeadLED led1,
                       MiPHeadLED led2,
                       MiPHeadLED led3,
                       MiPHeadLED led4);

  /**
   * @brief Sets all four head LEDs using a struct without verification.
   *
   * @param headLEDs Struct containing the four LED patterns.
   */
  void unverifiedWrite(const MiPHeadLEDs& headLEDs);

 private:
  int8_t rawGet(MiPHeadLEDs& headLEDs);
  void rawSet(MiPHeadLED led1,
              MiPHeadLED led2,
              MiPHeadLED led3,
              MiPHeadLED led4);
  bool isValidSingleLED(uint8_t led);

  MiP& m_mip;  // Stores a reference to the main MiP class.
};

#endif  // MPU_HEADLEDS_H
