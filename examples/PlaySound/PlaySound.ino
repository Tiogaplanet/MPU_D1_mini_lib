/**
 * @file PlaySound.ino
 * @brief Example sketch demonstrating MiP sound playback and sound lists.
 *
 * @details This sketch shows how to use the MiP library to play single sounds
 * and to build and play sound lists (sequences). It demonstrates playing a
 * single sound with sound.play(), creating a sound list with sound.beginList()
 * and sound.addEntryToList(), and playing the list with sound.playList(). The
 * example plays a single "drinking" sound, then constructs a two-entry sound
 * list (eating, then burping) with different volumes and repeats the list.
 *
 * The example exercises these API calls:
 *   - mip.begin()
 *   - mip.sound.play()
 *   - mip.sound.beginList()
 *   - mip.sound.addEntryToList()
 *   - mip.sound.playList()
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
 * sound.play(), sound.beginList(), sound.addEntryToList(), and
 * sound.playList().
 */
MiP mip;

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Arduino setup function.
 *
 * @details Initializes communication with MiP by calling mip.begin().
 * If the connection fails, an error message is printed to Serial1 and setup
 * returns early. On success, the sketch demonstrates:
 *   - Playing a single sound with sound.play().
 *   - Building a sound list using sound.beginList() and sound.addEntryToList().
 *   - Playing the sound list with sound.playList() and repeating it after a
 *     delay.
 *
 * The example uses delays to allow sounds and sound lists to complete before
 * proceeding to the next action.
 */
void setup() {
  connectResult = mip.begin();
  if (!connectResult) {
    Serial1.println(F("PlaySound.ino: Failed connecting to MiP!"));
    return;
  }

  Serial1.println(F("PlaySound.ino: Play a few sounds."));

  // Play a single sound (drinking) at volume level 4.
  mip.sound.play(MIP_SOUND_ACTION_DRINKING, MIP_VOLUME_4);
  delay(3000);  // Allow the single sound to play.

  // Build a sound list: two entries with a 1 second gap between them,
  // then play the list once and repeat it later.
  mip.sound.beginList();
  // First entry: eating, 1000 ms delay before next entry, volume 4.
  mip.sound.addEntryToList(MIP_SOUND_ACTION_EATING, 1000, MIP_VOLUME_4);
  // Second entry: burping, no delay after, louder volume 7.
  mip.sound.addEntryToList(MIP_SOUND_ACTION_BURPING, 0, MIP_VOLUME_7);
  // Play the constructed list once (repeat count = 1).
  mip.sound.playList(1);

  // Wait long enough for the list to finish, then play it again.
  delay(10000);
  mip.sound.playList();

  Serial1.println();
  Serial1.println(F("PlaySound.ino: Done."));
}

/**
 * @brief Arduino loop function.
 *
 * @details This example performs all actions in setup() and does not require
 * repeated work in loop().
 */
void loop() {
  // Exit immediately if connecting to MiP failed during setup()
  if (!connectResult) { return; }
}
