# Backend Architecture
LoRaWAN → MQTT → Processing → Database → Dashboard

---

## 1. Overview

The backend acts as the **central data processing and orchestration layer** of the system.  
It bridges low-power embedded devices with cloud-based visualization and analytics while remaining lightweight and scalable.

The backend is intentionally designed to:
- Avoid heavy computation
- Handle unreliable uplinks
- Remain stateless at the processing layer
- Push complexity to software-configurable logic (geofencing, alerts)

---

## 2. Backend Responsibilities

The backend performs the following core functions:

- Subscribe to LoRaWAN uplinks via **MQTT**
- Decode compact binary payloads
- Enrich data with metadata (timestamp, device ID)
- Update **real-time vehicle state**
- Execute **geofencing logic**
- Generate **events and alerts**
- Persist data to a cloud database
- Serve data to the dashboard in real time

---

## 3. High-Level Data Flow

```
ESP32 Vehicle Node
        ↓
LoRaWAN Gateway
        ↓
The Things Network (TTN)
        ↓
MQTT Broker
        ↓
Python Backend
        ↓
Firestore Database
        ↓
React Dashboard
```

Each layer is loosely coupled, enabling independent scaling and replacement.

---

## 4. MQTT Ingestion Layer

### Protocol Choice: MQTT

**Why MQTT:**
- Lightweight
- Event-driven
- Native support in TTN
- Ideal for intermittent IoT traffic

### Subscription Model

The backend subscribes to:

```
v3/{application_id}@ttn/devices/+/up
```

This allows handling data from **any number of devices** without per-device configuration.

---

## 5. Payload Decoding Layer

### Payload Characteristics
- Binary encoded
- Fixed-size
- No JSON (LoRaWAN payload-efficient)

### Decoding Strategy
- Fixed-point integers reconstructed into floating-point values
- Scaling factor: `1e6`

Example:

```
Bytes 0–3 → Latitude
Bytes 4–7 → Longitude
```

This decoding is handled in a dedicated module (`data_parser.py`), keeping logic clean and testable.

---

## 6. Geofencing Engine

### Design Choice: Backend-Based Geofencing

Geofencing is implemented **entirely in software** at the backend.

**Reasons:**
- Zones can be updated dynamically
- No firmware reflash required
- Supports complex polygonal zones
- Enables advanced analytics (dwell time)

### Algorithm
- Polygon-based geofencing
- Ray-casting point-in-polygon algorithm
- State-based detection (ENTRY / EXIT)

### Outputs
- Zone transitions
- Dwell time calculation
- Alert triggers for restricted zones

---

## 7. Database Layer

### Database Used: Firebase Firestore (Prototype)

Firestore is used for rapid prototyping and real-time UI updates.

#### Key Collections
- `vehicles` – Static vehicle metadata
- `live_state` – Real-time vehicle positions (map source)
- `events` – Historical geofence transitions
- `alerts` – Active and critical violations
- `zones` – Geofence definitions

### Data Separation Strategy
- **Overwrite** live state for fast reads
- **Append** events for history
- **Isolate** alerts for operator focus

This structure minimizes dashboard query complexity.

---

## 8. Real-Time Dashboard Integration

The dashboard uses **real-time listeners** on selected collections:

| Collection   | Purpose |
|--------------|--------|
| `live_state` | Live map rendering |
| `zones`      | Geofence visualization |
| `events`     | Timeline & history |
| `alerts`     | Critical notifications |

No polling is required; updates are pushed instantly.

---

## 9. Fault Tolerance & Reliability

The backend is designed to handle common IoT failure modes:

- **Packet loss** → tolerated via periodic updates
- **Out-of-order data** → timestamp-based processing
- **Temporary downtime** → Firebase ensures persistence
- **Gateway failure** → multiple gateways supported by TTN

The system prioritizes **eventual consistency** over strict real-time guarantees.

---

## 10. Scalability Considerations

- MQTT supports thousands of devices
- Backend is stateless (horizontal scaling possible)
- Firestore auto-scales reads and writes
- Geofencing logic is lightweight (O(n) per zone)

Future production upgrades:
- PostgreSQL + PostGIS
- TimescaleDB for time-series analytics
- Redis for alert state caching

---

## 11. Security Considerations

- TTN handles device authentication
- Firebase credentials stored securely (not committed)
- No inbound commands to devices (uplink-only design)
- Read-only dashboard access possible

This minimizes attack surface.

---

## 12. Limitations

- In-memory geofence state (prototype)
- Firebase vendor lock-in
- No guaranteed delivery (LoRaWAN constraint)

These are acceptable for prototype and pilot deployments.

---

## 13. Summary

The backend architecture provides a **robust, scalable, and low-complexity bridge** between LoRaWAN devices and a real-time dashboard. By separating real-time state, historical events, and alerts, the system remains responsive, maintainable, and extensible for future industrial deployments.
