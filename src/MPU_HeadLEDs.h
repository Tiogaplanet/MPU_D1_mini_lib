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
 * @brief Individual head LED lighting and blinking patterns.
 */
enum MiPHeadLED : uint8_t {
  MIP_HEAD_LED_OFF = 0,         ///< Head LED is turned off.
  MIP_HEAD_LED_ON = 1,          ///< Head LED is continuously turned on.
  MIP_HEAD_LED_BLINK_SLOW = 2,  ///< Head LED blinks at a slow rate.
  MIP_HEAD_LED_BLINK_FAST = 3,  ///< Head LED blinks at a fast rate.
};

/**
 * @brief Encapsulates the lighting patterns of all four head (eye) LEDs.
 *
 * @details Stores the configuration state for each individual head LED
 * (LED 1 through LED 4).
 */
class MiPHeadLEDs {
public:
  /**
   * @brief Constructs a new MiPHeadLEDs container and initializes all LEDs to
   * off.
   *
   * @details Automatically calls clear() to set led1, led2, led3, and led4 to
   * MIP_HEAD_LED_OFF.
   */
  MiPHeadLEDs() {
    clear();
  }

  /**
   * @brief Resets all four head LED states back to MIP_HEAD_LED_OFF.
   */
  void clear() {
    led1 = MIP_HEAD_LED_OFF;
    led2 = MIP_HEAD_LED_OFF;
    led3 = MIP_HEAD_LED_OFF;
    led4 = MIP_HEAD_LED_OFF;
  }

  MiPHeadLED led1;  ///< Pattern state for Head LED 1.
  MiPHeadLED led2;  ///< Pattern state for Head LED 2.
  MiPHeadLED led3;  ///< Pattern state for Head LED 3.
  MiPHeadLED led4;  ///< Pattern state for Head LED 4.
};

/**
 * @brief Manages MiP's eye/head LEDs.
 */
class MiP_HeadLEDs {
public:
  /**
   * @brief Reads the current state of all four head LEDs.
   *
   * @details Sends the get head LEDs request over UART and populates @p
   * headLEDs with the active lighting pattern of each LED. Performs retries
   * automatically on communication failure.
   *
   * @param[out] headLEDs Reference to a MiPHeadLEDs struct to populate with
   * retrieved states.
   */
  void read(MiPHeadLEDs& headLEDs);

  /**
   * @brief Sets all four head LEDs and verifies the change.
   *
   * @details Sends the set command and immediately reads back the state to
   * confirm success. Retries automatically on mismatch or communication error.
   *
   * @param led1 Head LED 1 pattern.
   * @param led2 Head LED 2 pattern.
   * @param led3 Head LED 3 pattern.
   * @param led4 Head LED 4 pattern.
   */
  void write(MiPHeadLED led1, MiPHeadLED led2, MiPHeadLED led3, MiPHeadLED led4);

  /**
   * @brief Sets all four head LEDs using a struct and verifies the change.
   *
   * @details Sends the set command and immediately reads back the state to
   * confirm success. Retries automatically on mismatch or communication error.
   *
   * @param headLEDs Struct containing the four LED patterns.
   */
  void write(const MiPHeadLEDs& headLEDs);

  /**
   * @brief Sets all four head LEDs without verification (fire-and-forget).
   *
   * @details Faster than the verified version because it does not perform a
   * read-back check, but provides no confirmation that the command succeeded on
   * hardware.
   *
   * @param led1 Head LED 1 pattern.
   * @param led2 Head LED 2 pattern.
   * @param led3 Head LED 3 pattern.
   * @param led4 Head LED 4 pattern.
   */
  void unverifiedWrite(MiPHeadLED led1, MiPHeadLED led2, MiPHeadLED led3, MiPHeadLED led4);

  /**
   * @brief Sets all four head LEDs using a struct without verification
   * (fire-and-forget).
   *
   * @details Faster than the verified version because it does not perform a
   * read-back check, but provides no confirmation that the command succeeded on
   * hardware.
   *
   * @param headLEDs Struct containing the four LED patterns.
   */
  void unverifiedWrite(const MiPHeadLEDs& headLEDs);

protected:
  /**
   * @brief MiP protocol command byte to set the patterns of all four head LEDs.
   */
  static constexpr uint8_t MIP_CMD_SET_HEAD_LEDS = 0x8A;

  /**
   * @brief MiP protocol command byte to query the current patterns of all four
   * head LEDs.
   */
  static constexpr uint8_t MIP_CMD_GET_HEAD_LEDS = 0x8B;

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   *
   * @param mip A reference to the main MiP object to access core communication
   * services.
   */
  explicit MiP_HeadLEDs(MiP& mip);

  int8_t rawGet(MiPHeadLEDs& headLEDs);
  void rawSet(MiPHeadLED led1, MiPHeadLED led2, MiPHeadLED led3, MiPHeadLED led4);
  bool isValidSingleLED(uint8_t led);

  MiP& m_mip;  // Stores a reference to the main MiP class.

  /**
   * @brief Allows MiP to call private constructor.
   */
  friend class MiP;
};

#endif  // MPU_HEADLEDS_H
