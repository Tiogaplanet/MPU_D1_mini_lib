/**
 * @file Volume.ino
 * @brief Example sketch demonstrating reading and writing MiP's audio volume.
 *
 * @details
 * This sketch connects to MiP and demonstrates how to set speaker volume using
 * sound.writeVolume() and read the active volume level back using
 * sound.readVolume(). The example first sets the volume to the predefined
 * constant MIP_VOLUME_OFF (mute), prints the resulting volume level to Serial1,
 * and then restores the volume back to MIP_VOLUME_7 (maximum).
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.sound.writeVolume()
 *   - mip.sound.readVolume()
 *
 * Usage notes:
 *   - Ensure MiP is powered and able to accept UART commands.
 *   - Adjust the volume value passed to writeVolume() to experiment with
 *     different audio levels supported by MiP (0 to 7).
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
 * @details Use this object to call MiP API functions such as begin(),
 * sound.writeVolume(), and sound.readVolume(). Keeping the instance at file
 * scope makes it available in both setup() and loop().
 */
MiP mip;

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Arduino setup function.
 *
 * @details
 * - Initializes the MiP connection via mip.begin().
 * - If the connection fails, prints an error to Serial1 and returns early.
 * - On success, sets MiP's volume to MIP_VOLUME_OFF using writeVolume(),
 *   reads the current volume back with readVolume(), and prints the value to
 *   Serial1 for verification.
 * - Restores volume back to MIP_VOLUME_7 and reads back again to verify.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("Volume.ino: Failed connecting to MiP."));
    return;
  }

  Serial1.println(F("Volume.ino: Use sound.readVolume() and sound.writeVolume(). Set "
                    "volume level to off (0) and read out afterwards."));

  // 1. Set speaker volume to the predefined "off" constant (mute)
  Serial1.println(F(" Setting volume to MIP_VOLUME_OFF (0)..."));
  mip.sound.writeVolume(MIP_VOLUME_OFF);

  // Read the current volume level back from MiP
  uint8_t volume = mip.sound.readVolume();
  Serial1.print(F(" Readback Volume = "));
  Serial1.println(volume);

  delay(1000);

  // 2. Restore speaker volume back to maximum (7)
  Serial1.println(F(" Restoring volume to MIP_VOLUME_7 (7)..."));
  mip.sound.writeVolume(MIP_VOLUME_7);

  // Read back restored volume level
  volume = mip.sound.readVolume();
  Serial1.print(F(" Readback Volume = "));
  Serial1.println(volume);

  Serial1.println();
  Serial1.println(F("Volume.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs its demonstration in setup() and does not
 * require repeated work in loop().
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) { return; }
}
