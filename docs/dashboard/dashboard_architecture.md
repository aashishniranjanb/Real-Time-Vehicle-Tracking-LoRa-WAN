# Dashboard Architecture
Real-Time Fleet Visualization & Geofencing UI

---

## 1. Overview

The dashboard is a **map-centric, real-time monitoring interface** designed for industrial vehicle tracking across large campuses.

Its primary goal is to provide:
- Immediate situational awareness
- Clear visualization of vehicle movement
- Zone-based operational insights
- Minimal operator cognitive load

The dashboard consumes **real-time data** from the backend and remains **read-only**, ensuring safety and simplicity.

---

## 2. Design Principles

The dashboard is designed around the following principles:

- **Map-first UI**: Spatial context is primary
- **Real-time updates**: No manual refresh or polling
- **Visual clarity**: Minimal clutter, clear color semantics
- **Smooth motion**: Avoid jumpy GPS updates
- **Separation of concerns**: UI logic isolated from backend logic

---

## 3. High-Level Architecture

```
Firestore Database
        ↓ (Real-time listeners)
React Dashboard
        ↓
Map Rendering Engine (Leaflet)
        ↓
User Interface Components
```

The dashboard directly subscribes to selected Firestore collections using real-time listeners.

---

## 4. Core UI Components

The dashboard is composed of the following major components:

```
Dashboard
├── Header
├── VehicleList
├── MapView
│   ├── VehicleMarker
│   ├── RouteTrail
│   └── ZonePolygon
└── EventTimeline
```

Each component has a single, well-defined responsibility.

---

## 5. Data Sources & Responsibilities

| Firestore Collection | Consumed By | Purpose |
|----------------------|-------------|---------|
| `live_state` | MapView, VehicleList | Real-time vehicle positions |
| `zones` | MapView | Geofence visualization |
| `events` | EventTimeline | Entry/Exit history |
| `alerts` | EventTimeline | Critical violations |
| `vehicles` | VehicleList | Metadata display |

The dashboard does **not** perform any data mutation.

---

## 6. Map Rendering Engine

### Technology Choice: Leaflet + OpenStreetMap

**Why Leaflet:**
- Lightweight
- Open-source
- No API key required
- Suitable for industrial dashboards

### Map Features
- Real-time vehicle markers
- Polygonal geofences
- Route trails
- Smooth animated motion
- Interactive popups and tooltips

---

## 7. Vehicle Visualization

### Vehicle Markers
- Represented using car/truck icons
- Rotated based on heading (if available)
- Color-coded by state:
  - 🟢 Moving
  - 🔵 Idle
  - 🔴 Restricted Zone

### Smooth Motion
Vehicle positions are interpolated on the frontend to ensure smooth animation between discrete GPS updates.

**Reason:**
LoRaWAN updates are periodic (seconds), not continuous.

---

## 8. Route Trail Visualization

To provide spatial context without clutter:

- A sliding window of the **last N GPS points** is maintained per vehicle
- Trails are rendered as semi-transparent polylines
- Older points are discarded automatically

This allows operators to understand recent movement patterns at a glance.

---

## 9. Geofence Visualization

### Zone Representation
- Zones are rendered as **polygon overlays**
- Each zone has:
  - Name
  - Color
  - Type (Production, Parking, Restricted)

### Interaction
- Hover tooltip shows zone name
- Restricted zones use high-contrast colors
- Entry triggers visual alert in timeline

---

## 10. Event & Alert Timeline

The event panel displays:

- ENTRY / EXIT events
- Dwell time (for EXIT events)
- Alert severity indicators

Events are ordered by timestamp and automatically updated in real time.

---

## 11. Styling & UX

### Theme
- Dark industrial theme
- Low-saturation background
- Color reserved for state changes and alerts

### UX Goals
- Suitable for long monitoring sessions
- Readable under factory lighting
- Minimal animation fatigue

---

## 12. Performance Considerations

- Only active vehicles are rendered
- Route trail length is capped (e.g., 15 points)
- Firebase listeners are scoped to required collections
- No heavy computation on UI thread

This allows the dashboard to scale to dozens of vehicles smoothly.

---

## 13. Security Considerations

- Dashboard is read-only
- No device control exposed
- Firebase rules can enforce:
  - Authenticated read access
  - Restricted write access

This minimizes operational risk.

---

## 14. Limitations

- Depends on GPS availability
- Frontend interpolation assumes linear motion
- No offline caching (can be added later)

These are acceptable for prototype and pilot deployments.

---

## 15. Summary

The dashboard architecture provides a **clear, responsive, and operator-friendly interface** for real-time vehicle tracking and geofencing. By combining real-time data streams, smooth animation, and spatial visualization, the dashboard turns low-bandwidth IoT telemetry into actionable operational insight.
