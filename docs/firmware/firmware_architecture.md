# Firmware Architecture
ESP32 + GPS + LoRaWAN Vehicle Tracking Node

---

## 1. Overview

The firmware running on the ESP32 is designed as a **low-power, event-driven system** optimized for long-range IoT deployments using LoRaWAN.

Instead of running continuously, the firmware executes a **single tracking cycle**, transmits relevant data, and then enters **deep sleep** to conserve energy. This design is critical for battery-powered vehicle nodes deployed in large industrial campuses.

---

## 2. Design Principles

The firmware follows these core principles:

- **Single-cycle execution**: One wake–process–transmit–sleep loop
- **Edge intelligence**: Filter insignificant GPS movement locally
- **Power-first design**: Aggressive use of ESP32 deep sleep
- **Modularity**: Clear separation of concerns across components
- **LoRaWAN-aware**: Minimal airtime usage and compact payloads

---

## 3. High-Level Firmware Flow

```
Wake from Deep Sleep
        ↓
Initialize Peripherals
        ↓
Acquire GPS Fix
        ↓
Movement Filtering
        ↓
Payload Encoding
        ↓
LoRaWAN Uplink
        ↓
Adaptive Sleep Decision
        ↓
Enter Deep Sleep
```

The ESP32 resets after every deep sleep wake-up and restarts execution from `setup()`.

---

## 4. Module Breakdown

The firmware is divided into the following logical modules:

### 4.1 Main Application (`main.cpp`)

**Role:**
- Orchestrates the overall firmware flow
- Initializes all managers
- Controls adaptive tracking logic
- Triggers deep sleep

**Key Responsibilities:**
- Call GPS acquisition
- Decide whether transmission is required
- Select appropriate sleep interval

---

### 4.2 GPS Manager (`gps_manager.cpp`)

**Role:**
- Interface with NEO-6M GPS module
- Parse NMEA data using TinyGPS++
- Provide validated latitude and longitude

**Key Features:**
- Timeout-based GPS fix acquisition
- Distance-based movement detection
- GPS drift filtering using threshold (≥10 m)

**Why edge filtering matters:**
Filtering insignificant movement at the node reduces unnecessary LoRaWAN transmissions and conserves both power and network airtime.

---

### 4.3 LoRaWAN Manager (`lorawan_manager.cpp`)

**Role:**
- Handle LoRaWAN stack initialization (LMIC)
- Encode GPS data into compact binary payloads
- Transmit uplink packets

**Design Choices:**
- LoRaWAN Class A (lowest power)
- Unconfirmed uplinks (energy-efficient)
- Fixed-point payload encoding (no JSON)

**Security Note:**
ABP activation is used for prototype simplicity. OTAA is planned for production deployments.

---

### 4.4 Power Manager (`power_manager.cpp`)

**Role:**
- Control ESP32 power states
- Configure deep sleep timers
- Ensure graceful shutdown before sleep

**Wake-up Source:**
- Timer-based wake-up (`esp_sleep_enable_timer_wakeup`)

**Future Extensions:**
- IMU-based wake on motion
- GPIO-based external wake
- RTC memory for state persistence

---

## 5. Adaptive Tracking Logic

The firmware dynamically adjusts its behavior based on vehicle motion:

| Vehicle State | GPS Transmission | Sleep Interval |
|--------------|------------------|---------------|
| Moving       | Enabled          | 30–60 seconds |
| Stationary   | Skipped          | 5–10 minutes  |

This strategy significantly increases battery life compared to fixed-interval tracking.

---

## 6. Payload Design

To comply with LoRaWAN payload constraints:

- Latitude and longitude are encoded as **32-bit fixed-point integers**
- Scaling factor: `1e6`
- Total payload size: **8 bytes**

Example:

```
[ Lat (4 bytes) | Lon (4 bytes) ]
```

Payload decoding is handled at the backend.

---

## 7. Fault Handling Strategy

The firmware handles common failure scenarios gracefully:

- **GPS fix failure** → Enter short retry sleep
- **LoRaWAN busy state** → Skip transmission and retry later
- **Packet loss** → Tolerated by periodic uplinks

The system is designed to be **fault-tolerant**, not fault-free.

---

## 8. Power Consumption Considerations

Major power consumers:
- GPS module (highest)
- LoRa transmission (short burst)
- ESP32 active mode

Mitigation strategies:
- GPS duty cycling
- Minimal active CPU time
- Deep sleep between cycles

Estimated battery life (prototype):
- 3–5 days under frequent movement
- Weeks possible with extended idle periods

---

## 9. Limitations

- GPS performance degrades indoors
- No downlink-heavy control due to Class A constraints
- Consumer-grade GPS accuracy (2–5 m drift)

These limitations are acceptable for the intended industrial use case.

---

## 10. Summary

This firmware architecture demonstrates a **practical, scalable, and power-efficient design** for real-time vehicle tracking using LoRaWAN. By combining embedded edge intelligence with cloud-based analytics, the system minimizes infrastructure cost while maintaining operational visibility across large industrial campuses.
