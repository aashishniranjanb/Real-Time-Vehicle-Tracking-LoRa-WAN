"""
=========================================================
SYNTHETIC DATA GENERATOR
=========================================================

Generates realistic test data for 5 vehicles.
Use for dashboard testing, screenshots, and demos.

Creates:
    - 3 geofence zones
    - 5 vehicles with metadata
    - Live state for each vehicle
    - Sample entry/exit events
    - 1 restricted zone alert

Author: LoRaWAN Vehicle Tracking Project
=========================================================
"""

import sys
import time
from datetime import datetime, timedelta
import random

from firebase_db import (
    init_firebase,
    create_vehicle,
    update_live_state,
    store_event,
    create_alert,
    create_zone
)


def generate_synthetic_data():
    """Generate all synthetic data for demo purposes."""
    
    print("="*60)
    print(" SYNTHETIC DATA GENERATOR")
    print(" LoRaWAN Vehicle Tracking System")
    print("="*60)
    print()
    
    # -------------------------------------------------
    # STEP 1: Initialize Firebase (optional)
    # -------------------------------------------------
    print("[1/5] Connecting to Firebase...")
    connected = init_firebase()
    
    if not connected:
        print("      Running in mock mode (data printed but not saved)")
    print()
    
    # -------------------------------------------------
    # STEP 2: Create Geofence Zones
    # -------------------------------------------------
    print("[2/5] Creating geofence zones...")
    
    zones = [
        {
            "name": "Assembly_Area",
            "type": "PRODUCTION",
            "color": "#2ECC71",  # Green
            "polygon": [
                {"lat": 12.9718, "lon": 80.2201},
                {"lat": 12.9725, "lon": 80.2201},
                {"lat": 12.9725, "lon": 80.2210},
                {"lat": 12.9718, "lon": 80.2210}
            ]
        },
        {
            "name": "Parking_Lot",
            "type": "PARKING",
            "color": "#3498DB",  # Blue
            "polygon": [
                {"lat": 12.9705, "lon": 80.2185},
                {"lat": 12.9712, "lon": 80.2185},
                {"lat": 12.9712, "lon": 80.2193},
                {"lat": 12.9705, "lon": 80.2193}
            ]
        },
        {
            "name": "Restricted_Zone",
            "type": "RESTRICTED",
            "color": "#E74C3C",  # Red
            "polygon": [
                {"lat": 12.9720, "lon": 80.2198},
                {"lat": 12.9724, "lon": 80.2198},
                {"lat": 12.9724, "lon": 80.2203},
                {"lat": 12.9720, "lon": 80.2203}
            ]
        },
        {
            "name": "Loading_Bay",
            "type": "LOGISTICS",
            "color": "#F39C12",  # Orange
            "polygon": [
                {"lat": 12.9708, "lon": 80.2195},
                {"lat": 12.9714, "lon": 80.2195},
                {"lat": 12.9714, "lon": 80.2200},
                {"lat": 12.9708, "lon": 80.2200}
            ]
        },
        {
            "name": "Testing_Track",
            "type": "TESTING",
            "color": "#9B59B6",  # Purple
            "polygon": [
                {"lat": 12.9726, "lon": 80.2205},
                {"lat": 12.9735, "lon": 80.2205},
                {"lat": 12.9735, "lon": 80.2218},
                {"lat": 12.9726, "lon": 80.2218}
            ]
        }
    ]
    
    for z in zones:
        create_zone(z["name"], z["type"], z["color"], z["polygon"])
    
    print(f"      Created {len(zones)} zones")
    print()
    
    # -------------------------------------------------
    # STEP 3: Create Vehicle Metadata
    # -------------------------------------------------
    print("[3/5] Creating vehicle metadata...")
    
    vehicles = [
        ("VH001", "Test Vehicle Alpha", "Test Car", "QA Testing"),
        ("VH002", "Material Carrier B2", "Forklift", "Logistics"),
        ("VH003", "Track Runner 03", "Test Car", "Performance Testing"),
        ("VH004", "Loading Truck L4", "Transport", "Dispatch"),
        ("VH005", "Security Patrol SP1", "Utility Vehicle", "Security")
    ]
    
    for vid, name, vtype, dept in vehicles:
        create_vehicle(vid, name, vtype, dept)
    
    print(f"      Created {len(vehicles)} vehicles")
    print()
    
    # -------------------------------------------------
    # STEP 4: Set Live States
    # -------------------------------------------------
    print("[4/5] Setting live vehicle states...")
    
    live_states = [
        # (vehicle_id, lat, lon, zone, moving, speed)
        ("VH001", 12.9719, 80.2204, "Assembly_Area", True, 14.2),
        ("VH002", 12.9709, 80.2190, "Parking_Lot", False, 0.0),
        ("VH003", 12.9730, 80.2212, "Testing_Track", True, 28.5),
        ("VH004", 12.9711, 80.2197, "Loading_Bay", False, 0.0),
        ("VH005", 12.9722, 80.2200, "Restricted_Zone", True, 8.7)
    ]
    
    for vid, lat, lon, zone, moving, speed in live_states:
        update_live_state(
            vehicle_id=vid,
            lat=lat,
            lon=lon,
            zone=zone,
            moving=moving,
            speed_kmph=speed,
            heading_deg=random.randint(0, 359)
        )
    
    print(f"      Updated {len(live_states)} live states")
    print()
    
    # -------------------------------------------------
    # STEP 5: Generate Events & Alerts
    # -------------------------------------------------
    print("[5/5] Generating events and alerts...")
    
    # Entry events
    store_event("VH001", "ENTRY", "Assembly_Area")
    store_event("VH003", "ENTRY", "Testing_Track")
    store_event("VH005", "ENTRY", "Restricted_Zone")
    
    # Exit events with dwell time
    store_event("VH002", "EXIT", "Loading_Bay", dwell_time_sec=1800)
    store_event("VH004", "EXIT", "Parking_Lot", dwell_time_sec=7200)
    
    # Restricted zone alert
    create_alert(
        vehicle_id="VH005",
        alert_type="RESTRICTED_ZONE_ENTRY",
        zone="Restricted_Zone",
        severity="HIGH",
        message="Security Patrol SP1 entered Restricted Zone"
    )
    
    print("      Created 5 events and 1 alert")
    print()
    
    # -------------------------------------------------
    # SUMMARY
    # -------------------------------------------------
    print("="*60)
    print(" ✅ SYNTHETIC DATA GENERATION COMPLETE")
    print("="*60)
    print()
    print(" Created:")
    print(f"   • {len(zones)} geofence zones")
    print(f"   • {len(vehicles)} vehicles")
    print(f"   • {len(live_states)} live states")
    print("   • 5 events (3 ENTRY, 2 EXIT)")
    print("   • 1 restricted zone alert")
    print()
    print(" Dashboard is ready for testing!")
    print("="*60)


