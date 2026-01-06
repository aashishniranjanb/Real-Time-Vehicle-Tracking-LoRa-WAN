# System Architecture Diagram
```mermaid
flowchart LR
    subgraph VN["🚗 Vehicle Node"]
        direction TB
        ESP32["ESP32"]
        GPS["├─ GPS (NEO-6M)"]
        EDGE["├─ Edge Filtering"]
        ADAPT["├─ Adaptive Tracking"]
        ENCODE["├─ Payload Encoder"]
        LORA["└─ LoRaWAN Stack (Class A)"]
    end

    subgraph GW["📡 LoRaWAN Gateway"]
        direction TB
        RF["LoRa RF Receiver"]
        BACKHAUL["Ethernet / Cellular Backhaul"]
    end

    subgraph TTN["☁️ Network Server (TTN)"]
        direction TB
        DEDUP["Packet Deduplication"]
        DECODE["Payload Decoder"]
        MQTT_FWD["MQTT Forwarder"]
    end

    subgraph BACKEND["⚙️ Application Backend"]
        direction TB
        MQTT_SUB["MQTT Subscriber"]
        PROCESSOR["Data Processor"]
        GEOFENCE["Geofence Engine"]
        ALERT["Alert Manager"]
        DB["Database"]
    end

    subgraph DASH["🖥️ Web Dashboard"]
        direction TB
        MAP["Live Map View"]
        STATUS["Vehicle Status"]
        GEO_VIZ["Geofence Visualization"]
        DWELL["Dwell-Time Analytics"]
    end

    VN -->|"LoRa RF (Uplink)"| GW
    GW -->|"UDP / Internet"| TTN
    TTN -->|"MQTT"| BACKEND
    BACKEND -->|"WebSocket / REST API"| DASH

    style VN fill:#1e3a5f,stroke:#4fc3f7,stroke-width:2px,color:#fff
    style GW fill:#2e4a3f,stroke:#81c784,stroke-width:2px,color:#fff
    style TTN fill:#3d2d5c,stroke:#ba68c8,stroke-width:2px,color:#fff
    style BACKEND fill:#4a3728,stroke:#ffb74d,stroke-width:2px,color:#fff
    style DASH fill:#1e3a3a,stroke:#4dd0e1,stroke-width:2px,color:#fff
```

## Key Design Notes

| Component | Purpose |
|-----------|---------|
| **Edge Filtering** | Reduces LoRa airtime by filtering redundant data at source |
| **Adaptive Tracking** | Dynamic sleep intervals based on movement state |
| **LoRaWAN Class A** | Uplink-focused, lowest power consumption |

> [!NOTE]
> 1–2 gateways can cover an entire campus (2-5 km range)
