# Security Policy

## Supported Versions

| Version | Supported          |
| ------- | ------------------ |
| 2.0.2   | :white_check_mark: |
| < 2.0.2 | :x:                |
| development branches | :x:       |

Only version **2.0.2** receives security updates.

## Reporting a Vulnerability

We take the security of the MiP Power Up library seriously.

**Please do not report security vulnerabilities through public GitHub issues.**

Instead, use one of the following private channels:

1. **Preferred** – Open a [private GitHub Security Advisory](https://github.com/Tiogaplanet/MPU_D1_mini_lib/security/advisories/new) on this repository.
2. Email the maintainer (see the GitHub profile or `CODE_OF_CONDUCT.md` for contact details).

### What to include in your report

- A clear description of the vulnerability
- Steps to reproduce (code snippets, configuration, hardware setup if relevant)
- Affected library version(s) and ESP8266 core version
- Potential impact (e.g., unexpected robot motion, exposure of Wi-Fi credentials, remote code execution via OTA/debug interfaces, etc.)
- Any suggested mitigations or patches (optional but appreciated)

### What to expect

- You should receive an initial acknowledgement within a few days (this is a volunteer-maintained project).
- We will investigate and keep you informed of progress.
- Once a fix is ready we will coordinate a public disclosure and credit the reporter (unless you prefer to remain anonymous).

## Scope

The following are considered in scope:

- Vulnerabilities in the library’s network, OTA, mDNS, or remote-debug functionality
- Memory-safety or buffer issues that could lead to crashes or unexpected behaviour on the ESP8266
- Logic flaws that could cause the robot to move or act in an unsafe or unintended way when the library is used as documented
- Supply-chain concerns in the published library packages

The following are generally **out of scope**:

- Issues that only exist on unreleased development branches and have never been part of a tagged release
- Vulnerabilities in third-party dependencies (ESP8266 Arduino core, RemoteDebug, etc.) – please report those upstream
- Physical safety of the MiP robot itself (balancing, tipping, etc.) when the library is used correctly
- Social-engineering or physical-access attacks

## Safe Harbour

We will not take legal action against researchers who:

- Make a good-faith effort to follow this policy
- Avoid privacy violations, destruction of data, or interruption of service
- Do not exploit the vulnerability beyond what is necessary to demonstrate it

Thank you for helping keep the MiP community and its robots safe!
