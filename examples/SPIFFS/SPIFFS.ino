/**
 * @file SPIFFS.ino
 * @brief Demonstrates reading and writing the D1 mini's SPI flash file system (SPIFFS)
 *        and using the MiP chest LED to indicate success.
 *
 * @details
 * This example mounts the SPIFFS filesystem, writes a short "password" string to
 * a file, reads the file back, compares the read value to the original, and
 * uses the MiP chest LED to indicate whether the read matched the write.
 * If the file contents match the original password the chest LED is set to
 * violet; otherwise it is set to red. After a short delay the chest LED is
 * restored to green and the temporary file is removed.
 *
 * The sketch demonstrates these operations and APIs:
 *   - SPIFFS.begin(), SPIFFS.open(), File::println(), File::readStringUntil(), SPIFFS.remove()
 *   - MiP::begin(), MiP::writeChestLED()
 *
 * Typical usage:
 *   - Load this sketch onto an MPU: D1 mini.
 *   - Observe Serial1 output for progress and the MiP chest LED for pass/fail.
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
#include "FS.h"

/**
 * @brief Global MiP instance used to control the robot.
 *
 * @details Use this object to call MiP API functions such as begin() and
 * writeChestLED(). Keeping the instance at file scope makes it available
 * throughout setup() and loop().
 */
MiP mip;

/**
 * @brief Arduino setup function.
 *
 * @details
 * - Initializes the MiP connection via mip.begin().
 * - Mounts the SPIFFS filesystem with SPIFFS.begin().
 * - Creates or overwrites "/f.txt" and writes a password string to it.
 * - Reopens the file, reads the stored line, trims whitespace, and compares
 *   it to the original password.
 * - Sets the MiP chest LED to violet on match or red on mismatch.
 * - Deletes the temporary file and restores the chest LED to green after a delay.
 *
 * The function prints progress and diagnostic messages to Serial1 so the
 * user can observe each step of the demonstration.
 */
void setup() {
  String password = "1234secret";

  bool connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("SPIFFS.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("SPIFFS.ino: Read and write the the SPI flash file system (SPIFFS).\n"));
  Serial1.println(F(" Chest turns violet if the read matches the write, else red.\n"));

  // Mount the SPI flash filesystem. Report success or failure to Serial1.
  (SPIFFS.begin()) ? Serial1.println(F(" SPIFFS opened.")) : Serial1.println(F("\n\nSPIFFS failed to open."));

  // Open the file in write mode (creates or truncates).
  File f = SPIFFS.open("/f.txt", "w");
  if (!f) {
    Serial1.println(F(" SPIFFS.ino: File creation failed."));
  } else {
    // Write the password followed by newline and close the file.
    f.println(password);
    f.close();
  }

  // Reopen the file in read mode to verify contents.
  f = SPIFFS.open("/f.txt", "r");
  String line;

  if (!f) {
    Serial1.println(F(" SPIFFS.ino: Failed to open file for reading."));
  } else {
    // Read the file line-by-line until EOF. For this example the file contains one line.
    while (f.available()) {
      line = f.readStringUntil('\n');
    }
    // Remove leading/trailing whitespace and newline characters.
    line.trim();

    // Print both the original password and the file contents for debugging.
    Serial1.println(" Password is " + password + ".");
    Serial1.println(" File contained " + line + ".");

    // If the file contents match the original password, set chest LED to violet,
    // otherwise set it to red to indicate a mismatch.
    if (line == password) {
      // Violet: R=0xB6, G=0x00, B=0xFF
      mip.writeChestLED(0xB6, 0x00, 0xFF);
    } else {
      // Red: R=0xFF, G=0x00, B=0x00
      mip.writeChestLED(0xFF, 0x00, 0x00);
    }

    f.close();
  }

  // Remove the temporary file and report the result.
  (SPIFFS.remove("/f.txt")) ? Serial1.println(F(" File deleted.")) : Serial1.println(F(" Error deleting file."));

  // Wait so the user can observe the chest LED color, then restore to green.
  delay(5000);
  mip.writeChestLED(0x00, 0xFF, 0x00);

  Serial1.println(F("SPIFFS.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs its demonstration in setup() and does not
 * require repeated work in loop(). The function is intentionally left empty
 * so the sketch completes once during initialization.
 */
void loop() {
}
