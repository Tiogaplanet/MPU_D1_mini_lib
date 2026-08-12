/**
 * @file EnableMiPDetectionMode.ino
 * @brief Example sketch demonstrating MiP's IR-based detection mode.
 *
 * @details This sketch shows how to enable and disable MiP's detection mode
 * which allows one MiP to discover another using infrared. It
 * demonstrates the infrared.enableMiPDetectionMode(),
 * infrared.disableMiPDetectionMode(), infrared.isMiPDetectionModeEnabled(),
 * infrared.availableDetectedMiPEvents(), and infrared.readDetectedMiP() APIs.
 * The sketch:
 *   - Initializes communication with MiP.
 *   - Disables detection mode and verifies it is disabled.
 *   - Enables detection mode with a specified ID and IR transmit power and
 *     verifies it is enabled.
 *   - In loop(), polls for detected MiP events and prints detected MiP IDs.
 *
 * Example API methods demonstrated:
 *   - mip.begin()
 *   - mip.infrared.disableMiPDetectionMode()
 *   - mip.infrared.isMiPDetectionModeEnabled()
 *   - mip.infrared.enableMiPDetectionMode(MIP_ID_NO, MIP_IR_TX_POWER)
 *   - mip.infrared.availableDetectedMiPEvents()
 *   - mip.infrared.readDetectedMiP()
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
 * infrared.enableMiPDetectionMode(), infrared.disableMiPDetectionMode(),
 * infrared.isMiPDetectionModeEnabled(), infrared.availableDetectedMiPEvents(),
 * and infrared.readDetectedMiP().
 */
MiP mip;

/**
 * @brief MiP ID number to advertise when detection mode is enabled.
 *
 * @details When detection mode is enabled, this ID is transmitted via IR so
 * another MiP can identify this unit. Change this value when running two
 * MiPs facing each other so each uses a unique ID.
 */
#define MIP_ID_NO 0x10

/**
 * @brief IR transmit power used when enabling detection mode.
 *
 * @details Value controls the IR transmitter power. Valid ranges depend on
 * the MiP firmware/hardware; 0x78 is used here as an example to provide a
 * moderate transmit strength.
 */
#define MIP_IR_TX_POWER 0x78

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Arduino setup function.
 *
 * @details Initializes communication with MiP by calling mip.begin().
 * If the connection fails, an error message is printed to Serial1 and setup
 * returns early. The function then demonstrates disabling detection mode and
 * verifying the disabled state, followed by enabling detection mode with the
 * configured ID and IR power and verifying the enabled state.
 *
 * API usage in this function:
 *   - mip.begin()
 *   - mip.infrared.disableMiPDetectionMode()
 *   - mip.infrared.isMiPDetectionModeEnabled()
 *   - mip.infrared.enableMiPDetectionMode(MIP_ID_NO, MIP_IR_TX_POWER)
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("EnableMiPDetectionMode.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("EnableMiPDetectionMode.ino: Enable MiP to be discovered "
                    "by another MiP using IR."));

  /* Ensure detection mode is off and verify. */
  mip.infrared.disableMiPDetectionMode();

  if (!mip.infrared.isMiPDetectionModeEnabled()) {
    Serial1.println(F(" I am not discoverable."));
  }

  /* Enable detection mode with configured ID and IR transmit power. */
  mip.infrared.enableMiPDetectionMode(MIP_ID_NO, MIP_IR_TX_POWER);

  if (mip.infrared.isMiPDetectionModeEnabled()) {
    Serial1.println(F(" Now I can be discovered."));
  }
}

/**
 * @brief Arduino loop function.
 *
 * @details Polls for detected MiP events using
 * infrared.availableDetectedMiPEvents(). While events are available,
 * infrared.readDetectedMiP() returns each detected MiP ID, which is printed to
 * Serial1 in hexadecimal format.
 *
 * API usage in this function:
 *   - mip.infrared.availableDetectedMiPEvents()
 *   - mip.infrared.readDetectedMiP()
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) { return; }

  // Poll for available detected MiP events
  while (mip.infrared.availableDetectedMiPEvents() > 0) {
    uint8_t detectedId = mip.infrared.readDetectedMiP();
    Serial1.print(F(" I detected MiP with ID number 0x"));
    if (detectedId < 0x10) {
      Serial1.print(F("0"));  // Leading zero padding
    }
    Serial1.println(detectedId, HEX);
  }

  // Yield control briefly to prevent watchdog reset triggers
  delay(10);
}
