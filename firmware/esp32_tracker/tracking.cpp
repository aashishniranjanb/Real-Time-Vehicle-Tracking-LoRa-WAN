/**
 * =========================================================
 * TRACKING MODULE - IMPLEMENTATION
 * =========================================================
 * Implements GPS coordinate comparison and Haversine distance.
 * Persists variables across deep sleep cycles using RTC memory.
 */

#include "tracking.h"
#include "config.h"
#include "logger.h"
#include <math.h>

// =========================================================
// RTC MEMORY VARIABLES
// =========================================================
// Variables marked with RTC_DATA_ATTR are placed in RTC slow memory
// and survive ESP32 deep sleep cycles.
RTC_DATA_ATTR static double rtcLastLat = 0.0;
RTC_DATA_ATTR static double rtcLastLon = 0.0;
RTC_DATA_ATTR static bool   rtcHasLastFix = false;

#define DEG_TO_RAD 0.017453292519943295769236907684886

/**
 * Math helper to calculate Haversine distance.
 */
static double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    double dLat = (lat2 - lat1) * DEG_TO_RAD;
    double dLon = (lon2 - lon1) * DEG_TO_RAD;
    
    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1 * DEG_TO_RAD) * cos(lat2 * DEG_TO_RAD) *
               sin(dLon / 2) * sin(dLon / 2);
               
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return 6371000.0 * c; // Returns distance in meters
}

bool evaluateMovement(double currentLat, double currentLon, float speed) {
    // 1. Direct velocity check: if vehicle is moving faster than 1.5 km/h
    if (speed > 1.5) {
        logInfo("Movement detected via velocity sensor: %.1f km/h", speed);
        rtcLastLat = currentLat;
        rtcLastLon = currentLon;
        rtcHasLastFix = true;
        return true;
    }

    // 2. Initial fix check: first boot or first GPS fix
    if (!rtcHasLastFix) {
        logInfo("Initial coordinate baseline established.");
        rtcLastLat = currentLat;
        rtcLastLon = currentLon;
        rtcHasLastFix = true;
        return true; // Report true to force an initial telemetry upload
    }

    // 3. Calculate distance change from baseline
    double distanceMoved = calculateDistance(rtcLastLat, rtcLastLon, currentLat, currentLon);
    logInfo("Distance delta since last report: %.2f meters", distanceMoved);

    // 4. Compare with threshold
    if (distanceMoved >= MOVEMENT_THRESHOLD_M) {
        logInfo("Significant movement detected (>= %.1fm). Updating position.", MOVEMENT_THRESHOLD_M);
        rtcLastLat = currentLat;
        rtcLastLon = currentLon;
        return true;
    }

    logInfo("Position delta is below threshold. Filtering out GPS drift/noise.");
    return false;
}

uint32_t getSleepInterval(bool isMoving) {
    if (isMoving) {
        logInfo("Vehicle state: MOVING. Adaptive frequency = %d seconds.", SLEEP_MOVING_SEC);
        return SLEEP_MOVING_SEC;
    } else {
        logInfo("Vehicle state: STATIONARY. Adaptive frequency = %d seconds.", SLEEP_STATIONARY_SEC);
        return SLEEP_STATIONARY_SEC;
    }
}

double getLastLat() {
    return rtcLastLat;
}

double getLastLon() {
    return rtcLastLon;
}
