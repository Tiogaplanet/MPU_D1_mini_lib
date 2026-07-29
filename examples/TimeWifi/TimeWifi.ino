/**
 * @file TimeWifi.ino
 * @brief Turns the WowWee MiP robot into a network-synchronized clock.
 *
 * @details This example connects the Wemos D1 mini to a WiFi network, retrieves
 * the current time using the Network Time Protocol (NTP), and parses it. It
 * then displays the time (hours and minutes) digit by digit using the MiP's
 * head LEDs to represent numbers and the chest LED to signal states (e.g., blue
 * for zero, green for transitions, magenta for cycle completion).
 *
 * @copyright Copyright (C) 2018 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <MPU_D1_mini.h>
#include <time.h>  // We'll read the time and parse it.

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

  Serial1.println(
    F("TimeWifi.ino: Make MiP a clock!  Display time using the eyes."));

  mip.wifi.begin(ssid, password, hostname);

    // Configure NTP servers. The first parameter is the timezone offset in
    // seconds (-4 hours).
    configTime(-4 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial1.println(F("\n Waiting for time"));
  while (!time(nullptr)) {
    Serial1.print(F("."));
    delay(1000);
  }
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
    return;  // If connecting to MiP failed in setup(), exit now.

  // Without this we can't do OTA programming.
  ArduinoOTA.handle();

  time_t now = time(nullptr);  // Read the time from NTP.
  struct tm *timeinfo;

  Serial1.print(F(" "));
  Serial1.println(ctime(&now));
  timeinfo = localtime(&now);

  // Parse the time into individual numbers.
  uint8_t hour_tens = timeinfo->tm_hour / 10;
  uint8_t hour_ones = timeinfo->tm_hour % 10;
  uint8_t minute_tens = timeinfo->tm_min / 10;
  uint8_t minute_ones = timeinfo->tm_min % 10;

  Serial1.print(F(" Hour tens: "));
  Serial1.println(hour_tens);
  Serial1.print(F(" Hour ones: "));
  Serial1.println(hour_ones);
  Serial1.print(F(" Minute tens: "));
  Serial1.println(minute_tens);
  Serial1.print(F(" Minute ones: "));
  Serial1.println(minute_ones);

  // ---------------------------------------------------------
  // Hour: Tens Digit
  // ---------------------------------------------------------
  switch (hour_tens) {
    case 0:
      mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                        MIP_HEAD_LED_OFF);
      // Signal a zero by writing blue to the chest LED.
      mip.chestLED.write(0x00, 0x00, 0xff);
      break;
    case 1:
      mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                        MIP_HEAD_LED_OFF);
      break;
    case 2:
      mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF,
                        MIP_HEAD_LED_OFF);
      break;
  }

  delay(2000);

  // Between digits, reset eyes and chest.
  mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                    MIP_HEAD_LED_OFF);
  mip.chestLED.write(0x00, 0xff, 0x00);  // Back to green.
  delay(500);

  // ---------------------------------------------------------
  // Hour: Ones Digit
  // ---------------------------------------------------------
  switch (hour_ones) {
    case 0:
      mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                        MIP_HEAD_LED_OFF);
      // Signal a zero by writing blue to the chest LED.
      mip.chestLED.write(0x00, 0x00, 0xff);
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
      mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON,
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

  delay(2000);

  // Between digits, reset eyes and chest.
  mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                    MIP_HEAD_LED_OFF);
  mip.chestLED.write(0x00, 0xff, 0x00);  // Back to green.
  delay(500);

  // ---------------------------------------------------------
  // Minute: Tens Digit
  // ---------------------------------------------------------
  switch (minute_tens) {
    case 0:
      mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                        MIP_HEAD_LED_OFF);
      // Signal a zero by writing blue to the chest LED.
      mip.chestLED.write(0x00, 0x00, 0xff);
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
  }

  delay(2000);

  // Between digits, reset eyes and chest.
  mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                    MIP_HEAD_LED_OFF);
  mip.chestLED.write(0x00, 0xff, 0x00);  // Back to green.
  delay(500);

  // ---------------------------------------------------------
  // Minute: Ones Digit
  // ---------------------------------------------------------
  switch (minute_ones) {
    case 0:
      mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                        MIP_HEAD_LED_OFF);
      // Signal a zero by writing blue to the chest LED.
      mip.chestLED.write(0x00, 0x00, 0xff);
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
      mip.headLEDs.write(MIP_HEAD_LED_ON, MIP_HEAD_LED_OFF, MIP_HEAD_LED_ON,
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

  delay(2000);

  // Reset the head, but change the chest to magenta to show that this cycle is
  // complete.
  mip.headLEDs.write(MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF, MIP_HEAD_LED_OFF,
                    MIP_HEAD_LED_OFF);
  mip.chestLED.write(0xff, 0x01, 0xfe);  // Magenta.

  delay(3000);
  mip.chestLED.write(0x00, 0xff, 0x00);  // Back to green.
}