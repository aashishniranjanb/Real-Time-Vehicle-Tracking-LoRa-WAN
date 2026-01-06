"""
=========================================================
GEOFENCING ENGINE - Zone Detection & Events
=========================================================

Implements polygon-based geofencing with:
- Point-in-polygon detection (ray casting algorithm)
- Entry / Exit event generation
- Dwell time tracking

Author: LoRaWAN Vehicle Tracking Project
=========================================================
"""

from datetime import datetime
from typing import Dict, List, Tuple, Optional, Any


# =========================================================
# GEOFENCE ZONE DEFINITIONS
# =========================================================

# Define zones as polygons (list of lat, lon vertices)
# These can be loaded from database in production

GEOFENCES: Dict[str, List[Tuple[float, float]]] = {
    "Main_Campus": [
        (12.9700, 80.2180),
        (12.9750, 80.2180),
        (12.9750, 80.2230),
        (12.9700, 80.2230)
    ],
    "Assembly_Area": [
        (12.9718, 80.2201),
        (12.9725, 80.2201),
        (12.9725, 80.2210),
        (12.9718, 80.2210)
    ],
    "Parking_Lot": [
        (12.9705, 80.2185),
        (12.9712, 80.2185),
        (12.9712, 80.2193),
        (12.9705, 80.2193)
    ],
    "Restricted_Zone": [
        (12.9730, 80.2215),
        (12.9740, 80.2215),
        (12.9740, 80.2225),
        (12.9730, 80.2225)
    ]
}

# Zone metadata (for alerts and rules)
ZONE_CONFIG = {
    "Main_Campus": {"alert_on_exit": True, "max_dwell_minutes": None},
    "Assembly_Area": {"alert_on_exit": False, "max_dwell_minutes": 60},
    "Parking_Lot": {"alert_on_exit": False, "max_dwell_minutes": 480},
    "Restricted_Zone": {"alert_on_exit": False, "max_dwell_minutes": 5}
}


# =========================================================
# POINT-IN-POLYGON ALGORITHM
# =========================================================

def point_in_polygon(lat: float, lon: float, polygon: List[Tuple[float, float]]) -> bool:
    """
    Determine if a point is inside a polygon using ray casting algorithm.
    
    Algorithm: Cast a ray from the point to infinity (rightward) and count
    how many polygon edges it crosses. Odd = inside, Even = outside.
    
    Args:
        lat: Latitude of point to test
        lon: Longitude of point to test
        polygon: List of (lat, lon) vertices defining the polygon
        
    Returns:
        True if point is inside polygon, False otherwise
        
    Reference:
        https://en.wikipedia.org/wiki/Point_in_polygon#Ray_casting_algorithm
    """
    
    inside = False
    n = len(polygon)
    
    for i in range(n):
        lat1, lon1 = polygon[i]
        lat2, lon2 = polygon[(i + 1) % n]
        
        # Check if ray from point crosses this edge
        if ((lon1 > lon) != (lon2 > lon)):
            # Calculate x-coordinate of intersection
            intersect_lat = (lat2 - lat1) * (lon - lon1) / (lon2 - lon1) + lat1
            
            if lat < intersect_lat:
                inside = not inside
    
    return inside


# =========================================================
# ZONE DETECTION
# =========================================================

def detect_zone(lat: float, lon: float) -> Optional[str]:
    """
    Determine which geofence zone contains the given point.
    
    Args:
        lat: Latitude
        lon: Longitude
        
    Returns:
        Zone name if inside a geofence, None if outside all zones
        
    Note:
        If point is inside multiple overlapping zones, returns first match.
    """
    
    for zone_name, polygon in GEOFENCES.items():
        if point_in_polygon(lat, lon, polygon):
            return zone_name
    
    return None


def detect_all_zones(lat: float, lon: float) -> List[str]:
    """
    Find all zones containing the given point (for overlapping zones).
    
    Args:
        lat: Latitude
        lon: Longitude
        
    Returns:
        List of zone names (empty if outside all zones)
    """
    
    zones = []
    for zone_name, polygon in GEOFENCES.items():
        if point_in_polygon(lat, lon, polygon):
            zones.append(zone_name)
    
    return zones


# =========================================================
# VEHICLE STATE TRACKING
# =========================================================

# In-memory state storage (use database for production)
vehicle_states: Dict[str, Dict[str, Any]] = {}


