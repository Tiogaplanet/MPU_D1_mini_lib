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
 */
class MiPChestLED {
 public:
  MiPChestLED() {
    clear();
  }
  void clear() {
    onTime = 0;
    offTime = 0;
    red = 0;
    green = 0;
    blue = 0;
  }
  uint16_t onTime;
  uint16_t offTime;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

/**
 * @brief Manages MiP's chest LED.
 */
class MiP_ChestLED {
 public:
  /**
   * @brief MiP protocol command bytes used by the chest LED subsystem.
   *
   * These values are placed in the first byte of requests sent to the MiP
   * (and appear in the corresponding responses).  See the official
   * [MiP BLE
   * Protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol/blob/master/MiP-Protocol.md)
   * for the complete list.
   */
  static constexpr uint8_t MIP_CMD_GET_CHEST_LED = 0x83;
  static constexpr uint8_t MIP_CMD_SET_CHEST_LED = 0x84;
  static constexpr uint8_t MIP_CMD_FLASH_CHEST_LED = 0x89;

  /**
   * @brief Constructs the chest LED manager.
   * @param mip A reference to the main MiP object to access core services.
   */
  MiP_ChestLED(MiP& mip);

  /**
   * @brief Reads the current RGB state and flash timings of the chest LED.
   * * @param chestLED A reference to a MiPChestLED object where the retrieved
   * data will be stored.
   */
  void read(MiPChestLED& chestLED);

  /**
   * @brief Sets the chest LED to a solid RGB color and verifies the change.
   * * Sends the set command and immediately reads the state back from the MiP
   * to ensure the color was successfully updated. Retries upon failure.
   * * @param red   Intensity for the red channel (0-255).
   * @param green Intensity for the green channel (0-255).
   * @param blue  Intensity for the blue channel (0-255). Note: MiP truncates
   * the lower 2 bits.
   */
  void write(uint8_t red, uint8_t green, uint8_t blue);

  /**
   * @brief Sets the chest LED to flash an RGB color at a specific interval and
   * verifies the change.
   * * @param red     Intensity for the red channel (0-255).
   * @param green   Intensity for the green channel (0-255).
   * @param blue    Intensity for the blue channel (0-255).
   * @param onTime  Time in milliseconds the LED stays on. (Converted internally
   * to 20ms ticks).
   * @param offTime Time in milliseconds the LED stays off. (Converted
   * internally to 20ms ticks).
   */
  void write(uint8_t red,
             uint8_t green,
             uint8_t blue,
             uint16_t onTime,
             uint16_t offTime);

  /**
   * @brief Sets the chest LED to flash an RGB color at a specific interval and
   * verifies the change.
   * * @param chestLED A MiPChestLED instance.
   */
  void write(const MiPChestLED& chestLED);

  /**
   * @brief Sets the chest LED to a solid RGB color without verifying the
   * change.
   * * This is a "fire-and-forget" method. It sends the command but does not
   * read back the state to check for success, making it faster but less
   * reliable than writeChestLED().
   * * @param red   Intensity for the red channel (0-255).
   * @param green Intensity for the green channel (0-255).
   * @param blue  Intensity for the blue channel (0-255).
   */
  void unverifiedWrite(uint8_t red, uint8_t green, uint8_t blue);

  /**
   * @brief Sets the chest LED to a solid RGB color without verifying the
   * change.
   * * This is a "fire-and-forget" method. It sends the command but does not
   * read back the state to check for success, making it faster but less
   * reliable than writeChestLED().
   * * @param red   Intensity for the red channel (0-255).
   * @param green Intensity for the green channel (0-255).
   * @param blue  Intensity for the blue channel (0-255).
   * @param onTime  Time in milliseconds the LED stays on. (Converted internally
   * to 20ms ticks).
   * @param offTime Time in milliseconds the LED stays off. (Converted
   * internally to 20ms ticks).
   */
  void unverifiedWrite(uint8_t red,
                       uint8_t green,
                       uint8_t blue,
                       uint16_t onTime,
                       uint16_t offTime);

  /**
   * @brief Sets the chest LED to a solid RGB color without verifying the
   * change.
   * * This is a "fire-and-forget" method. It sends the command but does not
   * read back the state to check for success, making it faster but less
   * reliable than writeChestLED().
   * * @param chestLED   An instance of MiPChestLED.
   */
  void unverifiedWrite(const MiPChestLED& chestLED);

 private:
  int8_t rawGet(MiPChestLED& chestLED);
  void rawSet(uint8_t red, uint8_t green, uint8_t blue);
  void rawFlash(uint8_t red,
                uint8_t green,
                uint8_t blue,
                uint8_t onTime,
                uint8_t offTime);

  MiP& m_mip;  // Stores a reference to the main MiP class.
};

#endif  // MPU_CHESTLED_H
