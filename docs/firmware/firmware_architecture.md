# Firmware Architecture

## Overview
The firmware is event-driven and optimized for low-power operation.

## Main Functional Blocks
- GPS Manager
- LoRaWAN Manager
- Power Manager
- Adaptive Tracking Logic

## Core Flow
1. Wake from sleep
2. Acquire GPS fix
3. Filter insignificant movement
4. Encode payload
5. Transmit via LoRaWAN
6. Enter deep sleep

## Adaptive Tracking
- Moving: Short update interval
- Stationary: Long sleep interval
