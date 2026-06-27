# PCB Layout Files

This directory is designated for the PCB fabrication and design source files for the ESP32 tracking node.

## Design Software
- **KiCad EDA v7.0** (Open-source PCB design suite)

## Directory Structure
- `sources/`: Contains the KiCad schematic (`.kicad_sch`) and board layout (`.kicad_pcb`) files.
- `gerbers/`: Standard RS-274X Gerber files for manufacturing:
  - Copper layers (F.Cu, B.Cu)
  - Solder mask (F.Mask, B.Mask)
  - Silkscreen (F.SilkS, B.SilkS)
  - Drill file (`.drl`)
  - Board outline (`.Edge_Cuts`)

## Electrical Rules & Layout Guidelines
1. **Trace Widths**: 
   - Power traces (VCC, GND): 0.5 mm (20 mil) minimum to handle up to 500mA peak during LoRa transmissions.
   - Signal traces (SPI, UART): 0.2 mm (8 mil) for density.
2. **RF Layout**:
   - The RF trace connecting the SX1278 pin to the SMA antenna connector is designed as a **coplanar waveguide** with **50-Ohm impedance matching**.
   - Keep the RF trace as short as possible with no 90-degree bends (use 45-degree angles or curves).
   - Ensure a solid ground plane underneath the RF trace.
3. **Decoupling**:
   - Place 10uF and 100nF ceramic decoupling capacitors directly adjacent to the VCC pins of the ESP32, GPS, and LoRa modules.
4. **GPS Ground plane isolation**:
   - Avoid routing high-speed digital signals (e.g., SPI lines) directly underneath the GPS antenna trace to minimize electromagnetic interference (EMI).
