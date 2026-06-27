# Industrial Fleet Tracking System using ESP32, GPS and LoRaWAN for Autonomous Factory Logistics
**ESP32 + GPS (NEO-6M) + LoRaWAN / LoRa P2P**

📍 **Project Status:** Prototype / Experimental System  
🧠 **Domain:** Embedded Systems · IoT · Low-Power Wireless · Industrial Logistics  
🛠 **Primary Tech Stack:** ESP32, LoRaWAN (TTN) / LoRa P2P, GPS, Python Backend, Firebase RTDB, React Dashboard  

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

The system follows a rich **Device → Gateway → Network → Backend → Dashboard** pipeline optimized for low-power operation and event-driven updates.

### End-to-End Architecture Data Flow

```
+------------------------------------+
|            ESP32 Node              |
|   +----------------------------+   |
|   |         NEO-6M GPS         |   |
|   +--------------+-------------+   |
|                  | (UART)          |
|   +--------------v-------------+   |
|   |        SX1278 LoRa         |   |
|   +--------------+-------------+   |
+------------------|-----------------+
                   | (865/915 MHz RF)
                   ▼
+------------------------------------+
|          LoRaWAN Gateway           |
|   - Listens on regional channels   |
|   - Forwards packets to Network    |
+------------------|-----------------+
                   | (UDP / MQTT over IP)
                   ▼
+------------------------------------+
|        The Things Network          |
|   - Network Server (Deduplication) |
|   - Decodes AppSKey / NwkSKey      |
+------------------|-----------------+
                   | (MQTT Integration)
                   ▼
+------------------------------------+
|            MQTT Broker             |
|   - Publishes uplink JSON objects  |
+------------------|-----------------+
                   | (TCP Connection)
                   ▼
+------------------------------------+
|          Python Backend            |
|   - Subscribes to device uplinks   |
|   +--------------+-------------+   |
|                  |                 |
|   +--------------v-------------+   |
|   |       Geofence Engine      |   |
|   |   - Point-in-Polygon check |   |
|   +--------------+-------------+   |
|                  |                 |
|   +--------------v-------------+   |
|   |      Firebase Database     |   |
|   |   - Real-time coordinates  |   |
|   +--------------+-------------+   |
+------------------|-----------------+
                   | (WebSocket / Live Sync)
                   ▼
+------------------------------------+
|          React Dashboard           |
|   - Live Map Visualization         |
|   - Alert Banner for Geofence Exits|
+------------------------------------+
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

---

## 5. Firmware Architecture

The firmware is **event-driven** and modular, prioritizing **maximum deep sleep time**.

### Module Design (Arduino Project: [esp32_tracker/](file:///c:/Users/Home/Downloads/PROJECTS/Real-Time-Vehicle-Tracking-LoRa-WAN/firmware/esp32_tracker/))
- **`esp32_tracker.ino`**: Coordinates the setup and execution loop.
- **`config.h`**: Stores central pin assignments, regional frequencies, and settings.
- **`gps.cpp` / `.h`**: Handles serial GPS connection, NMEA parsing, and location validation.
- **`lora.cpp` / `.h`**: Controls P2P LoRa radio transmission and power states.
- **`payload.cpp` / `.h`**: Compresses coordinates and battery voltage into a compact 14-byte binary payload to minimize airtime.
- **`power.cpp` / `.h`**: Handles deep sleep registers, battery ADC reads, and low battery brownout protection.
- **`tracking.cpp` / `.h`**: Calculates adaptive sleep intervals by detecting movement. Stores last-known coordinates in deep-sleep-resistant ESP32 RTC memory.
- **`geofence.cpp` / `.h`**: Local geofencing checks on the edge (e.g. checks if coordinate is inside factory gates).
- **`logger.cpp` / `.h`**: Pre-formatted debugging logging.
- **`utils.cpp` / `.h`**: General support utilities (e.g. course heading to compass conversion).

---

## 6. Engineering Analysis & Assumptions

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

## 7. Reliability & Failure Handling

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

## 8. Experimental & Validation Plan

Recruiters and engineers appreciate verified designs. The following testing protocols are planned to evaluate system performance:

- [ ] **LoRa Range Verification**: Conduct field testing across varying spreading factors (SF7 vs SF12) to verify RSSI/SNR signal quality up to 5 km.
- [ ] **GPS Accuracy Evaluation**: Measure coordinate drift when stationary to validate the effectiveness of the 10-meter movement threshold filter.
- [ ] **Battery Discharge Logging**: Graph battery voltage discharge curve over 7 days of operation to verify average current models.
- [ ] **Gateway Throughput Testing**: Simulate 50 concurrent active nodes to verify gateway packet collision rates.
- [ ] **Geofence Edge Latency**: Measure delay from the instant a vehicle crosses a virtual coordinate boundary to the dashboard notification.
- [ ] **Indoor/Outdoor Transition Profile**: Evaluate time-to-first-fix (TTFF) when a vehicle transitions from an enclosed metal warehouse to an open logistics yard.

---

## License
MIT License
