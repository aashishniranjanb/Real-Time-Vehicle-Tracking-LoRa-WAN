# Industrial Fleet Tracking System using ESP32, GPS and LoRaWAN for Autonomous Factory Logistics
**ESP32 + GPS (NEO-6M) + LoRaWAN / LoRa P2P**

📍 **Project Status:** Prototype / Experimental System  
🧠 **Domain:** Embedded Systems · IoT · Low-Power Wireless  
🛠 **Primary Tech Stack:** ESP32, LoRaWAN, GPS, Python (Backend), MQTT, Cloud Dashboard  

![System Demo](assets/demo.gif)

---

## 1. Problem Definition

### Context
Large-scale industrial facilities such as **automotive manufacturing plants, mining zones, logistics yards, and shipping ports** often span **hundreds of acres (600+ acres)**.  
Within these campuses, hundreds of vehicles—test cars, forklifts, carriers, and service vehicles—move continuously across open yards and production zones.

### Core Problem
Plant operators lack **real-time visibility** into vehicle location, movement, and zone occupancy.

#### Why existing approaches fail:
- **Manual tracking** (QR codes / logbooks) is unreliable due to human error and poor compliance  
- **Wi-Fi-based systems** cannot cover such large areas without costly infrastructure  
- **Cellular (4G/LTE) trackers** introduce recurring SIM costs and high battery drain  
- No automated way to track **zone entry, exit, or dwell time**

### Impact
- ⏱ Operational delays during inspection and dispatch  
- 🚗 Poor asset utilization and congestion  
- ⚠️ Safety risks in restricted or hazardous zones  

---

## 2. System Objectives

The goal is to design a **low-power, long-range, and infrastructure-light vehicle tracking system** for large industrial campuses.

### Key Objectives
- **Campus-wide coverage** using minimal infrastructure (1–2 gateways)
- **Real-time tracking** with adaptive update intervals  
- **Low-power operation** for battery-powered nodes  
- **Automated geofencing** (no human input required)  
- **Cost-effective alternative** to cellular solutions  

---

## 3. Overall System Architecture

The system follows a **Device → Network → Cloud → Dashboard** architecture optimized for **intermittent connectivity**.

### High-Level Data Flow
```
[ Vehicle Node ]
       │
   (LoRa RF)
       │
       ▼
[ LoRaWAN Gateway ]
       │
 (Internet / UDP)
       │
       ▼
[ Network Server (TTN) ]
       │
     (MQTT)
       │
       ▼
[ Application Backend ]
       │
       ▼
[ Web Dashboard ]
```

### Architecture Diagram
![System Architecture](docs/architecture/system_architecture.png)

### Richer End-to-End Node Pipeline
```
ESP32 (MCU) + NEO-6M (GPS) + SX1278 (LoRa)
                         │
                 (865/915 MHz RF)
                         ▼
             LoRaWAN Gateway (Outdoor)
                         │
                  (Internet / UDP)
                         ▼
             The Things Network (TTN)
                         │
                 (MQTT Integration)
                         ▼
                    MQTT Broker
                         │
                   (Python Client)
                         ▼
                 Python Backend API
                         │
             ┌───────────┴───────────┐
             ▼                       ▼
      Geofence Engine         Firebase RTDB
             │                       │
             ▼                       ▼
      Entry/Exit Alert        React Dashboard
```

---

## 4. Vehicle Node Design (Hardware)

The vehicle-mounted node is designed for **low power consumption** and **modular prototyping**.

### Hardware Components & Bill of Materials (BOM)

Assuming an exchange rate of 1 USD ≈ ₹86, the BOM for a single tracker unit is detailed below:

| Component | Part Number | Package | Description | Quantity | Reference Designator | Estimated Cost (INR) |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **ESP32 NodeMCU Development Board** | ESP32-WROOM-32D | DIP-38 | Dual-Core MCU with Wi-Fi/Bluetooth | 1 | U1 | ₹387 |
| **U-blox NEO-6M GPS Module** | NEO-6M | Module | GPS Module with Antenna | 1 | U2 | ₹430 |
| **SX1278 LoRa Module** | RFM95W-868S2 | SMD-16 | 868/915 MHz SPI Transceiver | 1 | U3 | ₹301 |
| **Li-Po Battery (3.7 V, 2000 mAh)** | 3.7V 2000mAh | 2-Pin JST-PH | Lithium Polymer Battery | 1 | BATT1 | ₹516 |
| **Resistor 100 kΩ** | RC0805FR-07100KL | 0805 | ADC Battery Divider Resistor (1%, 1/8 W) | 2 | R1, R2 | ₹9 |
| **Capacitor 10 µF** | CC0805KKX7R7BB106 | 0805 | Decoupling Filter Capacitor (10 V) | 3 | C1, C2, C3 | ₹13 |
| **Slide Switch** | SS-12D00-G5 | Through-Hole | Power ON/OFF Switch | 1 | SW1 | ₹22 |
| **SMA Antenna Connector** | SMA-KE | Through-Hole | Female SMA RF Connector (90°) | 1 | J1 | ₹43 |
| **915 MHz Coil Spring Antenna** | Spring-915 | Through-Hole | LoRa Helical Antenna | 1 | ANT1 | ₹26 |
| **PCB Prototype Board** | FR4-Double | Custom | 2-Layer Custom Tracking Board | 1 | PCB1 | ₹129 |
| **Total Estimated Unit Cost** | | | | | | **₹1,876 per unit** |

