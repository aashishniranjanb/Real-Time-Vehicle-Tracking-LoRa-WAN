"""
=========================================================
DATA PARSER - LoRaWAN Payload Decoder
=========================================================

Decodes binary payloads from LoRaWAN uplinks.
Matches the fixed-point encoding used in firmware.

Payload Format:
    Bytes 0-3: Latitude  (int32, scaled by 1e6)
    Bytes 4-7: Longitude (int32, scaled by 1e6)
    
Total: 8 bytes

Author: LoRaWAN Vehicle Tracking Project
=========================================================
"""

from datetime import datetime
from typing import Dict, Any, Optional


def decode_payload(payload_bytes: bytes) -> Dict[str, float]:
    """
    Decode binary GPS payload from LoRaWAN uplink.
    
    Args:
        payload_bytes: Raw binary payload (minimum 8 bytes)
        
    Returns:
        Dictionary with latitude and longitude as floats
        
    Raises:
        ValueError: If payload is too short
        
    Example:
        >>> payload = bytes.fromhex('00C5C1000052A300')
        >>> decode_payload(payload)
        {'latitude': 12.963072, 'longitude': 5415680.0}
    """
    
    if len(payload_bytes) < 8:
        raise ValueError(f"Invalid payload length: {len(payload_bytes)} bytes (expected >= 8)")
    
    # Decode latitude (bytes 0-3, big-endian, signed)
    lat_raw = int.from_bytes(payload_bytes[0:4], byteorder='big', signed=True)
    
    # Decode longitude (bytes 4-7, big-endian, signed)
    lon_raw = int.from_bytes(payload_bytes[4:8], byteorder='big', signed=True)
    
    # Convert from fixed-point (scaled by 1e6) back to float
    latitude = lat_raw / 1_000_000
    longitude = lon_raw / 1_000_000
    
    return {
        "latitude": latitude,
        "longitude": longitude
    }


def decode_extended_payload(payload_bytes: bytes) -> Dict[str, Any]:
    """
    Decode extended payload with additional sensor data.
    
    Extended Format (future use):
        Bytes 0-3:  Latitude
        Bytes 4-7:  Longitude
        Bytes 8-9:  Speed (uint16, 0.01 km/h units)
        Bytes 10:   Battery level (0-100%)
        Bytes 11:   Status flags
        
    Args:
        payload_bytes: Raw binary payload
        
    Returns:
        Dictionary with all decoded fields
    """
    
    # Get basic GPS data
    data = decode_payload(payload_bytes)
    
    # Decode extended fields if present
    if len(payload_bytes) >= 10:
        speed_raw = int.from_bytes(payload_bytes[8:10], byteorder='big', signed=False)
        data["speed_kmh"] = speed_raw * 0.01
        
    if len(payload_bytes) >= 11:
        data["battery_percent"] = payload_bytes[10]
        
    if len(payload_bytes) >= 12:
        status = payload_bytes[11]
        data["is_moving"] = bool(status & 0x01)
        data["gps_valid"] = bool(status & 0x02)
        data["low_battery"] = bool(status & 0x04)
    
    return data


def create_location_record(
    device_id: str,
    payload_bytes: bytes,
    received_at: Optional[str] = None,
    rssi: Optional[int] = None,
    snr: Optional[float] = None
) -> Dict[str, Any]:
    """
    Create a complete location record with metadata.
    
    Args:
        device_id: TTN device identifier
        payload_bytes: Raw binary payload
        received_at: ISO timestamp from TTN
        rssi: Received Signal Strength Indicator
        snr: Signal-to-Noise Ratio
        
    Returns:
        Complete location record ready for database storage
    """
    
    # Decode payload
    location = decode_payload(payload_bytes)
    
    # Build complete record
    record = {
        "device_id": device_id,
        "latitude": location["latitude"],
        "longitude": location["longitude"],
        "received_at": received_at or datetime.utcnow().isoformat(),
        "processed_at": datetime.utcnow().isoformat()
    }
    
    # Add RF metadata if available
    if rssi is not None:
        record["rssi"] = rssi
    if snr is not None:
        record["snr"] = snr
    
    return record


# =========================================================
# TESTING / VALIDATION
# =========================================================

if __name__ == "__main__":
    # Test with sample payload
    # Latitude: 12.963072 → 12963072 → 0x00C5C100
    # Longitude: 80.221350 → 80221350 → 0x04C83C26
    
    test_payload = bytes([
        0x00, 0xC5, 0xC1, 0x00,  # Latitude
        0x04, 0xC8, 0x3C, 0x26   # Longitude
    ])
    
    result = decode_payload(test_payload)
    
    print("=== Data Parser Test ===")
    print(f"Payload (hex): {test_payload.hex().upper()}")
    print(f"Decoded latitude:  {result['latitude']:.6f}")
    print(f"Decoded longitude: {result['longitude']:.6f}")
    
    # Verify
    expected_lat = 12.963072
    expected_lon = 80.221350
    
    lat_ok = abs(result['latitude'] - expected_lat) < 0.000001
    lon_ok = abs(result['longitude'] - expected_lon) < 0.000001
    
    print(f"\nValidation: {'✓ PASS' if lat_ok and lon_ok else '✗ FAIL'}")
