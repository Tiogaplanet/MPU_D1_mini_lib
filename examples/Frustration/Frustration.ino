/**
 * @file frustration.ino
 * @brief Playful MiP "frustration" behavior: wander, evade, then tantrum.
 *
 * @details This sketch makes a MiP wander happily using radar-based obstacle
 * detection. When the robot detects too many near obstructions within a
 * cooldown interval, it expresses a short "frustration" routine: chest LED
 * turns red, head LEDs flash, it plays angry sounds, spins randomly a few
 * times, then recovers (chest LED turns green and it makes an exhaustion
 * sound). The sketch uses non-blocking timing (millis()) to track a cooldown
 * period and counts near-obstruction events to determine when to trigger the
 * frustration routine. If the MiP is tipped over, the sketch relinquishes
 * control back to factory behavior when a clap is detected.
 *
 * The example exercises these API calls:
 *   - begin()
 *   - writeVolume()
 *   - enableRadarMode()
 *   - disableRadarMode()
 *   - enableClapEvents()
 *   - availableClapEvents()
 *   - writeHeadLEDs()
 *   - writeChestLED()
 *   - beginSoundList(), addEntryToSoundList(), playSoundList()
 *   - continuousDrive(), turnLeft(), turnRight()
 *   - isUpright(), end()
 *
 * @copyright Copyright (C) 2019 Samuel Trassare (https://github.com/Tiogaplanet)
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <MPU_D1_mini.h>

/**
 * @brief Global MiP instance used to control the robot.
 *
 * @details Use this object to call MiP API functions throughout the sketch.
 */
static MiP mip;

/**
 * @brief Chest LED color components used by the sketch.
 *
 * @details We toggle between solid red and solid green for the "frustration"
 * and recovery states. Blue is kept at zero for solid red/green colors.
 */
uint8_t red = 0x00;
uint8_t green = 0x00;
const uint8_t blue = 0x00;

/**
 * @brief Cooldown interval in milliseconds.
 *
 * @details If MiP avoids near obstacles for this interval, its frustration
 * counter is reset. Default is one minute (60000 ms).
 */
const long cooldownInterval = 60000;

/**
 * @brief Number of near-obstruction events tolerated within the cooldown
 * interval before MiP expresses frustration.
 */
const uint8_t frustrationThreshold = 4;

/**
 * @brief Current frustration level (counts near-obstruction events).
 *
 * @details Incremented when a near obstruction is detected within the
 * cooldown interval; reset after a tantrum or when cooldown elapses.
 */
uint8_t frustrationLevel = 0;

/**
 * @brief Timestamp of the last near-obstruction event or last reset.
 *
 * @details Used with millis() to implement non-blocking cooldown timing.
 */
unsigned long previousMillis = 0;

/**
 * @brief Tracks whether the initial connection to the MiP succeeded.
 *
 * @details Stored so other parts of the sketch could check connection state
 * if extended.
 */
bool connectResult;

/**
 * @brief Initialize MiP and prepare sensors and audio.
 *
 * @details Connects to the MiP, mutes initial volume, seeds the random
 * generator, and enables radar mode for obstacle detection. If connection
 * fails, prints an error to Serial and returns early.
 */
void setup() {
  // Initialize the serial connection with the MiP.
  connectResult = mip.begin();
  if (!connectResult)
  {
    Serial.println(F("Frustration.ino: Failed connecting to MiP.  Is it turned on?"));
    return;
  }

  // Start quietly.
  mip.writeVolume(0);

  // Seed random with current time for varied evasions and spins.
  randomSeed(millis());

  // Enable radar-based obstacle detection.
  mip.enableRadarMode();
}

/**
 * @brief Main behavior loop.
 *
 * @details While the robot is upright, it wanders forward and uses radar
 * readings to decide whether to evade, increment frustration, or trigger the
 * frustration routine. If the robot is tipped over, the sketch disables
 * radar mode and enables clap detection; a clap will end the sketch and
 * return control to factory behavior.
 */