def simulate_vehicle_movement(duration_sec: int = 60):
    """
    Simulate real-time vehicle movement for demo.
    Updates live_state every few seconds.
    """
    print(f"\n[SIMULATION] Starting {duration_sec}s vehicle movement simulation...")
    print("             Press Ctrl+C to stop\n")
    
    init_firebase()
    
    # Base positions
    positions = {
        "VH001": {"lat": 12.9719, "lon": 80.2204, "zone": "Assembly_Area"},
        "VH003": {"lat": 12.9730, "lon": 80.2212, "zone": "Testing_Track"},
    }
    
    start_time = time.time()
    
    try:
        while time.time() - start_time < duration_sec:
            for vid, pos in positions.items():
                # Add small random movement
                pos["lat"] += random.uniform(-0.0001, 0.0001)
                pos["lon"] += random.uniform(-0.0001, 0.0001)
                
                update_live_state(
                    vehicle_id=vid,
                    lat=pos["lat"],
                    lon=pos["lon"],
                    zone=pos["zone"],
                    moving=True,
                    speed_kmph=random.uniform(10, 25)
                )
            
            time.sleep(3)
            
    except KeyboardInterrupt:
        print("\n[SIMULATION] Stopped by user")


# =========================================================
# ENTRY POINT
# =========================================================

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "--simulate":
        duration = int(sys.argv[2]) if len(sys.argv) > 2 else 60
        simulate_vehicle_movement(duration)
    else:
        generate_synthetic_data()
