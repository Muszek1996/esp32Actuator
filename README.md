# esp32Actuator

ESP32 firmware for a temperature-controlled linear actuator (e.g. a window or vent opener). It reads DS18B20 temperature sensors and drives the actuator through a PWM H-bridge to hold a target temperature. A built-in web UI lets you watch readings and control it live.

## Features
- DS18B20 sensors on a OneWire bus (GPIO 15), averaged readings
- Actuator PWM on GPIO 25/26 (20 kHz, 10-bit), open/close/stop with adjustable speed
- Target temperature with tolerance (default 22 °C ± 0.05)
- Web UI served from SPIFFS (`data/`), live updates over WebSocket `/ws`
- Settings (speed, target temp) persisted in NVS
- OTA firmware updates (ArduinoOTA)
- FreeRTOS tasks for sensing, reporting and control

## Hardware
- ESP32 DevKit v1 (`esp32doit-devkit-v1`)
- DS18B20 sensor(s) + 4.7 kΩ pull-up
- H-bridge motor driver + linear actuator

## Build & flash
Requires [PlatformIO](https://platformio.org/).

1. Set your WiFi SSID/password in `src/Components/wifi.h`.
2. Flash firmware: `pio run -t upload`
3. Upload web UI: `pio run -t uploadfs`
4. Open the IP printed on the serial monitor (`pio device monitor`).

## Structure
```
src/main.cpp                 setup, PWM, FreeRTOS tasks
src/Components/Actuator/     motor control
src/Components/TempSensor/   DS18B20 reading, target logic
src/Components/http.h        async web server, SPIFFS
src/Components/webSocketAsync.h  WebSocket protocol
src/Components/nvs.h         persistent settings
src/Components/ota.h         OTA updates
data/                        web UI (index.html, WebSocket.js)
```
