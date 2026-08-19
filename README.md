# MiP Power Up - D1 mini library
**Turn your WowWee MiP into a cloud-connected, autonomous robot.**

![MiP Power Up mounted on MiP](https://github.com/Tiogaplanet/Experimenting-with-the-MiP/raw/master/images/IMG_5967_medium.JPG)

This Arduino library gives you control of [WowWee Labs’ MiP](https://wowwee.com/mip) — the self-balancing, hacker-friendly robot. Pair it with an ESP8266-based board (Wemos D1 mini or compatible modules) and you unlock motion, lights, sound, sensors, and wireless connectivity in a clean, modular API.

MiP ships with a 4-pin hacker port and a published [BLE protocol](https://github.com/WowWeeLabs/MiP-BLE-Protocol). This library sits on top of that protocol and turns the robot into something you can actually *program*.

## Why this library?
- **Modular by design** — Clear, focused classes for motion, LEDs, sensors, audio, odometer, battery, network, and more.
- **Reliable commands** — Verified write/read paths with automatic retries so your robot does what you asked.
- **Cloud-ready** — Built-in WiFi, OTA updates, mDNS, and remote debug over telnet.
- **Familiar Arduino feel** — Simple `begin()`, intuitive method names, and solid error reporting.

## What you can do
- Drive with continuous or distance-based commands (forward, reverse, turn, get-up, fall).
- Control the four head LEDs individually (on / off / blink) and the full RGB chest LED (color + flash timing).
- Play any of the 100+ built-in sounds or build custom sound lists.
- Read radar distance, detect hand gestures, listen for claps, and check pose / weight / shake events.
- Track distance traveled with the wheel odometer.
- Monitor battery voltage and store user data in EEPROM.
- Connect to WiFi, push OTA firmware updates, and debug wirelessly.

## Hardware
Works with any ESP8266 board that can talk UART to MiP’s hacker port. The most popular options are:
- [Wemos D1 mini](https://wiki.wemos.cc/products:d1:d1_mini) (or clones)
- The dedicated [MiP Power Up adapter boards](https://github.com/Tiogaplanet) that mount cleanly on MiP’s battery compartment

A simple breadboard connection works too if you prefer.

## Installation
1. Install the [ESP8266 Arduino core](https://github.com/esp8266/Arduino) first.
2. In the Arduino IDE: **Sketch → Include Library → Add .ZIP Library…** and select the downloaded release.
3. Or clone / copy the library folder into your `Arduino/libraries` directory.

## Quick start
```cpp
#include <MiP_Power_Up.h>

MiP mip;

void setup() {
  Serial.begin(115200);
  mip.begin("your-ssid", "your-password", "mip-robot");  // Wi-Fi + OTA ready
}

void loop() {
  mip.network().handle();          // keep OTA / debug alive
  mip.chestLED().write(0, 255, 0); // green chest
  mip.motion().distanceDrive(20, 0); // 20 cm forward
  delay(2000);
}
```

Full examples and a detailed guide live in the [wiki](https://github.com/Tiogaplanet/MPU_D1_mini_lib/wiki).

## Acknowledgements
- Original protocol work and ProMini library by [Adam Green](https://github.com/adamgreen/MiP_ProMini-Pack).
- Remote debugging foundation from [JoaoLopesF’s RemoteDebug](https://github.com/JoaoLopesF/RemoteDebug).
- The entire ESP8266 Arduino community that makes wireless robotics this accessible.

## Contributing
This project exists to make programming MiP fun and approachable. Bug reports, new features, documentation improvements, and hardware variants are all welcome. See [CONTRIBUTING.md](CONTRIBUTING.md) for the details.

---

**Ready to make MiP do something cool?**  
Grab the latest release, flash your board, and start hacking.
