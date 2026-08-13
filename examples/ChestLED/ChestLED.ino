/**
 * @file ChestLED.ino
 * @brief Example sketch demonstrating MiP chest LED read and write operations.
 *
 * @details This sketch shows how to set and read MiP's chest LED color and
 * blink timing. It demonstrates verified write APIs (chestLED.read() and
 * chestLED.write()) and unverified fire-and-forget write APIs
 * (chestLED.unverifiedWrite()), which send commands without read-back checks.
 *
 * The example exercises these API calls:
 *   - chestLED.read()
 *   - chestLED.write()
 *   - chestLED.unverifiedWrite()
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MiP_Power_Up_-_D1_mini.h>

/**
 * @brief Global MiP instance used to communicate with MiP.
 *
 * This object is used throughout the sketch to call MiP API functions such as
 * begin(), chestLED.read(), chestLED.write(), and chestLED.unverifiedWrite().
 */
MiP mip;

/**
 * @brief Stores the result of the MiP initialization attempt.
 */
bool connectResult;

/**
 * @brief Prints the current chest LED setting to Serial1.
 *
 * @details Reads the current chest LED state from MiP using chestLED.read()
 * and prints the red, green, blue, onTime, and offTime values in a
 * human-readable format.
 */
static void printCurrentChestLEDSetting() {
  MiPChestLED chestLED;
  mip.chestLED.read(chestLED);

  Serial1.println(F(" Current Chest LED Setting"));
  Serial1.print(F("    red: "));
  Serial1.println(chestLED.red);
  Serial1.print(F("    green: "));
  Serial1.println(chestLED.green);
  Serial1.print(F("    blue: "));
  Serial1.println(chestLED.blue);
  Serial1.print(F("    on time: "));
  Serial1.print(chestLED.onTime);
  Serial1.println(F(" milliseconds"));
  Serial1.print(F("    off time: "));
  Serial1.print(chestLED.offTime);
  Serial1.println(F(" milliseconds"));
}

/**
 * @brief Arduino setup function.
 *
 * @details Initializes communication with MiP and demonstrates several
 * chest LED operations:
 *  - Setting a static color (magenta) with no flash timing specified.
 *  - Setting a blinking color (red) with explicit on/off times.
 *  - Setting a static color (green) using a MiPChestLED struct.
 *  - Repeating the sequence using unverified write APIs.
 *
 * The function prints status messages to Serial1 and uses
 * printCurrentChestLEDSetting() to display the current chest LED state after
 * each verified write.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("ChestLED.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("ChestLED.ino: Set Chest LED to different colors."));

  // 1. Set static color (magenta) using verified write
  Serial1.println(F(" Set chest LED to magenta, no time specified."));
  uint8_t red = 0xFF;
  uint8_t green = 0x01;
  uint8_t blue = 0xFE;
  mip.chestLED.write(red, green, blue);
  printCurrentChestLEDSetting();
  delay(1000);

  // 2. Set flashing color (red) using verified write
  Serial1.println(F(" Set chest LED to blink red, on time: 990ms, off time: 989ms."));
  red = 0xFF;
  green = 0x01;
  blue = 0x05;
  const uint16_t onTime = 990;
  const uint16_t offTime = 989;
  mip.chestLED.write(red, green, blue, onTime, offTime);
  printCurrentChestLEDSetting();
  delay(4000);

  // 3. Set static color (green) using a MiPChestLED struct
  Serial1.println(F(" Set chest LED back to green, no time specified."));
  MiPChestLED chestLED;
  chestLED.red = 0x00;
  chestLED.green = 0xFF;
  chestLED.blue = 0x00;
  chestLED.onTime = 0;
  chestLED.offTime = 0;
  mip.chestLED.write(chestLED);
  printCurrentChestLEDSetting();
  delay(1000);

  // 4. Repeat sequence using unverified write (fire-and-forget) APIs
  Serial1.println(F(" Trying to set chest LED to magenta, no time specified."));
  red = 0xFF;
  green = 0x01;
  blue = 0xFE;
  mip.chestLED.unverifiedWrite(red, green, blue);
  delay(1000);

  Serial1.println(F(" Trying to set chest LED to blink red, with time specified."));
  red = 0xFF;
  green = 0x01;
  blue = 0x05;
  mip.chestLED.unverifiedWrite(red, green, blue, onTime, offTime);
  delay(4000);

  Serial1.println(F(" Trying to set chest LED back to green with a flash time."));
  chestLED.red = 0x00;
  chestLED.green = 0xFF;
  chestLED.blue = 0x00;
  chestLED.onTime = 200;
  chestLED.offTime = 200;
  mip.chestLED.unverifiedWrite(chestLED);
  delay(1000);

  // Restore solid green state at the end of the test
  Serial1.println(F(" Set chest LED back to solid green."));
  chestLED.red = 0x00;
  chestLED.green = 0xFF;
  chestLED.blue = 0x00;
  chestLED.onTime = 0;
  chestLED.offTime = 0;
  mip.chestLED.write(chestLED);

  Serial1.println(F("ChestLED.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * This example performs all actions in setup() and does not require repeated
 * work in loop().
 */
void loop() {
  if (!connectResult) { return; }
}
