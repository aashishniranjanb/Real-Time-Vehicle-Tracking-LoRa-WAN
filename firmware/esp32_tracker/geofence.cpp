/**
 * =========================================================
 * GEOFENCE ENGINE - IMPLEMENTATION
 * =========================================================
 * Implements edge geofence checks against bounding box zones.
 */

#include "geofence.h"
#include "logger.h"

// Define geofence boundaries for a prototype industrial campus yard
// coordinates centered around an arbitrary plant zone (e.g. lat=12.9716, lon=77.5946)
static const GeofenceZone CAMPUS_BOUNDARY = {
    "Main Campus Yard", 
    12.9600, 12.9800, 
    77.5800, 77.6100
};

static const GeofenceZone SUB_ZONES[] = {
    {"Assembly Building", 12.9710, 12.9720, 77.5940, 77.5950},
    {"Finished Goods Yard", 12.9730, 12.9750, 77.5960, 77.5980},
    {"Logistics Loading Dock", 12.9700, 12.9715, 77.5920, 77.5935}
};

static const int NUM_SUB_ZONES = sizeof(SUB_ZONES) / sizeof(SUB_ZONES[0]);

bool isWithinCampus(double lat, double lon) {
    if (lat >= CAMPUS_BOUNDARY.latMin && lat <= CAMPUS_BOUNDARY.latMax &&
        lon >= CAMPUS_BOUNDARY.lonMin && lon <= CAMPUS_BOUNDARY.lonMax) {
        return true;
    }
    logWarn("Alert: Vehicle coordinates are OUTSIDE main campus boundary!");
    return false;
}

const char* getLocalZoneName(double lat, double lon) {
    // Check sub zones
    for (int i = 0; i < NUM_SUB_ZONES; i++) {
        const GeofenceZone& zone = SUB_ZONES[i];
        if (lat >= zone.latMin && lat <= zone.latMax &&
            lon >= zone.lonMin && lon <= zone.lonMax) {
            logInfo("Local Geofence hit: Vehicle is inside [%s]", zone.name);
            return zone.name;
        }
    }
    
    // Check main campus
    if (isWithinCampus(lat, lon)) {
        return "General Yard Transit";
    }
    
    return "Unknown Offsite Territory";
}
