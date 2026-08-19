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
 * The output is printed to mip.console in a human-readable format so the user can
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
#include <MiP_Power_Up.h>

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
 * - If the connection fails, prints an error to mip.console and returns early.
 * - On success, prints the MPU library version string and active UART baud
 * rate.
 * - Reads the software version into a MiPSoftwareVersion struct
 *   and prints a formatted version string (YYYY-MM-DD.uniqueVersion).
 * - Reads hardware information into a MiPHardwareInfo struct and prints the
 *   voice chip and body hardware revision values.
 *
 * The function prints progress and completion messages to mip.console so the
 * user can observe the retrieved version, hardware, and link speed details.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    mip.console.println(F("SoftwareHardwareVersion.ino: Failed connecting to MiP."));
    return;
  }

  mip.console.println(F("SoftwareHardwareVersion.ino: Use getBaudRate(), version.readSoftware(), "
                    "and version.readHardware() functions."));

  // Display the Arduino library version string
  mip.console.print(F(" MiP Power Up library version: "));
  mip.console.println(mip.version.readMPUString());

  // Display the active UART baud rate negotiated during begin()
  mip.console.print(F(" Active UART link speed: "));
  mip.console.print(mip.getBaudRate());
  mip.console.println(F(" baud"));

  /* Read and display software version information. */
  MiPSoftwareVersion softwareVersion;
  mip.version.readSoftware(softwareVersion);
  mip.console.print(F(" Software version: "));
  mip.console.print(softwareVersion.year);
  mip.console.print('-');
  if (softwareVersion.month < 10) mip.console.print('0');  // Month zero-padding
  mip.console.print(softwareVersion.month);
  mip.console.print('-');
  if (softwareVersion.day < 10) mip.console.print('0');  // Day zero-padding
  mip.console.print(softwareVersion.day);
  mip.console.print('.');
  mip.console.println(softwareVersion.uniqueVersion);

  /* Read and display hardware information. */
  MiPHardwareInfo hardwareInfo;
  mip.version.readHardware(hardwareInfo);
  mip.console.println(F(" Hardware info"));
  mip.console.print(F("  Voice chip version: "));
  mip.console.println(hardwareInfo.voiceChip);
  mip.console.print(F("  Hardware version: "));
  mip.console.println(hardwareInfo.hardware);

  mip.console.println(F("SoftwareHardwareVersion.ino: Done."));
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
