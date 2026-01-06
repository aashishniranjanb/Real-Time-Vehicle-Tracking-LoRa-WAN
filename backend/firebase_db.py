"""
=========================================================
FIREBASE DATABASE LAYER
=========================================================

Clean abstraction for Firebase Firestore operations.
Matches the dashboard data model schema.

Collections:
    - vehicles:    Vehicle metadata (static)
    - live_state:  Real-time map data (overwritten)
    - events:      Geofence transitions (append-only)
    - alerts:      Active alerts (append)
    - zones:       Geofence polygon definitions

Author: LoRaWAN Vehicle Tracking Project
=========================================================
"""

import os
from datetime import datetime
from typing import Dict, List, Any, Optional

# Firebase imports (optional - graceful fallback if not installed)
try:
    import firebase_admin
    from firebase_admin import credentials, firestore
    FIREBASE_AVAILABLE = True
except ImportError:
    FIREBASE_AVAILABLE = False
    print("[WARN] firebase-admin not installed. Using mock mode.")


# =========================================================
# FIREBASE INITIALIZATION
# =========================================================

db = None

def init_firebase(credentials_path: str = "serviceAccountKey.json") -> bool:
    """
    Initialize Firebase connection.
    
    Args:
        credentials_path: Path to Firebase service account JSON file
        
    Returns:
        True if initialization successful, False otherwise
    """
    global db
    
    if not FIREBASE_AVAILABLE:
        print("[FIREBASE] Running in mock mode (firebase-admin not installed)")
        return False
    
    if not os.path.exists(credentials_path):
        print(f"[FIREBASE] Credentials file not found: {credentials_path}")
        print("[FIREBASE] Running in mock mode")
        return False
    
    try:
        cred = credentials.Certificate(credentials_path)
        firebase_admin.initialize_app(cred)
        db = firestore.client()
        print("[FIREBASE] Connected successfully")
        return True
    except Exception as e:
        print(f"[FIREBASE] Connection failed: {e}")
        return False


def is_connected() -> bool:
    """Check if Firebase is connected."""
    return db is not None


# =========================================================
# VEHICLE METADATA (Static)
# =========================================================

def create_vehicle(
    vehicle_id: str,
    name: str,
    vehicle_type: str,
    department: str,
    status: str = "ACTIVE"
) -> None:
    """
    Create or update vehicle metadata.
    
    Example:
        create_vehicle("VH001", "Test Vehicle 1", "Test Car", "QA")
    """
    data = {
        "vehicle_id": vehicle_id,
        "name": name,
        "type": vehicle_type,
        "department": department,
        "status": status,
        "created_at": datetime.utcnow().isoformat()
    }
    
    if db:
        db.collection("vehicles").document(vehicle_id).set(data)
    
    print(f"[DB] Vehicle created: {vehicle_id}")


def get_vehicle(vehicle_id: str) -> Optional[Dict[str, Any]]:
    """Get vehicle metadata by ID."""
    if db:
        doc = db.collection("vehicles").document(vehicle_id).get()
        return doc.to_dict() if doc.exists else None
    return None


def get_all_vehicles() -> List[Dict[str, Any]]:
    """Get all vehicles."""
    if db:
        docs = db.collection("vehicles").stream()
        return [doc.to_dict() for doc in docs]
    return []


# =========================================================
# LIVE STATE (Real-time Map Source)
# =========================================================

def update_live_state(
    vehicle_id: str,
    lat: float,
    lon: float,
    zone: Optional[str] = None,
    moving: bool = True,
    speed_kmph: Optional[float] = None,
    heading_deg: Optional[float] = None
) -> None:
    """
    Update real-time vehicle state for dashboard map.
    This overwrites the previous state (not append).
    
    Example:
        update_live_state("VH001", 12.9718, 80.2202, "Assembly_Area", True, 14.5)
    """
    data = {
        "vehicle_id": vehicle_id,
        "latitude": lat,
        "longitude": lon,
        "zone": zone,
        "moving": moving,
        "speed_kmph": speed_kmph,
        "heading_deg": heading_deg,
        "last_update": datetime.utcnow().isoformat()
    }
    
    if db:
        db.collection("live_state").document(vehicle_id).set(data)
    
    print(f"[DB] Live state updated: {vehicle_id} at ({lat:.6f}, {lon:.6f})")


def get_live_state(vehicle_id: str) -> Optional[Dict[str, Any]]:
    """Get current live state for a vehicle."""
    if db:
        doc = db.collection("live_state").document(vehicle_id).get()
        return doc.to_dict() if doc.exists else None
    return None


def get_all_live_states() -> List[Dict[str, Any]]:
    """Get live states for all vehicles (for map rendering)."""
    if db:
        docs = db.collection("live_state").stream()
        return [doc.to_dict() for doc in docs]
    return []


# =========================================================
# EVENTS (Geofence Transitions - Append Only)
# =========================================================

def store_event(
    vehicle_id: str,
    event_type: str,
    zone: str,
    dwell_time_sec: Optional[float] = None
) -> str:
    """
    Store a geofence event (ENTRY, EXIT, DWELL_ALERT).
    
    Args:
        vehicle_id: Vehicle identifier
        event_type: "ENTRY", "EXIT", or "DWELL_ALERT"
        zone: Zone name
        dwell_time_sec: Time spent in zone (for EXIT events)
        
    Returns:
        Document ID of created event
    """
    data = {
        "vehicle_id": vehicle_id,
        "event_type": event_type,
        "zone": zone,
        "dwell_time_sec": dwell_time_sec,
        "timestamp": datetime.utcnow().isoformat()
    }
    
    doc_id = None
    if db:
        doc_ref = db.collection("events").add(data)
        doc_id = doc_ref[1].id
    
    print(f"[DB] Event stored: {vehicle_id} {event_type} {zone}")
    return doc_id or "mock_event_id"


