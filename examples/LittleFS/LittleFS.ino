/**
 * @file LittleFS.ino
 * @brief Robust LittleFS read/write example with MiP chest-LED verification.
 *
 * @details
 * This sketch is a LittleFS-based rewrite of the original SPIFFS example.
 * It mounts the LittleFS filesystem onboard the D1 mini, writes a short password to a temporary
 * file, reads it back, compares the contents, and indicates success or
 * failure using the MiP chest LED:
 *   - Violet when the read matches the written password.
 *   - Red when the read does not match or an error occurs.
 * After verification, the temporary file is removed and the chest LED is
 * restored to green.
 *
 * Safety and robustness improvements:
 *   - Verifies return values from LittleFS.begin(), LittleFS.open(), and
 *     LittleFS.remove().
 *   - Checks File objects before reading/writing.
 *   - Prints clear diagnostic messages to Serial1.
 *
 * Demonstrates these APIs:
 *   - begin()
 *   - chestLED.write()
 *   - LittleFS.begin(), LittleFS.open(), LittleFS.remove()
 *
 * Notes:
 *   - LittleFS is provided by the ESP8266 core for Wemos D1 mini boards.
 *
 * @copyright Copyright (C) 2018 Samuel Trassare (https://github.com/Tiogaplanet)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <MPU_D1_mini.h>
#include <FS.h>
#include <LittleFS.h>

/**
 * @brief Global MiP instance used to communicate with the robot.
 *
 * @details Use this object to call MiP API functions such as begin(), etc.
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details
 * - Initializes Serial1 for diagnostics.
 * - Attempts to initialize the MiP connection via mip.begin() and sets the
 *   global `connectResult` flag.
 * - Mounts LittleFS and verifies success.
 * - Writes a password to /f.txt, reads it back, trims whitespace, compares
 *   to the original, and sets the chest LED to violet on match or red on mismatch.
 * - Deletes the temporary file and restores the chest LED to green after a delay.
 *
 * If MiP connection or LittleFS mount fails, the function halts in a safe loop
 * after printing an error so loop() will not run and cause further errors.
 */
void setup() {
  // Initialize MiP and record result in global flag.
  bool connectResult = mip.begin();

  if (!connectResult) {
    Serial1.println(F("LittleFS.ino: Failed connecting to MiP!"));
    return;
  }

  const String password = "1234secret";

  Serial1.println(F("LittleFS.ino: Read and write the LittleFS flash file system."));
  Serial1.println(F("Chest turns violet if the read matches the write, else red."));

  // Mount the LittleFS filesystem and verify success.
  if (!LittleFS.begin()) {
    Serial1.println(F("LittleFS.ino: LittleFS failed to mount."));
    // Indicate error on chest LED (red) and stop.
    mip.chestLED.write(0xFF, 0x00, 0x00);
    while (true) {
      delay(1000);
    }
  }
  Serial1.println(F("LittleFS mounted."));

  // Write the password to a temporary file.
  {
    File f = LittleFS.open("/f.txt", "w");
    if (!f) {
      Serial1.println(F("LittleFS.ino: File creation failed."));
    } else {
      f.println(password);
      f.close();
      Serial1.println(F("LittleFS.ino: Wrote password to /f.txt"));
    }
  }

  // Read the file back and compare.
  String line;
  {
    File f = LittleFS.open("/f.txt", "r");
    if (!f) {
      Serial1.println(F("LittleFS.ino: Failed to open /f.txt for reading."));
      // Indicate error on chest LED (red).
      mip.chestLED.write(0xFF, 0x00, 0x00);
    } else {
      // Read the first line (file contains one line in this example).
      line = f.readStringUntil('\n');
      f.close();

      line.trim();

      Serial1.print(F(" Password is "));
      Serial1.println(password);
      Serial1.print(F(" File contained "));
      Serial1.println(line);

      if (line == password) {
        // Violet: R=0xB6, G=0x00, B=0xFF
        mip.chestLED.write(0xB6, 0x00, 0xFF);
        Serial1.println(F(" LittleFS.ino: Read matches write. Chest set to violet."));
      } else {
        // Red: R=0xFF, G=0x00, B=0x00
        mip.chestLED.write(0xFF, 0x00, 0x00);
        Serial1.println(F(" LittleFS.ino: Read does NOT match write. Chest set to red."));
      }
    }
  }

  // Attempt to remove the temporary file and report result.
  if (LittleFS.remove("/f.txt")) {
    Serial1.println(F(" File deleted."));
  } else {
    Serial1.println(F(" Error deleting file /f.txt."));
  }

  // Allow the user to observe the chest LED color, then restore to green.
  delay(5000);
  mip.chestLED.write(0x00, 0xFF, 0x00);
  Serial1.println(F("LittleFS.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details
 * This example performs its demonstration in setup() and does not require
 * repeated work in loop().
 */
void loop() {
}
