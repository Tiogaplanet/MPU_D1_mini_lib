/**
 * @file SoftwareHardwareVersion.ino
 * @brief Example sketch that reads MiP's software/hardware version
 * information, UART baud rate, and displays the MPU library version.
 *
 * @details
 * This sketch demonstrates how to query MiP for software version,
 * hardware information, and active UART link speed using the MiP library. It:
 *   - Initializes communication with MiP using mip.begin().
 *   - Displays the library version string via version.readMPUString().
 *   - Queries the active UART connection link speed negotiated with MiP
 *     via mip.getBaudRate().
 *   - Reads the software version into a MiPSoftwareVersion struct via
 *     version.readSoftware() and prints a formatted ISO date (YYYY-MM-DD)
 *     and unique version revision.
 *   - Reads hardware information into a MiPHardwareInfo struct via
 *     version.readHardware() and prints voice chip and body hardware revision
 *     details.
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.getBaudRate()
 *   - mip.version.readMPUString()
 *   - mip.version.readSoftware()
 *   - mip.version.readHardware()
 *
 * The output is printed to Serial1 in a human-readable format so the user can
 * inspect MiP's firmware build date, revision, hardware details, and link
 * speed.
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
 * getBaudRate(), version.readMPUString(), version.readSoftware(), and
 * version.readHardware().
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
 * - Attempts to initialize the MiP connection via mip.begin().
 * - If the connection fails, prints an error to Serial1 and returns early.
 * - On success, prints the MPU library version string and active UART baud
 * rate.
 * - Reads the software version into a MiPSoftwareVersion struct
 *   and prints a formatted version string (YYYY-MM-DD.uniqueVersion).
 * - Reads hardware information into a MiPHardwareInfo struct and prints the
 *   voice chip and body hardware revision values.
 *
 * The function prints progress and completion messages to Serial1 so the
 * user can observe the retrieved version, hardware, and link speed details.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("SoftwareHardwareVersion.ino: Failed connecting to MiP."));
    return;
  }

  Serial1.println(F("SoftwareHardwareVersion.ino: Use getBaudRate(), version.readSoftware(), "
                    "and version.readHardware() functions."));

  // Display the Arduino library version string
  Serial1.print(F(" MiP Power Up library version: "));
  Serial1.println(mip.version.readMPUString());

  // Display the active UART baud rate negotiated during begin()
  Serial1.print(F(" Active UART link speed: "));
  Serial1.print(mip.getBaudRate());
  Serial1.println(F(" baud"));

  /* Read and display software version information. */
  MiPSoftwareVersion softwareVersion;
  mip.version.readSoftware(softwareVersion);
  Serial1.print(F(" Software version: "));
  Serial1.print(softwareVersion.year);
  Serial1.print('-');
  if (softwareVersion.month < 10) Serial1.print('0');  // Month zero-padding
  Serial1.print(softwareVersion.month);
  Serial1.print('-');
  if (softwareVersion.day < 10) Serial1.print('0');  // Day zero-padding
  Serial1.print(softwareVersion.day);
  Serial1.print('.');
  Serial1.println(softwareVersion.uniqueVersion);

  /* Read and display hardware information. */
  MiPHardwareInfo hardwareInfo;
  mip.version.readHardware(hardwareInfo);
  Serial1.println(F(" Hardware info"));
  Serial1.print(F("  Voice chip version: "));
  Serial1.println(hardwareInfo.voiceChip);
  Serial1.print(F("  Hardware version: "));
  Serial1.println(hardwareInfo.hardware);

  Serial1.println(F("SoftwareHardwareVersion.ino: Done."));
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
