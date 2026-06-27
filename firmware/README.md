# Modular ESP32 Tracking Firmware

This directory houses the refactored, production-ready firmware for the ESP32 vehicle tracking node. The firmware is structured using clean separation of concerns, moving away from monolithic files into distinct modules.

## Directory Structure

```
firmware/esp32_tracker/
├── esp32_tracker.ino        # Main entry point (Setup & Loop)
├── config.h                 # Central pin definitions, sleep times, settings
├── gps.cpp / .h             # GPS module communication (NEO-6M)
├── lora.cpp / .h            # LoRa P2P radio communication (SX1278)
├── payload.cpp / .h         # Compact fixed-point binary payload encoder
├── power.cpp / .h           # Deep sleep, battery measuring & protection
├── tracking.cpp / .h        # Adaptive update rate logic & RTC memory caching
├── geofence.cpp / .h        # Local bounding-box geofencing on the edge
├── logger.cpp / .h          # Formatted serial debug logging
└── utils.cpp / .h           # Generic mathematical & translation helpers
```

## Module Responsibilities

### 1. Main Entry (`esp32_tracker.ino`)
Orchestrates the wake-to-sleep single-cycle flow. It boots, reads the battery, checks for low-voltage, attempts to acquire a GPS coordinate, evaluates whether the node has moved, packages the coordinates, transmits the LoRa frame, and triggers a low-power deep sleep cycle.

### 2. Configuration (`config.h`)
Contains all pin mappings (SPI for SX1278, UART for NEO-6M, Analog ADC for battery voltage), regional frequency definitions, Spreading Factor thresholds, and constants.

### 3. GPS (`gps.cpp` / `.h`)
Wraps the `TinyGPS++` library and reads raw NMEA strings from UART1. Responsible only for returning cleaned GPS variables:
- `double latitude()`
- `double longitude()`
- `float speed()`
- `uint8_t satellites()`
- `float hdop()` (Quality check)

### 4. LoRa (`lora.cpp` / `.h`)
Uses SPI to communicate with the SX1278 transceiver. Avoids the complex overhead of LoRaWAN network handshakes for direct P2P local telemetry transmission. Automatically sleeps the transceiver after transmission.

### 5. Payload (`payload.cpp` / `.h`)
Compresses raw coordinates, speed, course, and battery voltage into a compact **14-byte binary stream**. By mapping floats to fixed-point scaled integers (e.g. Lat * 1,000,000), it avoids bulky JSON formatting, reducing LoRa airtime and transmission currents.

### 6. Power (`power.cpp` / `.h`)
Implements low-power strategies:
- Measures Li-Po voltage through a resistor divider.
- Implements **Low Battery protection**: locks the board into indefinite sleep if cell voltage drops below 3.4V to prevent lithium cell swelling or damage.
- Maps ESP32 wakeup registers for telemetry log analysis.

### 7. Tracking (`tracking.cpp` / `.h`)
Calculates coordinates delta using the **Haversine formula**. Integrates **ESP32 RTC slow memory** (`RTC_DATA_ATTR`) to persist coordinates across deep sleep resets. Implements adaptive sleep frequencies:
- Moving: 30-second interval.
- Stationary: 5-minute heartbeat interval.

### 8. Geofence (`geofence.cpp` / `.h`)
Enables local coordinate processing against bounding boxes. Returns zone names (e.g. "Loading Dock", "Finished Goods Yard") immediately to the logger.

### 9. Logger (`logger.cpp` / `.h`)
Standardizes system status messages in the serial stream with severity headers: `[INFO]`, `[WARN]`, `[ERROR]`, `[DEBUG]`.

---

## Getting Started & Compilation

### Requirements
The following libraries are required to compile the firmware:
1. **TinyGPS++** (by Mikal Hart) - For NMEA string parsing.
2. **LoRa** (by Sandeep Mistry) - For P2P SX1278 transceiver communication.

### Compilation via Arduino IDE
1. Rename the folder `esp32_tracker` if needed, ensuring the folder name matches the main file name `esp32_tracker.ino`.
2. Open `esp32_tracker.ino` in the Arduino IDE.
3. Install the **ESP32 Board Library** via the Boards Manager.
4. Install **TinyGPS++** and **LoRa** via the Library Manager.
5. Select **ESP32 Dev Module** as the target board.
6. Press Compile / Upload.

### Verification of Operations
Open the Serial Monitor at **115200 Baud** during power-up. You should see the welcome banner followed by system diagnostic outputs:
```
=========================================
   ESP32 LORA VEHICLE TRACKER ACTIVE     
=========================================
[INFO] Wakeup source: POWER-ON / HARD RESET
[INFO] Battery status healthy: 4.12 V
[INFO] Initializing GPS module on UART1 (RX=16, TX=17, Baud=9600)
[INFO] Configuring SPI for LoRa: SCK=5, MISO=19, MOSI=27, NSS=18
[INFO] Starting LoRa transceiver at 865.0 MHz...
[INFO] LoRa P2P transceiver connected successfully.
[INFO] Node initialization completed successfully.
-----------------------------------------
[INFO] Starting tracking telemetry cycle...
[INFO] Acquiring GPS fix...
[INFO] GPS lock acquired! Lat: 12.971600, Lon: 77.594600, Sats: 8, HDOP: 1.20
[INFO] Local Geofence hit: Vehicle is inside [Assembly Building]
[INFO] Location telemetry check: Zone = 'Assembly Building', Heading = East
[INFO] Initial coordinate baseline established.
[INFO] Vehicle activity detected. Encoding telemetry data...
[DEBUG] Serialized Payload: [ 00 C5 EE C0 | 04 9F FD C8 | 00 00 | 23 28 | 10 18 ]
[INFO] Sending LoRa P2P package (14 bytes)...
[INFO] LoRa P2P package transmitted successfully in 65 ms
[INFO] Putting LoRa module to sleep mode...
[INFO] Vehicle state: MOVING. Adaptive frequency = 30 seconds.
[INFO] Telemetry cycle complete. Shutting down into deep sleep...
=========================================================
```