def get_recent_events(limit: int = 50) -> List[Dict[str, Any]]:
    """Get most recent events."""
    if db:
        docs = db.collection("events")\
            .order_by("timestamp", direction=firestore.Query.DESCENDING)\
            .limit(limit)\
            .stream()
        return [doc.to_dict() for doc in docs]
    return []


def get_vehicle_events(vehicle_id: str, limit: int = 20) -> List[Dict[str, Any]]:
    """Get events for a specific vehicle."""
    if db:
        docs = db.collection("events")\
            .where("vehicle_id", "==", vehicle_id)\
            .order_by("timestamp", direction=firestore.Query.DESCENDING)\
            .limit(limit)\
            .stream()
        return [doc.to_dict() for doc in docs]
    return []


# =========================================================
# ALERTS (Critical Events)
# =========================================================

def create_alert(
    vehicle_id: str,
    alert_type: str,
    zone: str,
    severity: str = "HIGH",
    message: Optional[str] = None
) -> str:
    """
    Create an alert for critical events.
    
    Alert types:
        - RESTRICTED_ZONE_ENTRY
        - DWELL_EXCEEDED
        - CAMPUS_EXIT
        - SPEED_VIOLATION
    """
    data = {
        "vehicle_id": vehicle_id,
        "alert_type": alert_type,
        "zone": zone,
        "severity": severity,
        "message": message or f"{vehicle_id} triggered {alert_type} in {zone}",
        "acknowledged": False,
        "timestamp": datetime.utcnow().isoformat()
    }
    
    doc_id = None
    if db:
        doc_ref = db.collection("alerts").add(data)
        doc_id = doc_ref[1].id
    
    print(f"[ALERT] ⚠️  {vehicle_id}: {alert_type} in {zone}")
    return doc_id or "mock_alert_id"


def get_active_alerts() -> List[Dict[str, Any]]:
    """Get unacknowledged alerts."""
    if db:
        docs = db.collection("alerts")\
            .where("acknowledged", "==", False)\
            .order_by("timestamp", direction=firestore.Query.DESCENDING)\
            .stream()
        return [doc.to_dict() for doc in docs]
    return []


def acknowledge_alert(alert_id: str) -> None:
    """Mark an alert as acknowledged."""
    if db:
        db.collection("alerts").document(alert_id).update({
            "acknowledged": True,
            "acknowledged_at": datetime.utcnow().isoformat()
        })


# =========================================================
# ZONES (Geofence Definitions)
# =========================================================

def create_zone(
    zone_name: str,
    zone_type: str,
    color: str,
    polygon_points: List[Dict[str, float]]
) -> None:
    """
    Create or update a geofence zone.
    
    Args:
        zone_name: Unique zone identifier
        zone_type: "PRODUCTION", "PARKING", "RESTRICTED", etc.
        color: Hex color for map rendering
        polygon_points: List of {lat, lon} points
        
    Example:
        create_zone("Assembly_Area", "PRODUCTION", "#2ECC71", [
            {"lat": 12.9718, "lon": 80.2201},
            {"lat": 12.9725, "lon": 80.2201},
            {"lat": 12.9725, "lon": 80.2210},
            {"lat": 12.9718, "lon": 80.2210}
        ])
    """
    data = {
        "zone_name": zone_name,
        "zone_type": zone_type,
        "color": color,
        "polygon": polygon_points,
        "created_at": datetime.utcnow().isoformat()
    }
    
    if db:
        db.collection("zones").document(zone_name).set(data)
    
    print(f"[DB] Zone created: {zone_name}")


def get_all_zones() -> List[Dict[str, Any]]:
    """Get all geofence zones (for map rendering)."""
    if db:
        docs = db.collection("zones").stream()
        return [doc.to_dict() for doc in docs]
    return []


def get_zone(zone_name: str) -> Optional[Dict[str, Any]]:
    """Get a specific zone by name."""
    if db:
        doc = db.collection("zones").document(zone_name).get()
        return doc.to_dict() if doc.exists else None
    return None


# =========================================================
# LOCATION HISTORY (Optional - for trails/playback)
# =========================================================

def store_location_history(
    vehicle_id: str,
    lat: float,
    lon: float,
    speed_kmph: Optional[float] = None
) -> None:
    """
    Store location for historical trails.
    Use sparingly to control storage costs.
    """
    data = {
        "vehicle_id": vehicle_id,
        "latitude": lat,
        "longitude": lon,
        "speed_kmph": speed_kmph,
        "timestamp": datetime.utcnow().isoformat()
    }
    
    if db:
        db.collection("vehicles").document(vehicle_id)\
            .collection("history").add(data)


# =========================================================
# INITIALIZATION CHECK
# =========================================================

if __name__ == "__main__":
    print("=== Firebase Database Module ===")
    print(f"Firebase available: {FIREBASE_AVAILABLE}")
    
    # Try to connect
    connected = init_firebase()
    print(f"Connected: {connected}")
