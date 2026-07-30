/**
 * @file ChestLED.ino
 * @brief Example sketch demonstrating MiP chest LED read/write operations.
 *
 * @details This sketch shows how to use the MiP library to set and read the chest LED
 * color and blink timing. It demonstrates the verified write APIs
 * (readChestLED, writeChestLED) and the unverified write APIs
 * (unverifiedWriteChestLED) which may not always be accepted by the robot.
 *
 * The example exercises these API calls:
 *   - readChestLED()
 *   - writeChestLED()
 *   - unverifiedChestLED()
 *
 * @copyright Copyright (C) 2018-2026 Adam Green (https://github.com/adamgreen)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <MPU_D1_mini.h>

/**
 * @brief Global MiP instance used to communicate with the robot.
 *
 * This object is used throughout the sketch to call MiP API functions such as
 * begin(), readChestLED(), writeChestLED(), and unverifiedWriteChestLED().
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * Initializes communication with the MiP robot and demonstrates several
 * chest LED operations:
 *  - Set a static color (magenta) with no timing specified.
 *  - Set a blinking color with explicit on/off times.
 *  - Set a static color using a MiPChestLED struct.
 *  - Repeat the above sequence using the unverified write APIs.
 *
 * The function prints status messages to Serial1 and uses
 * printCurrentChestLEDSetting() to display the current chest LED state after
 * each verified write.
 */
void setup() {
  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("ChestLED.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("ChestLED.ino: Set Chest LED to different colors."));

  Serial1.println(F(" Set chest LED to magenta, no time specified."));
  uint8_t red = 0xff;
  uint8_t green = 0x01;
  uint8_t blue = 0xfe;
  mip.chestLED.write(red, green, blue);
  printCurrentChestLEDSetting();
  delay(1000);

  Serial1.println(F(" Set chest LED to blink red, on time: 990, off time: 989."));
  red = 0xff;
  green = 0x01;
  blue = 0x05;
  const uint16_t onTime = 990;
  const uint16_t offTime = 989;
  mip.chestLED.write(red, green, blue, onTime, offTime);
  printCurrentChestLEDSetting();
  delay(4000);

  Serial1.println(F(" Set chest LED back to green, no time specified."));
  MiPChestLED chestLED;
  chestLED.red = 0x00;
  chestLED.green = 0xff;
  chestLED.blue = 0x00;
  chestLED.onTime = 0;
  chestLED.offTime = 0;
  mip.chestLED.write(chestLED);
  printCurrentChestLEDSetting();
  delay(1000);

  // Attempt to run through the same sequence of chest LED changes using the
  // unverifiedWriteChestLED() functions which don't always get accepted by MiP.
  Serial1.println(F(" Trying to set chest LED to magenta, no time specified."));
  red = 0xff;
  green = 0x01;
  blue = 0xfe;
  mip.chestLED.unverifiedWrite(red, green, blue);
  delay(1000);

  Serial1.println(F(" Trying to set chest LED to blink red, no time specified."));
  red = 0xff;
  green = 0x01;
  blue = 0x05;
  mip.chestLED.unverifiedWrite(red, green, blue, onTime, offTime);
  delay(4000);

  Serial1.println(F(" Trying to set chest LED back to green, no time specified."));
  chestLED.red = 0x00;
  chestLED.green = 0xff;
  chestLED.blue = 0x00;
  chestLED.onTime = 0;
  chestLED.offTime = 0;
  mip.chestLED.unverifiedWrite(chestLED);
  delay(1000);

  Serial1.println();
  Serial1.println(F("ChestLED.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * This example performs all actions in setup() and does not require repeated
 * work in loop(). The function is intentionally left empty.
 */
void loop() {
}

/**
 * @brief Print the current chest LED setting to Serial1.
 *
 * Reads the current chest LED state from the MiP robot using readChestLED()
 * and prints the red, green, blue, onTime, and offTime values in a human
 * readable format.
 *
 * @note This function is declared static to limit its linkage to this file.
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
  Serial1.println();
}

