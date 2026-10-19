# ESP32 Relapse Exploit

A self-contained [ESP32](https://www.espressif.com/en/products/socs/esp32) access point that hosts the [PS5 Relapse](https://github.com/ntfargo/Relapse-Exploit) WebKit + kernel exploit (firmwares **7.00–13.60**). It exposes a WiFi access point, a DNS captive portal, an HTTP server (port 80) and an HTTPS server (port 443), so a PS5 can run the exploit fully offline.

## Features

- Custom landing page with two actions:
  - **Run exploit** — runs the Relapse exploit chain (WebKit → kernel → `elfldr` listening on port 9021).
  - **Load payloads** — sends a tiny [usb-payload-browser](https://github.com/leafunes/ps5-usb-payload-browser) payload to the PS5 and opens its web UI to browse and load `.elf` payloads from a USB drive.
- WiFi access point (`PS5-Relapse`) + DNS captive portal (every DNS query resolves to the ESP32).
- HTTPS on port 443 with a self-signed certificate (persisted in LittleFS) to intercept the PS5 User's Guide and redirect it to the landing page.
- Serves all exploit assets (firmware offsets, `elfldr`, kernel exploit binary) from a LittleFS filesystem.

## Hardware

Built and tested **only** on a **ESP32-WROOM-32** (chip **ESP32-D0WD-V3**, **4 MB** flash) — a generic "ESP32 DevKit V1" board.

> **Note:** this is the only board the project has been tested on. Testing on other ESP32 boards (ESP32-S2, ESP32-S3, ESP32-C3, 8/16 MB flash, etc.) is welcome — please open an issue or a pull request with your results.

## ⚠️ AI Assistance Disclaimer

This project was developed with substantial assistance from AI tools (large language models). The code has been reviewed and tested on the hardware listed above, but AI-generated code can still contain subtle bugs, incorrect assumptions, or security issues. Use it at your own risk and review the code before relying on it.

## Repository layout

```
ps5-esp32-relapse/
├── platformio.ini            # PlatformIO project (esp32dev, Arduino, LittleFS)
├── src/main.cpp              # AP + DNS + HTTP + HTTPS + payload sender
├── data/index.html           # landing page (menu)
├── data/exploit.html         # Relapse exploit entry point (with "Menu" link)
├── data/src|offsets|payloads # Relapse exploit assets (vendored)
├── data/pl/                  # usb-payload-browser.elf (built from the linked repo)
├── scripts/update_assets.py  # re-fetches the Relapse assets from GitHub
└── THIRD_PARTY_NOTICES.md    # licenses of the vendored components
```

The Relapse exploit assets (`data/src`, `data/offsets`, `data/payloads`, `data/exploit.html`) are **committed** (vendored) so this repository builds on its own, even if the upstream [Relapse-Exploit](https://github.com/ntfargo/Relapse-Exploit) repository becomes unavailable. Use `scripts/update_assets.py` to refresh them.

## Prerequisites

- [PlatformIO](https://platformio.org/) (VS Code extension or CLI).

## Build & flash

1. Build and upload the firmware:

   ```sh
   pio run -t upload
   ```

2. Upload the LittleFS filesystem (exploit assets + payload browser):

   ```sh
   pio run -t uploadfs
   ```

Alternatively, with the PlatformIO VS Code extension, use the **Upload** and **Upload Filesystem Image** actions.

## Updating the exploit assets

The Relapse assets are committed, but you can refresh them from upstream:

```sh
python3 scripts/update_assets.py
```

This downloads the [Relapse-Exploit](https://github.com/ntfargo/Relapse-Exploit) repository (ref `main` by default; pin a commit/tag in the script for reproducibility) and refreshes `data/src`, `data/offsets`, `data/payloads` and `data/exploit.html`. Re-run `pio run -t uploadfs` afterwards.

`data/pl/usb-payload-browser.elf` is prebuilt and committed; to rebuild it, see [usb-payload-browser](https://github.com/leafunes/ps5-usb-payload-browser).

## Usage

1. Connect the PS5 to the **PS5-Relapse** WiFi network (password `relapse123`).
2. Open **Settings → User's Guide**; the captive portal redirects to the landing page.
3. Tap **Run exploit** and wait for "elfldr is listening on port 9021".
4. Tap **Load payloads**; the ESP32 sends `usb-payload-browser` and opens `http://<ps5-ip>:9090/`, which lists `.elf` payloads from a FAT32/exFAT USB drive. Select one with X to load it.

The SSID, password and IP address are configurable at the top of `src/main.cpp`.

## Credits & dependencies

- [Relapse-Exploit](https://github.com/ntfargo/Relapse-Exploit) — the PS5 WebKit + kernel exploit (MIT). Authors: Sonic_Iso, Jordy, ntfargo, ufm42, Dr. Yenyen.
- [esp32_https_server](https://github.com/fhessel/esp32_https_server) — HTTPS server library (MIT, Frank Hessel).
- [usb-payload-browser](https://github.com/leafunes/ps5-usb-payload-browser) — the USB payload browser payload used by "Load payloads".

## License

The ESP32 code and landing page in this repository are licensed under the [MIT License](LICENSE). The vendored third-party assets under `data/` keep their original licenses — see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

## Disclaimer

This project is intended for educational and security research purposes only. It does not endorse piracy or unauthorized access. Use it only on devices you own or are authorized to test, and comply with applicable laws. The software is provided as-is, without warranty.
