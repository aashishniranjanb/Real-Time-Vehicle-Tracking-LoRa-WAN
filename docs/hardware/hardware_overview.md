# Hardware Overview

## Vehicle Node Components
- ESP32 microcontroller
- NEO-6M GPS module
- SX1276/SX1278 LoRa module
- 3.7V Li-Po battery

## Design Rationale
ESP32 provides sufficient processing power and low-power modes.
LoRa enables long-range communication with minimal energy usage.
GPS provides acceptable accuracy for industrial asset tracking.

## Power Considerations
GPS is the most power-hungry component and is duty-cycled to reduce energy consumption.