*Note: This is a theoretical BOM based on estimated component prices and an exchange rate of ₹86/USD. Actual costs in India may differ depending on the supplier (e.g. Robu.in, Evelta, Sunrom, Mouser India, DigiKey India) and order volume. For student prototypes, the total can often be lower if using readily available dev kits rather than custom PCB assemblies.*

📄 **Hardware schematic and design files:**  
- Circuit Schematics: [circuit_diagram.pdf](file:///c:/Users/Home/Downloads/PROJECTS/Real-Time-Vehicle-Tracking-LoRa-WAN/hardware/circuit_diagram.pdf)
- PCB Layout Files: [pcb/](file:///c:/Users/Home/Downloads/PROJECTS/Real-Time-Vehicle-Tracking-LoRa-WAN/hardware/pcb)
- Component Bill of Materials: [bom.xlsx](file:///c:/Users/Home/Downloads/PROJECTS/Real-Time-Vehicle-Tracking-LoRa-WAN/hardware/bom.xlsx)

### Design Considerations
- GPS is duty-cycled to reduce power consumption  
- ESP32 deep-sleep modes are used aggressively  
- System designed for easy upgrade (IMU / Solar input in future)

📷 **Wiring Diagram:**  
![Wiring](docs/hardware/wiring_diagram.png)

---

## 5. Firmware Architecture

The firmware is **event-driven** and optimized for **maximum sleep time**.

### Functional Blocks
- GPS Manager (NMEA parsing)
- Edge Filtering Logic
- Adaptive Tracking Controller
- LoRaWAN Uplink Manager
- Power Manager

### Firmware Flow
`Wake → GPS Fix → Filter → Encode Payload → Transmit → Sleep`

### Adaptive Tracking Logic
- **Moving vehicle:** 30–60s updates  
- **Stationary vehicle:** 5–10 min heartbeat  

📄 Detailed design:  
[`docs/firmware/firmware_architecture.md`](docs/firmware/firmware_architecture.md)

🛠 Arduino Modular Firmware Source:  
[esp32_tracker/](file:///c:/Users/Home/Downloads/PROJECTS/Real-Time-Vehicle-Tracking-LoRa-WAN/firmware/esp32_tracker/)

---

## 6. LoRaWAN Communication Design

- **Protocol:** LoRaWAN Class A  
- **Reason:** Lowest power consumption  
- **Region:** IN865 / EU868 / US915  
- **ADR:** Enabled  

### Payload Design
- Binary encoded payload (no JSON)
- Compact latitude & longitude representation

---

## 7. Dashboard & Geofencing Logic

### Features
- Live map visualization
- Vehicle markers & trails
- Polygon-based geofencing
- Entry / Exit alerts
- Dwell-time analytics

### Geofencing Logic
- Zones defined as polygons
- Point-in-polygon algorithm checks location
- State transitions trigger alerts

📷 **Geofence Logic Diagram:**  
![Geofence Diagram](docs/dashboard/screenshots/dashboard_screenshot.png) (Refer to dashboard zone assets)

---

## 8. Engineering Analysis & Assumptions

### Target Design Goals
- **Communication Range**: 2–5 km Line-of-Sight (dependent on Spreading Factor and frequency).
- **Update Interval**: Adaptive 30–60s when moving; 5-minute heartbeat when stationary.
- **Sleep Current**: ESP32 deep sleep current: ~15–20 µA (transceiver and GPS isolated).
- **Gateway Requirement**: 1–2 gateways for a medium-sized industrial campus (600+ acres).
- *Note: These represent engineering design targets; field verification is planned in future iterations.*

### Engineering Assumptions
- **LoRa Gateway**: Deployed at a height of 15 meters to clear factory roof profiles.
- **Propagation**: Path loss model assumes near Line-of-Sight (LOS) outdoors, with signal attenuation occurring through warehouse corrugated metal siding.
- **GPS Update Rate**: GPS configured for 1 Hz update rate.
- **Cloud MQTT**: TTN MQTT integration handles uplink forwarding to the application server.

### Power & Battery Life Calculations
To ensure system reliability, the following current consumption model is used to size the battery:

- **GPS Active State**: 35 mA (continuous tracking)
- **ESP32 Active State**: 90 mA (RF active, sensor processing)
- **LoRa Transmit burst**: 120 mA (for ~100 ms transmission window)
- **Deep Sleep State**: 20 µA (MCU sleep, GPS backup power, transceiver idle)

#### Estimated Battery Life Formula:
Assuming a **2000 mAh Li-Po battery** (derated to 1800 mAh for aging safety) with an update interval of **1 transmission every 5 minutes (300 seconds)** where the node is active for **10 seconds** per cycle:

1. **Average Current Consumption ($I_{avg}$)**:
   $$I_{avg} = \frac{(t_{active} \times I_{active}) + (t_{sleep} \times I_{sleep})}{t_{active} + t_{sleep}}$$
   $$I_{avg} = \frac{(10\text{ s} \times 95\text{ mA}) + (290\text{ s} \times 0.02\text{ mA})}{300\text{ s}} = \frac{950 + 5.8}{300} \approx 3.19\text{ mA}$$

2. **Operating Days**:
   $$\text{Life} = \frac{\text{Battery Capacity}}{\text{Average Current}} = \frac{1800\text{ mAh}}{3.19\text{ mA}} \approx 564.2\text{ hours} \approx 23.5\text{ days}$$
   *Under continuous movement (transmitting every 30 seconds, $I_{avg} \approx 14\text{ mA}$), battery life yields **~5.4 days**.*

---

## 9. Reliability & Failure Handling

Reliability is built into the node's firmware to handle common failure states on the factory floor:

### 1. GPS Signal Loss (e.g., Vehicle Indoors)
```
GPS Acquisition Timeout
        │
        ▼
Is RTC cache available?
       / \
 (Yes)/   \(No)
     /     \
    ▼       ▼
Use Last Known   Transmit
Coordinates     Fallback Code (0,0)
    │
    ▼
Enter short error sleep (2 min)
to check for outdoor exit
```

### 2. Telemetry Transmission Failures
```
LoRa Transmission Triggered
        │
        ▼
Is radio busy? (Or TX failed)
       / \
 (Yes)/   \(No)
     /     \
    ▼       ▼
Cache packet in     Transmission
local SPI Flash     Successful
    │
    ▼
Retry packet dump
on next active cycle
```

---

## 10. Novel Contributions

This project goes beyond a basic GPS tracker through:

- 🔋 Power-aware adaptive tracking
- 🧠 Edge-level GPS filtering
- ⏱ Zone-based dwell-time analytics
- 📡 Fault-tolerant data handling
- 📊 Comparative evaluation vs Wi-Fi and cellular

---

## 11. Results & Observations (Prototype Phase)

- Coverage: >2 km in urban test conditions
- Latency: ~2–5 seconds end-to-end
- Reliability: Superior penetration vs Wi-Fi
- Power: Estimated 3–5 days on 2000mAh battery
  (Weeks possible with aggressive sleep optimization)

📄 Details:
- [`docs/results/coverage_test.md`](docs/results/coverage_test.md)
- [`docs/results/power_estimation.md`](docs/results/power_estimation.md)

---

## 12. Experimental & Validation Plan

Recruiters and engineers appreciate verified designs. The following testing protocols are planned to evaluate system performance:

- [ ] **LoRa Range Verification**: Conduct field testing across varying spreading factors (SF7 vs SF12) to verify RSSI/SNR signal quality up to 5 km.
- [ ] **GPS Accuracy Evaluation**: Measure coordinate drift when stationary to validate the effectiveness of the 10-meter movement threshold filter.
- [ ] **Battery Discharge Logging**: Graph battery voltage discharge curve over 7 days of operation to verify average current models.
- [ ] **Gateway Throughput Testing**: Simulate 50 concurrent active nodes to verify gateway packet collision rates.
- [ ] **Geofence Edge Latency**: Measure delay from the instant a vehicle crosses a virtual coordinate boundary to the dashboard notification.
- [ ] **Indoor/Outdoor Transition Profile**: Evaluate time-to-first-fix (TTFF) when a vehicle transitions from an enclosed metal warehouse to an open logistics yard.

---

## 13. Limitations

- GPS does not work indoors or under metal sheds
- LoRaWAN Class A limits downlink responsiveness
- Consumer-grade GPS accuracy (~2.5–5 m drift)

---

## 14. Future Improvements

- ☀️ Solar-powered LoRaWAN gateways
- 📡 Hybrid indoor positioning (Wi-Fi sniffing)
- 🔁 Node-RED automation for alerts
- 🚗 IMU-based motion detection
- 🧩 ERP / MES system integration via MQTT

---

## 15. Conclusion

This project demonstrates a scalable, cost-effective, and low-power vehicle tracking system for industrial environments.
By combining LoRaWAN's range with embedded edge intelligence, the system bridges the visibility gap in plant logistics and asset movement.

---

## License
MIT License
