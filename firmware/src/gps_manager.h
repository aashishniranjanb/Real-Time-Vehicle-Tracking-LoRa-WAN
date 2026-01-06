/**
 * =========================================================
 * GPS MANAGER - HEADER
 * =========================================================
 * Handles GPS acquisition, movement detection, and edge filtering
 */

#pragma once
#include <Arduino.h>

// =========================================================
// GPS DATA STRUCTURE
// =========================================================
struct GPSData {
    double latitude;
    double longitude;
    float  speed;       // km/h (optional, for future use)
    float  hdop;        // Horizontal dilution of precision
    bool   valid;
};

// =========================================================
// GPS MANAGER CLASS
// =========================================================
class GPSManager {
public:
    /**
     * Initialize GPS hardware and serial interface
     */
    void begin();

    /**
     * Attempt to acquire GPS fix within timeout
     * @param data Output structure for GPS coordinates
     * @return true if valid fix acquired, false on timeout
     */
    bool getLocation(GPSData &data);

    /**
     * Check if vehicle has moved significantly from last position
     * Implements edge filtering to reduce unnecessary transmissions
     * @param current Current GPS coordinates
     * @return true if movement exceeds threshold
     */
    bool hasMovedSignificantly(const GPSData &current);

    /**
     * Power down GPS module to save energy
     */
    void powerDown();

    /**
     * Get distance from last known position
     */
    double getDistanceFromLast(const GPSData &current);

private:
    GPSData lastLocation;
    bool    hasLastFix;
};