void loop() {
  if (!connectResult) return;  // If connecting to MiP failed in setup(), exit now.
  
  // While upright, wander and react to radar.
  while (mip.isUpright()) {
    // Drive forward continuously at moderate speed.
    mip.continuousDrive(16, 0);

    static MiPRadar lastRadar = MIP_RADAR_INVALID;
    MiPRadar        currentRadar = mip.readRadar();

    unsigned long currentMillis = millis();

    // Only react when radar reading changes and is valid.
    if (currentRadar != MIP_RADAR_INVALID && lastRadar != currentRadar)
    {
      switch (currentRadar)
      {
        case MIP_RADAR_NONE:
          // No obstruction detected; continue happily.
          break;

        case MIP_RADAR_10CM_30CM:
          // Distant obstruction: perform an evasive maneuver and continue.
          randomEvasion();
          mip.continuousDrive(16, 0);
          break;

        case MIP_RADAR_0CM_10CM:
          // Near obstruction: reset cooldown clock and increase frustration.
          previousMillis = currentMillis;
          frustrationLevel++;
          // If not yet at threshold, perform a quick evasion and continue.
          if (frustrationLevel != frustrationThreshold) {
            randomEvasion();
            mip.continuousDrive(16, 0);
          }
          break;

        default:
          break;
      }
      lastRadar = currentRadar;
    }

    // If frustration threshold reached, perform tantrum and reset counters.
    if (frustrationLevel >= frustrationThreshold) {
      frustration();
      previousMillis = currentMillis;
      frustrationLevel = 0;
    }
    // If cooldown interval elapsed without near obstructions, calm down.
    else if (currentMillis - previousMillis >= cooldownInterval) {
      previousMillis = currentMillis;
      frustrationLevel = 0;
    }
  }

  // If tipped over, relinquish control back to factory behavior on clap.
  while (!mip.isUpright()) {
    mip.disableRadarMode();
    mip.enableClapEvents();
    if (mip.availableClapEvents() > 0) {
      // Turn off head LEDs and end this sketch so factory code resumes.
      mip.writeHeadLEDs(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF);
      mip.end();
    }
  }

  // Re-enable radar mode when upright again.
  mip.enableRadarMode();
}

/**
 * @brief Express a short "frustration" routine.
 *
 * @details This routine:
 *   - Sets chest LED to red.
 *   - Plays an angry sound sequence.
 *   - Flashes head LEDs rapidly to look angry.
 *   - Performs three random spins at max speed.
 *   - Restores head LEDs to normal and plays an exhaustion sound.
 *   - Sets chest LED back to green to indicate recovery.
 */
void frustration() {

  // Set the chest LED to red to indicate anger.
  red = 0xFF;
  green = 0x00;
  mip.writeChestLED(red, green, blue);

  // Make an angry noise sequence.
  mip.beginSoundList();
  mip.addEntryToSoundList(MIP_SOUND_VOLUME_4, 0);
  mip.addEntryToSoundList(MIP_SOUND_MOOD_ANGRY, 1000);
  mip.addEntryToSoundList(MIP_SOUND_VOLUME_OFF, 0);
  mip.playSoundList(0);

  // Flash the eyes angrily: two fast blinks and two slow blinks.
  MiPHeadLEDs headLEDs;
  headLEDs.led2 = headLEDs.led3 = MIP_HEAD_LED_BLINK_FAST;
  headLEDs.led1 = headLEDs.led4 = MIP_HEAD_LED_BLINK_SLOW;
  mip.writeHeadLEDs(headLEDs);

  // Do three spins, each in a random direction for a random number of degrees.
  for (uint8_t i = 0; i < 3; i++) {
    (random(0, 2)) ? mip.turnLeft(random(0, 1276), 24) : mip.turnRight(random(0, 1276), 24);
    delay(1500); // allow spin to complete
  }

  // Restore the eyes to steady-on.
  headLEDs.led1 = headLEDs.led2 = headLEDs.led3 = headLEDs.led4 = MIP_HEAD_LED_ON;
  mip.writeHeadLEDs(headLEDs);

  // Play an "out of breath" / exhaustion sound.
  mip.beginSoundList();
  mip.addEntryToSoundList(MIP_SOUND_VOLUME_4, 0);
  mip.addEntryToSoundList(MIP_SOUND_ACTION_OUT_OF_BREATH, 0);
  mip.addEntryToSoundList(MIP_SOUND_VOLUME_OFF, 0);
  mip.playSoundList(0);

  // Set the chest LED back to green to indicate recovery.
  red = 0x00;
  green = 0xFF;
  mip.writeChestLED(red, green, blue);
}

/**
 * @brief Perform a short random evasion maneuver.
 *
 * @details Randomly chooses to turn left or right by 90 degrees at a moderate
 * speed, then pauses briefly to allow the maneuver to complete.
 */
void randomEvasion() {
  // Randomly turn left or right to avoid obstruction.
  (random(0, 2) == 0) ? mip.turnLeft(90, 12) : mip.turnRight(90, 12);
  delay(500);
}
