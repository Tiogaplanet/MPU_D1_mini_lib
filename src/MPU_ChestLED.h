/**
 * @file MPU_ChestLED.h
 * @brief Defines the public interface for chest LED control in the MiP library.
 *
 * @details This header declares the API used to configure and query MiP's
 * three-color chest LED.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_CHESTLED_H
#define MPU_CHESTLED_H

#include <stdint.h>

// Forward-declare the main MiP class to avoid circular include dependencies.
class MiP;

/**
 * @brief Chest LED color and flash timing configuration.
 *
 * @details Encapsulates the 8-bit RGB color intensities (Red, Green, Blue) and
 * flash timing intervals (onTime and offTime in milliseconds) for MiP's chest
 * LED.
 */
class MiPChestLED {
public:
  /**
   * @brief Constructs a new MiPChestLED configuration object.
   *
   * @details Automatically calls clear() to set all color intensity values and
   * flash timing durations to zero (LED off).
   */
  MiPChestLED() {
    clear();
  }

  /**
   * @brief Resets all RGB channel intensities and flash timing durations to
   * zero.
   */
  void clear() {
    onTime = 0;
    offTime = 0;
    red = 0;
    green = 0;
    blue = 0;
  }

  uint16_t onTime;   ///< Flash ON duration in milliseconds.
  uint16_t offTime;  ///< Flash OFF duration in milliseconds.
  uint8_t red;       ///< Red channel intensity (0-255).
  uint8_t green;     ///< Green channel intensity (0-255).
  uint8_t blue;  ///< Blue channel intensity (0-255; hardware truncates lower 2
                 ///< bits).
};

/**
 * @brief Manages MiP's three-color chest LED.
 */
class MiP_ChestLED {
public:
  /**
   * @brief Reads the current RGB state and flash timings of MiP's chest LED.
   *
   * @param[out] chestLED Reference to a MiPChestLED object where retrieved data
   * will be stored.
   */
  void read(MiPChestLED& chestLED);

  /**
   * @brief Sets MiP's chest LED to a solid RGB color and verifies the change.
   *
   * @details Sends the set command and reads back the state from MiP to confirm
   * the color was updated.
   *
   * @param red   Red channel intensity (0-255).
   * @param green Green channel intensity (0-255).
   * @param blue  Blue channel intensity (0-255; lower 2 bits truncated by MiP).
   */
  void write(uint8_t red, uint8_t green, uint8_t blue);

  /**
   * @brief Sets MiP's chest LED to flash an RGB color at a specific interval
   * and verifies the change.
   *
   * @param red     Red channel intensity (0-255).
   * @param green   Green channel intensity (0-255).
   * @param blue    Blue channel intensity (0-255).
   * @param onTime  Duration in milliseconds the LED stays on (converted to 20ms
   * ticks).
   * @param offTime Duration in milliseconds the LED stays off (converted to
   * 20ms ticks).
   */
  void write(uint8_t red, uint8_t green, uint8_t blue, uint16_t onTime, uint16_t offTime);

  /**
   * @brief Sets MiP's chest LED using a MiPChestLED object and verifies the
   * change.
   *
   * @param chestLED Reference to a MiPChestLED configuration object.
   */
  void write(const MiPChestLED& chestLED);

  /**
   * @brief Sets MiP's chest LED to a solid RGB color without verification
   * (fire-and-forget).
   *
   * @param red   Red channel intensity (0-255).
   * @param green Green channel intensity (0-255).
   * @param blue  Blue channel intensity (0-255).
   */
  void unverifiedWrite(uint8_t red, uint8_t green, uint8_t blue);

  /**
   * @brief Sets MiP's chest LED to flash an RGB color without verification
   * (fire-and-forget).
   *
   * @param red     Red channel intensity (0-255).
   * @param green   Green channel intensity (0-255).
   * @param blue    Blue channel intensity (0-255).
   * @param onTime  Duration in milliseconds the LED stays on.
   * @param offTime Duration in milliseconds the LED stays off.
   */
  void unverifiedWrite(
    uint8_t red, uint8_t green, uint8_t blue, uint16_t onTime, uint16_t offTime);

  /**
   * @brief Sets MiP's chest LED using a MiPChestLED object without verification
   * (fire-and-forget).
   *
   * @param chestLED Reference to a MiPChestLED configuration object.
   */
  void unverifiedWrite(const MiPChestLED& chestLED);

protected:
  static constexpr uint8_t MIP_CMD_GET_CHEST_LED = 0x83;  ///< Protocol command
                                                          ///< byte to query
                                                          ///< chest LED state.
  static constexpr uint8_t MIP_CMD_SET_CHEST_LED = 0x84;  ///< Protocol command
                                                          ///< byte to set solid
                                                          ///< chest LED.
  static constexpr uint8_t MIP_CMD_FLASH_CHEST_LED = 0x89;  ///< Protocol
                                                            ///< command byte to
                                                            ///< flash chest
                                                            ///< LED.

private:
  /**
   * @brief Private constructor; instantiated strictly by MiP orchestrator.
   *
   * @param mip Reference to the main MiP object for core communication
   * services.
   */
  explicit MiP_ChestLED(MiP& mip);

  int8_t rawGet(MiPChestLED& chestLED);
  void rawSet(uint8_t red, uint8_t green, uint8_t blue);
  void rawFlash(uint8_t red, uint8_t green, uint8_t blue, uint8_t onTicks, uint8_t offTicks);

  MiP& m_mip;

  friend class MiP;
};

#endif  // MPU_CHESTLED_H
