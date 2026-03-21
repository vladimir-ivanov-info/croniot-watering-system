# croniot-watering-system

**An autonomous solar-powered plant watering system running on ESP32-C6.**

Controls water pumps and solenoid valves, monitors battery and solar panel status, and streams real-time data to a mobile app — all remotely manageable from anywhere through a cloud server via MQTT.

~2,000 lines of device-specific code on top of the [croniot-iot](https://github.com/user/croniot-iot) framework (~4,500 lines). **70% of the code is reused** — connectivity, authentication, HTTP / MQTT routing, and persistence come from the framework, so this project focuses entirely on what the device does.

### Click below to watch a full demo of croniot-watering-system in action

[![Demo video](https://img.youtube.com/vi/sxZ00cP9hIw/maxresdefault.jpg)](https://www.youtube.com/watch?v=sxZ00cP9hIw)

| Mobile app view | Watering System's sensors | Watering System's task types
|-------|-------------|---------------|
| <img src="https://github.com/user-attachments/assets/cd27be2e-1d4f-46ed-a95f-bbec3fe6605d" width="320"> | <img src="https://github.com/user-attachments/assets/d358e151-9356-4e53-bb6c-8f6e5ca79828" width="320"> | <img src="https://github.com/user-attachments/assets/c3ffa9cd-32e9-45f0-9d6f-8a593c10ac07" width="320"> |

| Scheduled task | Stateful task
|-------|-------------|
|  <img src="https://github.com/user-attachments/assets/84161598-574b-4171-a8ba-ec5840636a6f" width="320"> | <img src="https://github.com/user-attachments/assets/44c3d697-619d-4d16-856c-4f63870e46c7" width="320"> |


---

## What it does

- Waters plants on demand or on a schedule, with real-time progress updates
- Monitors battery voltage, current, and charge percentage via ADS1115 ADC
- Tracks solar panel power generation
- Reports WiFi signal strength, RAM usage, and system time
- Controls LEDs (on/off, timed, PWM brightness) for testing and signaling
- Operates autonomously on solar + battery power

The ESP32 connects to WiFi, reaches the croniot server (local or cloud VPS), and communicates over MQTT. The Croniot Android app connects to the same server — so you can monitor and control the device from anywhere.

## Hardware

| Component | Connection | Function |
|---|---|---|
| ESP32-C6-DevKitC-1 | — | Main controller (RISC-V) |
| Water pump relay | GPIO 4 | Pump control (active-low) |
| Solenoid valve 1 | GPIO 5 | Normally closed valve |
| Solenoid valve 2 | GPIO 18 | Normally closed valve |
| LED | GPIO 2 | Status/test LED (PWM capable) |
| ADS1115 (I2C) | SDA: GPIO 21, SCL: GPIO 22 | 16-bit ADC for analog measurements |
| ACS712 (20A) | ADS1115 CH0 | Battery current sensing |
| Voltage divider (5:1) | ADS1115 CH1 | Battery voltage sensing (0–25V) |
| Solar current sensor | ADS1115 CH2 | Solar panel current |
| Solar voltage divider | ADS1115 CH3 | Solar panel voltage |
| Server select switch | GPIO 10 | HIGH: remote server, LOW: local |

## Tasks

| Task | Parameters | What it does |
|---|---|---|
| **Water Plants** | Duration (1–600s), Schedule (HH:MM) | Opens valve, starts pump, waters for N seconds, stops pump, closes valve. Reports progress 0–100%. |
| **Water Pump Switch** | on / off | Direct pump control for manual operation. |
| **LED Test** | Duration (1–10s) | Turns LED on for N seconds with progress reporting. |
| **LED Switch** | on / off | Persistent LED toggle. |
| **LED Slider** | Brightness (0–100) | PWM brightness control at 5kHz, 8-bit resolution. |

## Sensors

| Sensor | Interval | Source | Data |
|---|---|---|---|
| **Battery %** | 3s | ADS1115 CH0 + CH1 | Charge level (10V = 0%, 13.2V = 100%) |
| **Battery Power** | 3s | ADS1115 CH0 + CH1 | Watts (voltage x current) |
| **Solar Power** | 1–5s | ADS1115 CH2 + CH3 | Watts (only when generating) |
| **WiFi Signal** | 1s | esp_wifi | RSSI in dBm |
| **RAM Usage** | 1s | heap_caps | Used memory in KB |
| **System Time** | 1s | Server sync | HH:MM:SS |

## Watering sequence

```
1. Open valve 1              (wait 8s for full open)
2. Start water pump
3. Water for N seconds        (progress updates every 1s)
4. Stop water pump            (wait 1s)
5. Close valve 2              (wait 8s for full close)
```

The mobile app shows a progress bar from 0% to 100% during the entire sequence. If an error occurs, it's reported back immediately.

## Project structure

```
croniot-watering-system/
├── main/
│   └── main.cpp                    # Entry point, watchdog, network init
├── Tasks/
│   ├── TaskWaterPlants.h/cpp       # Watering sequence with valve/pump control
│   ├── TaskWaterPumpSwitch.h/cpp   # Direct pump on/off
│   ├── TaskLedTest.h/cpp           # Timed LED with progress
│   ├── TaskLedSwitch.h/cpp         # LED toggle
│   └── TaskLedSlider.h/cpp         # PWM brightness control
├── Sensors/
│   ├── Battery/
│   │   └── SensorBattery.h/cpp     # Battery voltage, current, percentage
│   ├── SensorSolarPanel.h/cpp      # Solar panel power monitoring
│   ├── SensorWiFiInfo.h/cpp        # WiFi RSSI
│   ├── SensorRamUsage.h/cpp        # Heap usage
│   ├── SensorCurrentTime.h/cpp     # Server-synced time
│   ├── ADS1115Controller.h/cpp     # I2C ADC driver (singleton)
│   └── SensorDefs.h                # Sensor/task ID constants
├── SensorsInitializer.h/cpp        # Sensor type registration
├── TasksInitializer.h/cpp          # Task type registration
├── secrets.h                       # WiFi credentials & server config
├── components/
│   ├── ads1x15/                    # ADS1115 ADC library
│   └── i2c/                        # I2C abstraction
├── partitions.csv                  # Flash partition table
└── platformio.ini                  # Build configuration
```

## How it works with croniot-iot

```
croniot-iot (framework)          croniot-watering-system (this project)
─────────────────────            ────────────────────────────────────────
TaskBase                    ←──  TaskWaterPlants, TaskLedSwitch, ...
Sensor                      ←──  SensorBattery, SensorSolarPanel, ...
TaskController                   registers 5 task types
SensorsController                registers 6 sensor types
CommonSetup                      boots WiFi, auth, MQTT automatically
MQTT routing                     receives commands, publishes data
```

The framework handles connectivity, authentication, and message routing. This project only defines **what the device does**.

## Build & flash

Requires [PlatformIO](https://platformio.org/).

```bash
# Build
pio run -e esp32-c6-devkitc-1

# Flash
pio run -e esp32-c6-devkitc-1 -t upload

# Monitor serial output
pio device monitor -b 115200
```

### Configuration

Copy `secrets_mock.h` to `secrets.h` and fill in your WiFi credentials and server address.

## Tech stack

| Component | Technology |
|---|---|
| MCU | ESP32-C6 (RISC-V, 160MHz) |
| Framework | ESP-IDF via PlatformIO |
| Language | C++17 |
| IoT Framework | [croniot-iot](https://github.com/user/croniot-iot) |
| ADC | ADS1115 (16-bit, I2C) |
| Current Sensor | ACS712 (20A model) |
| Storage | LittleFS |
| Communication | MQTT + HTTP (WiFi → cloud server) |
| Power | 12V battery + solar panel |