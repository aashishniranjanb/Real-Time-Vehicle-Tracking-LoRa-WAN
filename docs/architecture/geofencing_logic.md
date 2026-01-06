# Geofencing Logic

```mermaid
flowchart TD
    GPS["📍 Incoming GPS Coordinate"]
    LOOKUP["🗺️ Zone Lookup"]
    PIP["📐 Point-in-Polygon Check"]
    STATE{"🔄 Zone State Comparison<br/><i>Previous Zone ≠ Current Zone?</i>"}
    EVENT["⚡ Event Generation"]
    
    subgraph OUTPUTS["📤 Outputs"]
        ALERT["🔔 Alert Notification"]
        LOG["💾 Database Log"]
        DASH["🖥️ Dashboard Update"]
    end

    GPS --> LOOKUP
    LOOKUP --> PIP
    PIP --> STATE
    
    STATE -->|"YES<br/>(Zone changed)"| EVENT
    STATE -->|"NO<br/>(Same zone)"| SKIP["Skip Event"]
    
    EVENT --> ALERT
    EVENT --> LOG
    EVENT --> DASH

    style GPS fill:#1565c0,stroke:#42a5f5,stroke-width:2px,color:#fff
    style LOOKUP fill:#2e7d32,stroke:#66bb6a,stroke-width:2px,color:#fff
    style PIP fill:#6a1b9a,stroke:#ab47bc,stroke-width:2px,color:#fff
    style STATE fill:#f57c00,stroke:#ffb74d,stroke-width:3px,color:#fff
    style EVENT fill:#c62828,stroke:#ef5350,stroke-width:2px,color:#fff
    style SKIP fill:#455a64,stroke:#78909c,stroke-width:1px,color:#fff
    style OUTPUTS fill:#263238,stroke:#37474f,stroke-width:2px,color:#fff
    style ALERT fill:#ff7043,stroke:#ffab91,stroke-width:2px,color:#fff
    style LOG fill:#5c6bc0,stroke:#9fa8da,stroke-width:2px,color:#fff
    style DASH fill:#26a69a,stroke:#80cbc4,stroke-width:2px,color:#fff
```

## Point-in-Polygon Algorithm

```mermaid
flowchart LR
    COORD["(lat, lon)"]
    POLY["Zone Polygon<br/>[(x1,y1), (x2,y2), ...]"]
    RAY["Ray Casting Algorithm"]
    RESULT{"Inside?"}
    
    COORD --> RAY
    POLY --> RAY
    RAY --> RESULT
    
    RESULT -->|"Odd crossings"| IN["✅ INSIDE"]
    RESULT -->|"Even crossings"| OUT["❌ OUTSIDE"]

    style RAY fill:#7b1fa2,stroke:#ce93d8,stroke-width:2px,color:#fff
    style IN fill:#2e7d32,stroke:#81c784,stroke-width:2px,color:#fff
    style OUT fill:#c62828,stroke:#ef5350,stroke-width:2px,color:#fff
```

## Zone State Machine

```mermaid
stateDiagram-v2
    [*] --> Unknown
    Unknown --> InsideZone: First fix inside
    Unknown --> OutsideZone: First fix outside
    
    InsideZone --> OutsideZone: EXIT Event 🚪
    OutsideZone --> InsideZone: ENTRY Event 🚗
    
    InsideZone --> InsideZone: Dwell timer++
    OutsideZone --> OutsideZone: No action

    note right of InsideZone
        Track dwell time
        for analytics
    end note
```

## Event Types

| Event | Trigger | Action |
|-------|---------|--------|
| **ENTRY** | Vehicle enters geofence | Log + Dashboard notification |
| **EXIT** | Vehicle leaves geofence | Log + Alert (if restricted) |
| **DWELL** | Vehicle stationary > threshold | Dwell-time analytics update |

> [!NOTE]
> Geofencing is implemented entirely in the **backend** (software layer), not on the embedded device. This allows flexible zone management without firmware updates.
