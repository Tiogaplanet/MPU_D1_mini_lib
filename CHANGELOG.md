# Changelog

All notable changes to the MiP Power Up - D1 mini library are documented in this file.

## [2.0.1] - 2026-08-09
### Added
- `sendDongleCode()` now supports 3- and 4-byte IR codes (previously only reliable for 2-byte).
- New example: `SendDongleCode`.

### Fixed
- IR dongle transmit bit-length field now correctly reflects 16 / 24 / 32 bits instead of a fixed 0x10.
- Various minor robustness cleanups.

## [2.0.0] - 2026-08-01
### Breaking
- Replaced monolithic `MiP` API with subsystem objects
  (`mip.chestLED`, `mip.motion`, `mip.radar`, `mip.sound`, `mip.wifi`, ...).
- Wi‑Fi/OTA setup moved to `mip.wifi.begin()`.
- Library renamed to “MiP Power Up - D1 mini”.

### Added
- Modular source layout under `src/`.
- Explicit serial transport and OOB event dispatch (including radar).
- Richer keywords.txt / documentation pass.

### Fixed
- Various robustness and naming cleanups.

## [1.0.1] - 2026-06-14
### Added
- Added auto speed negotiation to switch between 9600 and 115200 baud depending on the MiP hardware revision.
- Added support for reporting various debug levels: none, error, warning, info.

### Fixed
- Failure to connect to early MiP boards employing 9600 baud serial speed.

## [0.0.1] - 2018-08-15
- Initial release of the MiP ESP8266 Library.
- Basic movement commands and chest LED control implementation.
