# Zeen-AI-Plug (Zero Green AI Plug)


![Top-img](https://github.com/user-attachments/assets/5ac0f1da-6818-4c8d-b417-a88da1d11cab)


> An open-source, single-channel AI-powered smart plug built on ESP32-S3 with online (Sinric Pro / Google Home / Alexa) and offline timer-based control-designed and shared for the engineering community.

![Mode-img](https://github.com/user-attachments/assets/5fc3f96d-2854-4106-8230-d0a78a61ebe9)


![Bottom-img](https://github.com/user-attachments/assets/a86ba63d-ad8d-4c75-baa3-0929e45eb0a5)


---

## 📺 Full Course-Hardware Design With AI Co-Pilot

This project is part of a complete open-source hardware design course on YouTube:

**▶️ [Hardware Design With AI Co-Pilot-Full Playlist](https://youtube.com/playlist?list=PLxgq6Jtu7shR8w3cHLHZtZtIrhJVOUqG1&si=Gfo0e48VRceQaZlR)**

The course covers the complete design journey-from schematic to PCB layout, firmware, and cloud integration.

---



---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Hardware Specifications](#-hardware-specifications)
- [System Architecture](#-system-architecture)
- [Operating Modes](#-operating-modes)
- [Repository Structure](#-repository-structure)
- [Getting Started](#-getting-started)
- [Firmware Setup](#-firmware-setup)
- [Cloud & Voice Assistant Integration](#-cloud--voice-assistant-integration)
- [Manufacturing](#-manufacturing)
- [Contributing](#-contributing)
- [License](#-license)

---

## 🔍 Overview

The **Zeen-AI-Plug (ZAP-01A)** is an open-source, single-channel smart plug designed for the engineering community. It combines an ESP32-S3 microcontroller with cloud-based AI voice assistants and a robust offline timer mode, making it functional even without an internet connection.

The board runs on **220VAC mains input**, converting it to **5V DC** via an onboard HLK-5M05 power supply module, and further to **3.3V** via an AMS1117-3.3 LDO - powering the ESP32-S3 and all peripherals.

---

## ✨ Features

- ✅ Single-channel relay control (SPDT)
- ✅ Online mode - Sinric Pro, Google Home, Amazon Alexa
- ✅ Offline mode - RTC-based (DS3231M) timer control
- ✅ USB-C (USB 2.0) for firmware flashing
- ✅ ESD protection on USB lines (USBLC6-2P6)
- ✅ Onboard RTC with battery backup (DS3231M + CH291-1220LF)
- ✅ Status LEDs - Network, Error, General indication
- ✅ Buzzer feedback (MLT-8530)
- ✅ 4x tactile switches for local control
- ✅ MOV surge protection (10D561K)
- ✅ Screw terminal output for load connection
- ✅ Fiducial markers for PCB assembly
- ✅ Fully open-source - Hardware + Firmware

---

## 🔧 Hardware Specifications

| Parameter | Details |
|---|---|
| Microcontroller | ESP32-S3-WROOM-1 |
| Input Voltage | 220V AC |
| Operating Voltage | 5V DC / 3.3V DC |
| Power Supply Module | HLK-5M05 (220VAC → 5VDC) |
| LDO Regulator | AMS1117-3.3 |
| Relay | SPDT — 1 Channel |
| RTC | DS3231M (I2C) |
| RTC Battery | CH291-1220LF (Coin Cell) |
| USB Connector | USB-C (USB 2.0) |
| ESD Protection | USBLC6-2P6 |
| Surge Protection | MOV 10D561K |
| Fuse | 500mA / 250V |
| Status LEDs | Red, Blue, Green |
| Buzzer | MLT-8530 |
| Switches | 4x Tactile + 1x Boot + 1x Reset |
| PCB Tool | KiCad 9.0.1 |
| Board Revision | Rev-v0.1.0A |

---

## 🏗️ System Architecture

```
220V AC Input
     │
     ▼
[F1 Fuse 500mA] ──→ [MOV RV1 Surge Protection]
     │
     ▼
[HLK-5M05] ──→ 5V DC
     │
     ├──→ [Relay K1 SPDT] ──→ Load Output (Screw Terminal J2)
     │
     └──→ [AMS1117-3.3] ──→ 3.3V DC
               │
               ├──→ [ESP32-S3-WROOM-1] ←→ WiFi / BLE
               ├──→ [DS3231M RTC] (I2C)
               ├──→ [Status LEDs]
               ├──→ [Buzzer MLT-8530]
               ├──→ [Tactile Switches x4]
               └──→ [USB-C] ──→ Firmware Flash / Serial Debug
```

---

## ⚙️ Operating Modes

### 🌐 Online Mode
- Connects to WiFi and syncs with **Sinric Pro** cloud platform
- Voice control via **Google Home** and **Amazon Alexa**
- Remote control from anywhere via smartphone app
- Real-time status sync across all platforms

### 📴 Offline Mode
- Fully functional without internet connection
- **DS3231M RTC** maintains accurate time even after power loss
- Timer-based scheduling stored locally
- Local switch control always active in both modes

---

## 📁 Repository Structure

```
Zeen-AI-Plug/
├── Docs/
│   ├── 1-Datasheet/           # Component datasheets
│   ├── 2-Images/              # PCB photos, product images
│   ├── 3-Pin-Mapping/         # ESP32-S3 pin mapping reference
│   └── 4-Testing-Note/        # Test procedures and results
├── Firmware/
│   ├── 1-Test-Code/           # Individual peripheral test sketches
│   └── 2-ZAP-01A-Firmware/    # Main production firmware
├── Hardware/
│   ├── 1-External-3D-Models/  # 3D models for KiCad
│   ├── 2-External-Libraries/  # Custom KiCad footprints & symbols
│   ├── 3-Manufacturing-Files/ # Gerbers, BOM, CPL
│   └── 4-ZAP-01A/             # KiCad project files
└── README.md
```

---

## 🚀 Getting Started

### Prerequisites
- [KiCad 9.0.1](https://www.kicad.org/) — for hardware files
- [Arduino IDE](https://www.arduino.cc/en/software) or [VS Code + ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/) — for firmware
- [Sinric Pro Account](https://sinric.pro/) — for cloud integration
- ESP32-S3 board support package installed in Arduino IDE

### Clone the Repository
```bash
git clone https://github.com/ampnics/Zeen-AI-Plug.git
cd Zeen-AI-Plug
```

---

## 💻 Firmware Setup

### 1. Install ESP32-S3 Board Support
In Arduino IDE → Preferences → add this URL:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
Then: **Tools → Board → Boards Manager → search "esp32" → Install**

### 2. Install Required Libraries
```
Sinric Pro
ArduinoJson
WebSockets
NTPClient
RTClib (for DS3231)
```

### 3. Configure Credentials
Open `Firmware/2-ZAP-01A-Firmware/` and update:
```cpp
For WiFi press "SET" button for 10Sec for updating WiFi Credential via AP Mode
#define APP_KEY         "your_sinric_app_key"
#define APP_SECRET      "your_sinric_app_secret"
#define SWITCH_ID       "your_sinric_device_id"
```

### 4. Flash the Firmware
- Connect via **USB-C**
- Select Board: `ESP32S3 Dev Module`
- Press & hold **BOOT (SW1)** → tap **RESET (SW2)** → release BOOT
- Click **Upload**

---

## ☁️ Cloud & Voice Assistant Integration

### Sinric Pro Setup
1. Create account at [sinric.pro](https://sinric.pro)
2. Add new device → **Smart Plug**
3. Copy **App Key**, **App Secret**, **Device ID** into firmware
4. Flash firmware — device appears online automatically

### Google Home
1. Open Google Home app → **"+" → Set up device**
2. Search **"Sinric Pro"** → Link account
3. Your ZAP-01A plug appears as a controllable device
4. Say: *"Hey Google, turn on the plug"*

### Amazon Alexa
1. Open Alexa app → **Skills & Games**
2. Search **"Sinric Pro"** → Enable skill → Link account
3. Discover devices
4. Say: *"Alexa, turn on the plug"*

---

## 🏭 Manufacturing

Fabrication files are located in `Hardware/3-Manufacturing-Files/` and include:

- **Gerber files** - for PCB fabrication
- **BOM (Bill of Materials)** - component list with values and part numbers
- **CPL (Component Placement List)** - for SMT assembly

> ⚠️ **Safety Warning:** This board operates at **220V AC mains voltage**. Always ensure proper insulation and safety precautions during assembly and testing. Do not touch the board while connected to mains power.

---

## 🤝 Contributing

Contributions are welcome from the engineering community!

1. Fork the repository
2. Create your feature branch: `git checkout -b feature/your-feature`
3. Commit your changes: `git commit -m "feat: add your feature"`
4. Push to the branch: `git push origin feature/your-feature`
5. Open a **Pull Request**

**Commit message convention:**

| Prefix | Use for |
|---|---|
| `hw:` | Hardware / KiCad changes |
| `fw:` | Firmware changes |
| `docs:` | Documentation updates |
| `mfg:` | Manufacturing file updates |
| `fix:` | Bug fixes |

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

You are free to use, modify, and distribute this project for personal and commercial purposes with attribution.

---

## Designed By

**Md Ammar Maniyar** - [Ampnics]((https://www.linkedin.com/in/ammarece?utm_source=share_via&utm_content=profile&utm_medium=member_ios))

## Sponsered By
**NextPCB** - [NextPCB](https://www.nextpcb.com/?code=Ampnics)

## Explore More
**Ampnics** - [Ampnics](https://www.ampnics.com)

*Engineered in India 🇮🇳*

---

## Support the Project

If this project helped you, consider:

- **Starring** this repository
- **Subscribing** to the [YouTube Channel](https://www.youtube.com/@ampnics)
- **Forking** and contributing back to the project
