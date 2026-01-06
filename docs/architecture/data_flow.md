# Data Flow & Firmware Logic

```mermaid
flowchart TD
    WAKE["🔋 Wake from Sleep"]
    GPS_ON["📍 GPS Power ON"]
    FIX["🛰️ Acquire GPS Fix"]
    MOVE{"🚗 Movement Check<br/>Location changed > 10m?"}
    ENCODE["📦 Encode Payload"]
    TX["📡 LoRaWAN Transmit"]
    INTERVAL["⏱️ Set Sleep Interval"]
    SLEEP["😴 Deep Sleep"]

    WAKE --> GPS_ON
    GPS_ON --> FIX
    FIX --> MOVE
    
    MOVE -->|"YES"| ENCODE
    MOVE -->|"NO<br/>(Skip transmission)"| SLEEP
    
    ENCODE --> TX
    TX --> INTERVAL
    INTERVAL --> SLEEP
    SLEEP -->|"Timer Wakeup"| WAKE

    %% Annotations
    INTERVAL -.- NOTE1["If Moving → 30-60s<br/>If Stationary → 5-10 min"]

    style WAKE fill:#1565c0,stroke:#42a5f5,stroke-width:2px,color:#fff
    style GPS_ON fill:#2e7d32,stroke:#66bb6a,stroke-width:2px,color:#fff
    style FIX fill:#2e7d32,stroke:#66bb6a,stroke-width:2px,color:#fff
    style MOVE fill:#f57c00,stroke:#ffb74d,stroke-width:3px,color:#fff
    style ENCODE fill:#6a1b9a,stroke:#ab47bc,stroke-width:2px,color:#fff
    style TX fill:#c62828,stroke:#ef5350,stroke-width:2px,color:#fff
    style INTERVAL fill:#00838f,stroke:#26c6da,stroke-width:2px,color:#fff
    style SLEEP fill:#37474f,stroke:#78909c,stroke-width:2px,color:#fff
    style NOTE1 fill:#fff3e0,stroke:#ffb74d,stroke-width:1px,color:#333
```

## Adaptive Tracking Logic (NOVELTY ⭐)

```mermaid
flowchart LR
    subgraph STATE["Movement State Machine"]
        MOVING["🚗 MOVING"]
        STATIONARY["🅿️ STATIONARY"]
    end
    
    MOVING -->|"No motion for 2 min"| STATIONARY
    STATIONARY -->|"Motion detected"| MOVING
    
    MOVING -.- FAST["Sleep: 30-60 sec"]
    STATIONARY -.- SLOW["Sleep: 5-10 min"]

    style MOVING fill:#4caf50,stroke:#81c784,stroke-width:2px,color:#fff
    style STATIONARY fill:#607d8b,stroke:#90a4ae,stroke-width:2px,color:#fff
    style FAST fill:#e8f5e9,stroke:#4caf50,color:#1b5e20
    style SLOW fill:#eceff1,stroke:#607d8b,color:#37474f
```

## Key Optimizations

| Technique | Benefit |
|-----------|---------|
| **Edge Filtering** | Skip transmission if location unchanged (saves airtime) |
| **Adaptive Sleep** | 10x longer sleep when stationary (saves battery) |
| **GPS Power Gating** | Only power GPS when needed |

> [!IMPORTANT]
> This adaptive logic is the **novelty** of the project — it differentiates from simple fixed-interval trackers
