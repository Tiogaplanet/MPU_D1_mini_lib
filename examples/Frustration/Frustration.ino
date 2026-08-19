/**
 * @file Frustration.ino
 * @brief Playful MiP "frustration" behavior: wander, evade, then tantrum.
 *
 * @details This sketch makes MiP wander happily using radar-based obstacle
 * detection. When MiP detects too many near obstructions within a
 * cooldown interval, a short "frustration" routine is expressed: chest LED
 * turns red, head LEDs flash, angry sounds play, MiP spins randomly a few
 * times, then recovers (chest LED turns green and an exhaustion sound plays).
 * The sketch uses non-blocking timing (millis()) to track a cooldown period
 * and counts near-obstruction events to determine when to trigger the
 * frustration routine. If MiP is tipped over, the sketch relinquishes control
 * back to factory behavior when a clap is detected.
 *
 * The example exercises these API calls:
 *   - begin()
 *   - sound.writeVolume()
 *   - radar.enable()
 *   - radar.disable()
 *   - clap.enableEvents()
 *   - clap.availableEvents()
 *   - headLEDs.write()
 *   - chestLED.write()
 *   - sound.beginList(), sound.addEntryToList(), sound.playList()
 *   - motion.continuousDrive(), motion.turnLeft(), motion.turnRight()
 *   - position.isUpright(), end()
 *
 * @author Samuel Trassare (Original Author)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MiP_Power_Up.h>

/**
 * @brief Forward declarations for helper functions.
 */
void frustration();
void randomEvasion();

/**
 * @brief Global MiP instance used to control MiP.
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
 * @details If MiP avoids near obstacles for this interval, the frustration
 * counter is reset. Default is one minute (60000 ms).
 */
const uint32_t cooldownInterval = 60000;

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
uint32_t previousMillis = 0;

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Initialize MiP and prepare sensors and audio.
 *
 * @details Connects to MiP, mutes initial volume, seeds the random
 * generator, and enables radar mode for obstacle detection. If connection
 * fails, prints an error to mip.console and returns early.
 */
void setup() {
  // Initialize the connection with MiP.
  connectResult = mip.begin();
  if (!connectResult) {
    mip.console.println(F("Frustration.ino: Failed connecting to MiP. Is MiP turned on?"));
    return;
  }

  // Start quietly.
  mip.sound.writeVolume(0);

  // Seed random with current time for varied evasions and spins.
  randomSeed(millis());

  // Enable radar-based obstacle detection.
  mip.radar.enable();
}

/**
 * @brief Main behavior loop.
 *
 * @details While MiP is upright, continuous forward driving occurs and radar
 * readings decide whether to evade, increment frustration, or trigger the
 * frustration routine. If MiP is tipped over, radar mode is disabled and clap
 * detection is enabled; a clap will end the sketch and return control to
 * factory behavior.
 */
void loop() {
  if (!connectResult)
    return;  // If connecting to MiP failed in setup(), exit now.

  // While upright, wander and react to radar.
  while (mip.position.isUpright()) {
    // Drive forward continuously at moderate speed.
    mip.motion.continuousDrive(16, 0);

    static MiPRadar lastRadar = MIP_RADAR_INVALID;
    MiPRadar currentRadar = mip.radar.read();

    uint32_t currentMillis = millis();

    // Only react when radar reading changes and is valid.
    if (currentRadar != MiPRadar::MIP_RADAR_INVALID && lastRadar != currentRadar) {
      switch (currentRadar) {
        case MiPRadar::MIP_RADAR_NONE:
          // No obstruction detected; continue happily.
          break;

        case MiPRadar::MIP_RADAR_10CM_30CM:
          // Distant obstruction: perform an evasive maneuver and continue.
          randomEvasion();
          mip.motion.continuousDrive(16, 0);
          break;

        case MiPRadar::MIP_RADAR_0CM_10CM:
          // Near obstruction: reset cooldown clock and increase frustration.
          previousMillis = currentMillis;
          frustrationLevel++;
          // If not yet at threshold, perform a quick evasion and continue.
          if (frustrationLevel != frustrationThreshold) {
            randomEvasion();
            mip.motion.continuousDrive(16, 0);
          }
          break;

        default: break;
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

    // Yield control briefly to prevent watchdog reset triggers
    delay(10);
  }

  // If tipped over, prepare to relinquish control back to factory behavior
  mip.radar.disable();
  mip.clap.enableEvents();

  // Wait until MiP is upright again or a clap is detected
  while (!mip.position.isUpright()) {
    if (mip.clap.availableEvents() > 0) {
      // Turn off head LEDs and end this sketch so factory code resumes.
      mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF);
      mip.end();
      connectResult = false;  // Mark connection inactive
      return;
    }
    delay(50);  // Yield CPU time while tipped over
  }

  // Re-enable radar mode when upright again.
  mip.radar.enable();
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
  mip.chestLED.write(red, green, blue);

  // Make an angry noise sequence.
  mip.sound.beginList();
  mip.sound.addEntryToList(MIP_SOUND_VOLUME_4, 0);
  mip.sound.addEntryToList(MIP_SOUND_MOOD_ANGRY, 1000);
  mip.sound.addEntryToList(MIP_SOUND_VOLUME_OFF, 0);
  mip.sound.playList(0);

  // Flash the eyes angrily: two fast blinks and two slow blinks.
  MiPHeadLEDs headLEDs;
  headLEDs.led2 = headLEDs.led3 = MIP_HEAD_LED_BLINK_FAST;
  headLEDs.led1 = headLEDs.led4 = MIP_HEAD_LED_BLINK_SLOW;
  mip.headLEDs.write(headLEDs);

  // Do three spins, each in a random direction for a random number of degrees.
  for (uint8_t i = 0; i < 3; i++) {
    (random(0, 2) == 0) ? mip.motion.turnLeft(random(0, 1276), 24)
                        : mip.motion.turnRight(random(0, 1276), 24);
    delay(1500);  // Allow spin to complete
  }

  // Restore the eyes to steady-on.
  headLEDs.led1 = headLEDs.led2 = headLEDs.led3 = headLEDs.led4 = MIP_HEAD_LED_ON;
  mip.headLEDs.write(headLEDs);

  // Play an "out of breath" / exhaustion sound.
  mip.sound.beginList();
  mip.sound.addEntryToList(MIP_SOUND_VOLUME_4, 0);
  mip.sound.addEntryToList(MIP_SOUND_ACTION_OUT_OF_BREATH, 0);
  mip.sound.addEntryToList(MIP_SOUND_VOLUME_OFF, 0);
  mip.sound.playList(0);

  // Set the chest LED back to green to indicate recovery.
  red = 0x00;
  green = 0xFF;
  mip.chestLED.write(red, green, blue);
}

/**
 * @brief Perform a short random evasion maneuver.
 *
 * @details Randomly chooses to turn left or right by 90 degrees at a moderate
 * speed, then pauses briefly to allow the maneuver to complete.
 */
void randomEvasion() {
  // Randomly turn left or right to avoid obstruction.
  (random(0, 2) == 0) ? mip.motion.turnLeft(90, 12) : mip.motion.turnRight(90, 12);
  delay(500);
}