def process_location(
    device_id: str,
    lat: float,
    lon: float,
    timestamp: Optional[datetime] = None
) -> Optional[Dict[str, Any]]:
    """
    Process a new location update and detect zone transitions.
    
    Args:
        device_id: Unique vehicle/device identifier
        lat: Current latitude
        lon: Current longitude
        timestamp: Event timestamp (defaults to current UTC time)
        
    Returns:
        Event dictionary if zone transition occurred, None otherwise
        
    Event types:
        - ENTRY: Vehicle entered a new zone
        - EXIT: Vehicle left a zone
        - DWELL_ALERT: Vehicle exceeded max dwell time in zone
    """
    
    if timestamp is None:
        timestamp = datetime.utcnow()
    
    # Detect current zone
    current_zone = detect_zone(lat, lon)
    
    # Get previous state
    previous_state = vehicle_states.get(device_id)
    
    # First observation for this device
    if previous_state is None:
        vehicle_states[device_id] = {
            "zone": current_zone,
            "entry_time": timestamp if current_zone else None,
            "last_lat": lat,
            "last_lon": lon,
            "last_update": timestamp
        }
        
        # Generate ENTRY event if starting inside a zone
        if current_zone:
            return {
                "type": "ENTRY",
                "device_id": device_id,
                "zone": current_zone,
                "latitude": lat,
                "longitude": lon,
                "timestamp": timestamp.isoformat()
            }
        return None
    
    # Check for zone transition
    prev_zone = previous_state["zone"]
    event = None
    
    if prev_zone != current_zone:
        # Zone changed - generate events
        
        # EXIT event from previous zone
        if prev_zone is not None:
            entry_time = previous_state.get("entry_time")
            dwell_time = None
            
            if entry_time:
                dwell_time = (timestamp - entry_time).total_seconds()
            
            event = {
                "type": "EXIT",
                "device_id": device_id,
                "zone": prev_zone,
                "dwell_time_seconds": dwell_time,
                "latitude": lat,
                "longitude": lon,
                "timestamp": timestamp.isoformat()
            }
            
            # Check if exit alert is needed
            zone_config = ZONE_CONFIG.get(prev_zone, {})
            if zone_config.get("alert_on_exit"):
                event["alert"] = True
                event["alert_message"] = f"Vehicle {device_id} exited {prev_zone}"
        
        # ENTRY event to new zone
        if current_zone is not None:
            event = {
                "type": "ENTRY",
                "device_id": device_id,
                "zone": current_zone,
                "latitude": lat,
                "longitude": lon,
                "timestamp": timestamp.isoformat()
            }
        
        # Update state
        vehicle_states[device_id] = {
            "zone": current_zone,
            "entry_time": timestamp if current_zone else None,
            "last_lat": lat,
            "last_lon": lon,
            "last_update": timestamp
        }
    
    else:
        # Same zone - check for dwell time alerts
        if current_zone:
            entry_time = previous_state.get("entry_time")
            zone_config = ZONE_CONFIG.get(current_zone, {})
            max_dwell = zone_config.get("max_dwell_minutes")
            
            if entry_time and max_dwell:
                dwell_minutes = (timestamp - entry_time).total_seconds() / 60
                
                if dwell_minutes > max_dwell:
                    # Check if we already sent this alert
                    if not previous_state.get("dwell_alert_sent"):
                        event = {
                            "type": "DWELL_ALERT",
                            "device_id": device_id,
                            "zone": current_zone,
                            "dwell_time_minutes": dwell_minutes,
                            "max_allowed_minutes": max_dwell,
                            "latitude": lat,
                            "longitude": lon,
                            "timestamp": timestamp.isoformat(),
                            "alert": True,
                            "alert_message": f"Vehicle {device_id} exceeded dwell time in {current_zone}"
                        }
                        previous_state["dwell_alert_sent"] = True
        
        # Update last known position
        previous_state["last_lat"] = lat
        previous_state["last_lon"] = lon
        previous_state["last_update"] = timestamp
    
    return event


def get_vehicle_state(device_id: str) -> Optional[Dict[str, Any]]:
    """Get current state of a vehicle."""
    return vehicle_states.get(device_id)


def get_all_vehicle_states() -> Dict[str, Dict[str, Any]]:
    """Get states of all tracked vehicles."""
    return vehicle_states.copy()


def clear_vehicle_state(device_id: str) -> None:
    """Clear state for a specific vehicle."""
    if device_id in vehicle_states:
        del vehicle_states[device_id]


# =========================================================
# TESTING
# =========================================================

if __name__ == "__main__":
    print("=== Geofencing Engine Test ===\n")
    
    # Test coordinates
    test_points = [
        ("Inside Main Campus", 12.9720, 80.2200),
        ("Inside Parking Lot", 12.9708, 80.2188),
        ("Inside Restricted Zone", 12.9735, 80.2220),
        ("Outside All Zones", 12.9600, 80.2100),
    ]
    
    print("Zone Detection Tests:")
    for name, lat, lon in test_points:
        zone = detect_zone(lat, lon)
        print(f"  {name}: ({lat}, {lon}) → {zone or 'OUTSIDE'}")
    
    print("\n" + "="*50)
    print("Zone Transition Simulation:")
    
    # Simulate vehicle movement
    device = "vehicle_001"
    movements = [
        (12.9600, 80.2100, "Starting outside"),
        (12.9720, 80.2200, "Entering Main Campus"),
        (12.9708, 80.2188, "Moving to Parking Lot"),
        (12.9735, 80.2220, "Entering Restricted Zone"),
        (12.9600, 80.2100, "Leaving all zones"),
    ]
    
    for lat, lon, description in movements:
        print(f"\n  → {description}: ({lat}, {lon})")
        event = process_location(device, lat, lon)
        if event:
            print(f"    EVENT: {event['type']} - Zone: {event.get('zone', 'N/A')}")
            if event.get('dwell_time_seconds'):
                print(f"    Dwell time: {event['dwell_time_seconds']:.0f} seconds")
        else:
            print(f"    No zone change")
    
    print("\n=== Test Complete ===")
