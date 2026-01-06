"""
=========================================================
MQTT HANDLER - TTN Integration
=========================================================

Subscribes to The Things Network (TTN) MQTT broker and
processes incoming LoRaWAN uplinks in real-time.

Flow:
    TTN → MQTT → Decode Payload → Geofencing → Database

Author: LoRaWAN Vehicle Tracking Project
=========================================================
"""

import json
import base64
import os
from datetime import datetime
from typing import Dict, Any, Optional

import paho.mqtt.client as mqtt
from dotenv import load_dotenv

from data_parser import decode_payload, create_location_record
from geofencing import process_location

# Load environment variables
load_dotenv()


# =========================================================
# CONFIGURATION
# =========================================================

# TTN MQTT Settings (v3 API)
MQTT_BROKER = os.getenv("TTN_MQTT_BROKER", "eu1.cloud.thethings.network")
MQTT_PORT = int(os.getenv("TTN_MQTT_PORT", "1883"))

# TTN Application Credentials
# Set these in .env file or environment variables
APP_ID = os.getenv("TTN_APP_ID", "your-ttn-app-id")
API_KEY = os.getenv("TTN_API_KEY", "your-ttn-api-key")

# MQTT Topic for uplinks
# Format: v3/{app_id}@ttn/devices/{device_id}/up
MQTT_TOPIC = f"v3/{APP_ID}@ttn/devices/+/up"


# =========================================================
# DATA STORAGE (In-memory for prototype)
# =========================================================

# Store recent locations (replace with database in production)
location_history: Dict[str, list] = {}

# Store events
event_log: list = []


def store_location(record: Dict[str, Any]) -> None:
    """Store location record in memory."""
    device_id = record["device_id"]
    
    if device_id not in location_history:
        location_history[device_id] = []
    
    location_history[device_id].append(record)
    
    # Keep only last 100 records per device
    if len(location_history[device_id]) > 100:
        location_history[device_id] = location_history[device_id][-100:]


def store_event(event: Dict[str, Any]) -> None:
    """Store geofencing event."""
    event_log.append(event)
    
    # Keep only last 500 events
    if len(event_log) > 500:
        event_log.pop(0)


# =========================================================
# MQTT CALLBACKS
# =========================================================

def on_connect(client: mqtt.Client, userdata: Any, flags: Dict, rc: int) -> None:
    """Called when connected to MQTT broker."""
    
    if rc == 0:
        print(f"[MQTT] Connected to {MQTT_BROKER}")
        print(f"[MQTT] Subscribing to: {MQTT_TOPIC}")
        client.subscribe(MQTT_TOPIC)
    else:
        error_messages = {
            1: "Incorrect protocol version",
            2: "Invalid client identifier",
            3: "Server unavailable",
            4: "Bad username or password",
            5: "Not authorized"
        }
        print(f"[MQTT] Connection failed: {error_messages.get(rc, f'Unknown error {rc}')}")


def on_disconnect(client: mqtt.Client, userdata: Any, rc: int) -> None:
    """Called when disconnected from MQTT broker."""
    print(f"[MQTT] Disconnected (rc={rc})")
    if rc != 0:
        print("[MQTT] Unexpected disconnect, will attempt reconnect...")


def on_subscribe(client: mqtt.Client, userdata: Any, mid: int, granted_qos: tuple) -> None:
    """Called when subscription is confirmed."""
    print(f"[MQTT] Subscribed successfully (QoS: {granted_qos})")


