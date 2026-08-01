/**
 * @file TimeWifi.ino
 * @brief Turns MiP into a network-synchronized clock.
 *
 * @details This example connects the Wemos D1 mini to a WiFi network, retrieves
 * the current time using the Network Time Protocol (NTP), and parses it. It
 * then displays the time, hours and minutes, digit by digit, using MiP's head
 * LEDs to represent numbers and the chest LED to signal states (e.g., blue for
 * zero, green for transitions, magenta for cycle completion).
 *
 * MiP displays hours and minutes, not seconds.  Therefore, NTP is polled only
 * once a minute.
 *
 * This sketch would be an excellent starting point to make MiP into an alarm
 * clock using built-in sounds, a web interface to set the alarm time, and a 
 * change in position, such as a knock over or tip forward to turn off the 
 * alarm.
 *
 * @author Samuel Trassare (Original Author)
 * * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MPU_D1_mini.h>
#include <time.h> // We'll read the time and parse it.

/**
 * @brief Wi‑Fi SSID to join.
 *
 * @note Replace the placeholder with your network SSID before uploading.
 */
const char *ssid = "..............";

/**
 * @brief Wi‑Fi password for the SSID.
 *
 * @note Replace the placeholder with your network password before uploading.
 */
const char *password = "..............";

/**
 * @brief Hostname used for MiP connection.
 *
 * @details This name appears in the network. Choose a unique hostname for each
 * device.
 */
const char *hostname = "MiP-Timekeeper";

/**
 * @brief Global MiP instance used to initialize and control the robot.
 *
 * @details The mip object is used to establish the network connection and
 * to integrate MiP-specific functionality with the telnet debug service.
 */
MiP mip;

/**
 * @brief Tracks whether the initial connection to the MiP succeeded.
 *
 * @details Stored so other parts of the sketch could check connection state
 * if extended.
 */
bool connectResult;

/**
 *  @brief Cached digits update only once per minute.
 *
 * @details MiP's eyes and chest persistently display the time, but the NTP
 * check happens only once a minute.
 */
uint8_t hour_tens = 0;
uint8_t hour_ones = 0;
uint8_t minute_tens = 0;
uint8_t minute_ones = 0;

/**
 *  @brief Tracks when we last called time().
 *
 * @details MiP doesn't display seconds, so only check NTP once a minute and
 * reduce network load.
 */
int lastFetchedMinute = -1; // tracks when we last called time()

/**
 * @brief Arduino setup routine.
 *
 * @details Initializes the MiP object, establishes a WiFi connection using the
 * provided credentials, configures the NTP servers to retrieve the current time
 * (adjusted for timezone), and blocks until a valid time signal is successfully
 * received.
 */
void setup() {
  connectResult = mip.begin();

  if (!connectResult) {
    Serial1.println(F("TimeWifi.ino: Failed connecting to MiP."));
    return;
  }

  Serial1.println(F("TimeWifi.ino: Make MiP a clock!  Display time using the "
                    "eyes and chest."));

  mip.wifi.begin(ssid, password, hostname);

  // Configure NTP servers. The first parameter is the timezone offset in
  // seconds (-4 hours).
  configTime(-4 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial1.println(F(" Waiting for a valid response from NTP."));
  while (!time(nullptr)) {
    Serial1.print(F("."));
    delay(1000);
  }

  // Force an initial fetch
  updateTimeDigits();
}

/**
 * @brief Main Arduino loop routine.
 *
 * @details Handles Over-The-Air (OTA) updates, retrieves the current local
 * time, parses it into individual digits (tens and ones for both hours and
 * minutes), and sequentially updates the MiP's head and chest LEDs to visually
 * broadcast the time.
 */
void loop() {
  if (!connectResult)
    return; // If connecting to MiP failed in setup(), exit now.

  // Without this we can't do OTA programming.
  ArduinoOTA.handle();

  // Refresh the cached digits only once per minute
  updateTimeDigits();

  // ---------------------------------------------------------
  // Continuous LED display cycle (uses the cached digits)
  // ---------------------------------------------------------

  // Hour tens
  showDigit(hour_tens);
  delay(2000);
  resetEyesAndChest();

  // Hour ones
  showDigit(hour_ones);
  delay(2000);
  resetEyesAndChest();

  // Minute tens
  showDigit(minute_tens);
  delay(2000);
  resetEyesAndChest();

  // Minute ones
  showDigit(minute_ones);
  delay(2000);

  // End-of-cycle indicator (magenta)
  mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                     MIP_HEAD_LED_OFF);
  mip.chestLED.write(0xff, 0x01, 0xfe); // Magenta
  delay(3000);
  mip.chestLED.write(0x00, 0xff, 0x00); // Back to green
}

// ---------------------------------------------------------------------------
// Time fetching – runs at most once per minute
// ---------------------------------------------------------------------------
void updateTimeDigits() {
  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);

  // Ignore invalid epoch values before the first good sync
  if (timeinfo->tm_year + 1900 < 2020) {
    return;
  }

  // Only update the cached digits when the minute changes
  if (timeinfo->tm_min == lastFetchedMinute) {
    return;
  }

  lastFetchedMinute = timeinfo->tm_min;

  hour_tens = timeinfo->tm_hour / 10;
  hour_ones = timeinfo->tm_hour % 10;
  minute_tens = timeinfo->tm_min / 10;
  minute_ones = timeinfo->tm_min % 10;

  // Clean output – no extra newlines or stair-stepping
  char timeBuf[32];
  strftime(timeBuf, sizeof(timeBuf), "%a %b %d %H:%M:%S %Y", timeinfo);

  Serial1.print(F(" Time updated: "));
  Serial1.println(timeBuf);
  Serial1.printf(" Displaying %d%d:%d%d\r\n", hour_tens, hour_ones, minute_tens,
                 minute_ones);
}

// ---------------------------------------------------------------------------
// LED helpers
// ---------------------------------------------------------------------------
void showDigit(uint8_t digit) {
  switch (digit) {
  case 0:
    mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                       MIP_HEAD_LED_OFF);
    mip.chestLED.write(0x00, 0x00, 0xff); // blue = zero
    break;
  case 1:
    mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                       MIP_HEAD_LED_OFF);
    break;
  case 2:
    mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF,
                       MIP_HEAD_LED_OFF);
    break;
  case 3:
    mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF,
                       MIP_HEAD_LED_OFF);
    break;
  case 4:
    mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON,
                       MIP_HEAD_LED_OFF);
    break;
  case 5:
    mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON,
                       MIP_HEAD_LED_OFF);
    break;
  case 6:
    mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON, MIP_HEAD_LED_ON,
                       MIP_HEAD_LED_OFF);
    break;
  case 7:
    mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_ON, MIP_HEAD_LED_ON,
                       MIP_HEAD_LED_OFF);
    break;
  case 8:
    mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                       MIP_HEAD_LED_ON);
    break;
  case 9:
    mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                       MIP_HEAD_LED_ON);
    break;
  }
}

void resetEyesAndChest() {
  mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                     MIP_HEAD_LED_OFF);
  mip.chestLED.write(0x00, 0xff, 0x00); // green
  delay(500);
}