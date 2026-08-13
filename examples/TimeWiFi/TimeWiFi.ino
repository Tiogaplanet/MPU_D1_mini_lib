/**
 * @file TimeWiFi.ino
 * @brief Turns MiP into a network-synchronized clock.
 *
 * @details This example connects the Wemos D1 mini to a WiFi network, retrieves
 * the current time using the Network Time Protocol (NTP), and parses it. It
 * then displays the time, hours and minutes, digit by digit, using MiP's head
 * LEDs to represent numbers and the chest LED to signal states (e.g., blue for
 * zero, green for transitions, magenta for cycle completion).
 *
 * MiP displays hours and minutes, not seconds. Therefore, NTP is polled only
 * once a minute.
 *
 * This sketch serves as an excellent starting point to make MiP into an alarm
 * clock using built-in sounds, a web interface to set the alarm time, and a
 * change in position (such as a tip forward) to turn off the alarm.
 *
 * Example API methods demonstrated:
 *   - mip.begin()
 *   - mip.wifi.begin(ssid, password, hostname)
 *   - mip.headLEDs.write()
 *   - mip.chestLED.write()
 *   - ArduinoOTA.handle()
 *
 * @author Samuel Trassare (Original Author)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#include <MiP_Power_Up_-_D1_mini.h>
#include <time.h>  // Read and parse time structures

/**
 * @brief WiFi SSID to join.
 *
 * @note Replace the placeholder with your network SSID before uploading.
 */
const char *ssid = "..............";

/**
 * @brief WiFi password for the SSID.
 *
 * @note Replace the placeholder with your network password before uploading.
 */
const char *password = "..............";

/**
 * @brief Hostname used for MiP connection.
 *
 * @details This name appears on the network via mDNS. Choose a unique hostname
 * for each device on your network.
 */
const char *hostname = "MiP-Timekeeper";

/**
 * @brief Global MiP instance used to communicate with MiP.
 *
 * @details The mip object is used throughout the sketch to call MiP API
 * functions.
 */
MiP mip;

/**
 * @brief Tracks whether the initial connection to MiP succeeded.
 */
bool connectResult;

/**
 * @brief Cached time digits updated once per minute.
 *
 * @details MiP's eyes and chest persistently display the time, but the NTP
 * check happens only once a minute to reduce network load.
 */
uint8_t hour_tens = 0;
uint8_t hour_ones = 0;
uint8_t minute_tens = 0;
uint8_t minute_ones = 0;

/**
 * @brief Tracks when time() was last fetched.
 *
 * @details MiP doesn't display seconds, so check NTP once a minute to
 * reduce network traffic.
 */
int lastFetchedMinute = -1;

// ---------------------------------------------------------------------------
// Helper Function Prototypes
// ---------------------------------------------------------------------------
void updateTimeDigits();
void showDigit(uint8_t digit);
void resetEyesAndChest();

/**
 * @brief Fetch current local time and update cached digit variables.
 *
 * @details Runs at most once per minute. Parses epoch time into individual
 * hour/minute tens and ones digits, and logs formatted time to Serial1.
 */
void updateTimeDigits() {
  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);

  // Ignore invalid epoch values before the first good sync
  if (timeinfo == nullptr || timeinfo->tm_year + 1900 < 2020) { return; }

  // Only update the cached digits when the minute changes
  if (timeinfo->tm_min == lastFetchedMinute) { return; }

  lastFetchedMinute = timeinfo->tm_min;

  hour_tens = timeinfo->tm_hour / 10;
  hour_ones = timeinfo->tm_hour % 10;
  minute_tens = timeinfo->tm_min / 10;
  minute_ones = timeinfo->tm_min % 10;

  char timeBuf[32];
  strftime(timeBuf, sizeof(timeBuf), "%a %b %d %H:%M:%S %Y", timeinfo);

  Serial1.print(F(" Time updated: "));
  Serial1.println(timeBuf);
  Serial1.printf(" Displaying %d%d:%d%d\r\n", hour_tens, hour_ones, minute_tens, minute_ones);
}

/**
 * @brief Display a single numerical digit (0-9) using MiP's head and chest
 * LEDs.
 *
 * @param digit Value from 0 to 9 to visually display on MiP.
 */
void showDigit(uint8_t digit) {
  switch (digit) {
    case 0:
      mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF);
      mip.chestLED.write(0x00, 0x00, 0xFF);  // Blue indicates zero
      break;
    case 1:
      mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF);
      break;
    case 2:
      mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF);
      break;
    case 3:
      mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF);
      break;
    case 4:
      mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF);
      break;
    case 5:
      mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF);
      break;
    case 6:
      mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON, MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF);
      break;
    case 7:
      mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_ON, MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF);
      break;
    case 8:
      mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON);
      break;
    case 9:
      mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON);
      break;
    default: break;
  }
}

/**
 * @brief Reset head LEDs to off and chest LED to green between digit
 * transitions.
 */
void resetEyesAndChest() {
  mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF);
  mip.chestLED.write(0x00, 0xFF, 0x00);  // Green inter-digit transition
  delay(500);
}

/**
 * @brief Arduino setup routine.
 *
 * @details Initializes the MiP object, establishes a WiFi connection,
 * configures NTP time synchronization, waits for a valid time signal, and
 * forces an initial time fetch.
 */
void setup() {
  connectResult = mip.begin();

  if (!connectResult) {
    Serial1.println(F("TimeWiFi.ino: Failed connecting to MiP."));
    return;
  }

  Serial1.println(F("TimeWiFi.ino: Make MiP a clock! Display time using "
                    "eyes and chest."));

  uint8_t wifiStatus = mip.wifi.begin(ssid, password, hostname);
  if (wifiStatus != WL_CONNECTED) {
    Serial1.println(F("TimeWiFi.ino: Failed connecting to WiFi."));
    connectResult = false;
    return;
  }

  // Configure NTP servers with timezone offset (-4 hours = -14400 seconds)
  configTime(-4 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial1.println(F(" Waiting for a valid response from NTP."));
  uint8_t ntpAttempts = 0;
  while (time(nullptr) < 1600000000UL && ntpAttempts < 30) {
    Serial1.print(F("."));
    delay(1000);
    ntpAttempts++;
  }
  Serial1.println();

  if (time(nullptr) < 1600000000UL) {
    Serial1.println(F("TimeWiFi.ino: NTP sync timed out."));
    connectResult = false;
    return;
  }

  // Force initial time digit update
  updateTimeDigits();
}

/**
 * @brief Main Arduino loop routine.
 *
 * @details Handles Over-The-Air (OTA) updates, checks for minute changes,
 * and sequentially updates MiP's head and chest LEDs to display the time.
 */
void loop() {
  if (!connectResult) {
    return;  // Exit immediately if connecting to MiP or WiFi failed during
             // setup()
  }

  // Required for OTA programming to function while running
  ArduinoOTA.handle();

  // Refresh cached digits if a minute has passed
  updateTimeDigits();

  // ---------------------------------------------------------
  // Continuous LED display cycle (uses cached digits)
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
  mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF);
  mip.chestLED.write(0xFF, 0x01, 0xFE);  // Magenta
  delay(3000);
  mip.chestLED.write(0x00, 0xFF, 0x00);  // Restore green
}