def on_message(client: mqtt.Client, userdata: Any, msg: mqtt.MQTTMessage) -> None:
    """
    Called when a message is received from TTN.
    This is the main data processing entry point.
    """
    
    try:
        # Parse JSON payload from TTN
        payload = json.loads(msg.payload.decode('utf-8'))
        
        # Extract device information
        end_device_ids = payload.get("end_device_ids", {})
        device_id = end_device_ids.get("device_id", "unknown")
        
        # Extract uplink message
        uplink = payload.get("uplink_message", {})
        
        # Get raw payload (base64 encoded)
        frm_payload = uplink.get("frm_payload")
        if not frm_payload:
            print(f"[MQTT] No payload from {device_id}")
            return
        
        # Decode base64 payload
        raw_payload = base64.b64decode(frm_payload)
        
        # Get RF metadata
        rx_metadata = uplink.get("rx_metadata", [{}])[0]
        rssi = rx_metadata.get("rssi")
        snr = rx_metadata.get("snr")
        
        # Get timestamp
        received_at = uplink.get("received_at")
        
        # ---------------------------------------------------------
        # STEP 1: Decode binary payload
        # ---------------------------------------------------------
        decoded = decode_payload(raw_payload)
        
        print(f"\n[DATA] Received from {device_id}:")
        print(f"       Latitude:  {decoded['latitude']:.6f}")
        print(f"       Longitude: {decoded['longitude']:.6f}")
        print(f"       RSSI: {rssi} dBm, SNR: {snr} dB")
        
        # ---------------------------------------------------------
        # STEP 2: Create location record
        # ---------------------------------------------------------
        record = create_location_record(
            device_id=device_id,
            payload_bytes=raw_payload,
            received_at=received_at,
            rssi=rssi,
            snr=snr
        )
        
        # Store location
        store_location(record)
        
        # ---------------------------------------------------------
        # STEP 3: Process geofencing
        # ---------------------------------------------------------
        event = process_location(
            device_id=device_id,
            lat=decoded['latitude'],
            lon=decoded['longitude']
        )
        
        if event:
            print(f"[GEO]  Event: {event['type']} - Zone: {event.get('zone', 'N/A')}")
            
            # Store event
            store_event(event)
            
            # Check for alerts
            if event.get('alert'):
                handle_alert(event)
        
        # ---------------------------------------------------------
        # STEP 4: Additional processing (expand as needed)
        # ---------------------------------------------------------
        # TODO: Push to dashboard via WebSocket
        # TODO: Store in Firebase/PostgreSQL
        # TODO: Send notifications
        
    except json.JSONDecodeError as e:
        print(f"[ERROR] Invalid JSON: {e}")
    except ValueError as e:
        print(f"[ERROR] Payload decode error: {e}")
    except Exception as e:
        print(f"[ERROR] Unexpected error: {e}")


def handle_alert(event: Dict[str, Any]) -> None:
    """
    Handle alert events (notifications, logging, etc.)
    """
    alert_message = event.get('alert_message', 'Unknown alert')
    print(f"[ALERT] ⚠️  {alert_message}")
    
    # TODO: Implement actual alert delivery
    # - Send email
    # - Push notification
    # - SMS via Twilio
    # - Slack/Teams webhook


# =========================================================
# MAIN MQTT CLIENT
# =========================================================

def create_mqtt_client() -> mqtt.Client:
    """Create and configure MQTT client."""
    
    client = mqtt.Client(client_id=f"lorawan-vehicle-tracker-{os.getpid()}")
    
    # Set TTN credentials
    client.username_pw_set(APP_ID, API_KEY)
    
    # Attach callbacks
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_subscribe = on_subscribe
    client.on_message = on_message
    
    return client


def run_mqtt_loop() -> None:
    """Start the MQTT client and run forever."""
    
    print("="*60)
    print(" LORAWAN VEHICLE TRACKING - BACKEND SERVICE")
    print("="*60)
    print(f" MQTT Broker: {MQTT_BROKER}:{MQTT_PORT}")
    print(f" TTN App ID:  {APP_ID}")
    print(f" Topic:       {MQTT_TOPIC}")
    print("="*60)
    print()
    
    # Validate configuration
    if APP_ID == "your-ttn-app-id":
        print("[WARN] ⚠️  TTN credentials not configured!")
        print("       Set TTN_APP_ID and TTN_API_KEY in .env file")
        print()
    
    # Create client
    client = create_mqtt_client()
    
    try:
        # Connect to broker
        print(f"[MQTT] Connecting to {MQTT_BROKER}...")
        client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
        
        # Start network loop (blocking)
        print("[MQTT] Starting message loop (Ctrl+C to stop)...")
        client.loop_forever()
        
    except KeyboardInterrupt:
        print("\n[MQTT] Shutting down...")
        client.disconnect()
        
    except Exception as e:
        print(f"[ERROR] MQTT connection failed: {e}")


# =========================================================
# API FUNCTIONS (for dashboard integration)
# =========================================================

def get_latest_location(device_id: str) -> Optional[Dict[str, Any]]:
    """Get the most recent location for a device."""
    if device_id in location_history and location_history[device_id]:
        return location_history[device_id][-1]
    return None


def get_location_history(device_id: str, limit: int = 50) -> list:
    """Get location history for a device."""
    if device_id in location_history:
        return location_history[device_id][-limit:]
    return []


def get_all_devices() -> list:
    """Get list of all tracked devices."""
    return list(location_history.keys())


def get_recent_events(limit: int = 50) -> list:
    """Get recent geofencing events."""
    return event_log[-limit:]


# =========================================================
# ENTRY POINT
# =========================================================

if __name__ == "__main__":
    run_mqtt_loop()
