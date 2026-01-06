/**
 * =========================================================
 * GPS MANAGER - IMPLEMENTATION
 * =========================================================
 * Uses TinyGPS++ for NMEA parsing
 * Implements edge-based movement filtering
 */

#include "gps_manager.h"
#include "config.h"
#include <TinyGPS++.h>
#include <HardwareSerial.h>

// =========================================================
// INTERNAL OBJECTS
// =========================================================
static TinyGPSPlus gps;
static HardwareSerial GPSSerial(1);  // Use UART1

// =========================================================
// INITIALIZATION
// =========================================================
void GPSManager::begin() {
    // Initialize UART for GPS module
    GPSSerial.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    
    // Initialize state
    hasLastFix = false;
    lastLocation = {0.0, 0.0, 0.0, 99.0, false};
    
    Serial.println("[GPS] Initialized on UART1");
}

// =========================================================
// ACQUIRE GPS FIX
// =========================================================
bool GPSManager::getLocation(GPSData &data) {
    
    Serial.println("[GPS] Acquiring fix...");
    unsigned long startTime = millis();

    // Poll GPS until valid fix or timeout
    while (millis() - startTime < GPS_FIX_TIMEOUT_MS) {
        
        // Feed NMEA data to parser
        while (GPSSerial.available()) {
            gps.encode(GPSSerial.read());
        }

        // Check for valid updated location
        if (gps.location.isUpdated() && gps.location.isValid()) {
            
            data.latitude  = gps.location.lat();
            data.longitude = gps.location.lng();
            data.speed     = gps.speed.kmph();
            data.hdop      = gps.hdop.hdop();
            data.valid     = true;

            Serial.print("[GPS] Fix acquired: ");
            Serial.print(data.latitude, 6);
            Serial.print(", ");
            Serial.println(data.longitude, 6);
            
            return true;
        }

        delay(10);  // Small delay to prevent busy-waiting
    }

    // Timeout: No valid fix
    Serial.println("[GPS] Fix timeout - no valid data");
    data.valid = false;
    return false;
}

// =========================================================
// MOVEMENT DETECTION (Edge Filtering)
// =========================================================
bool GPSManager::hasMovedSignificantly(const GPSData &current) {
    
    // First valid fix → always report as movement
    if (!hasLastFix) {
        lastLocation = current;
        hasLastFix = true;
        Serial.println("[GPS] First fix - treating as movement");
        return true;
    }

    // Calculate distance from last known position
    double distance = getDistanceFromLast(current);

    Serial.print("[GPS] Distance from last: ");
    Serial.print(distance, 1);
    Serial.println(" meters");

    // Check against threshold
    if (distance >= MOVEMENT_THRESHOLD_M) {
        // Significant movement detected - update last position
        lastLocation = current;
        Serial.println("[GPS] Significant movement detected");
        return true;
    }

    // Movement below threshold → likely GPS drift, ignore
    Serial.println("[GPS] No significant movement (filtering GPS drift)");
    return false;
}

// =========================================================
// DISTANCE CALCULATION
// =========================================================
double GPSManager::getDistanceFromLast(const GPSData &current) {
    return TinyGPSPlus::distanceBetween(
        lastLocation.latitude,
        lastLocation.longitude,
        current.latitude,
        current.longitude
    );
}

// =========================================================
// POWER DOWN GPS
// =========================================================
void GPSManager::powerDown() {
    // For NEO-6M, we can send a UBX command to enter backup mode
    // Or simply stop reading (module idles automatically)
    Serial.println("[GPS] Power down requested");
}
