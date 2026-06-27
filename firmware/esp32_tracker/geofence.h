/**
 * =========================================================
 * GEOFENCE ENGINE - HEADER
 * =========================================================
 * Evaluates vehicle coordinates locally on the ESP32 edge.
 * Identifies if the vehicle enters/exits designated zones.
 */

#pragma once
#include <Arduino.h>

struct GeofenceZone {
    const char* name;
    double latMin;
    double latMax;
    double lonMin;
    double lonMax;
};

/**
 * Determine if coordinates reside inside the active factory logistics geofence.
 * @param lat Latitude of the vehicle.
 * @param lon Longitude of the vehicle.
 * @return true if vehicle is inside the campus boundary.
 */
bool isWithinCampus(double lat, double lon);

/**
 * Identify the specific yard zone (e.g., Loading Dock, Parking, Warehouse).
 * @param lat Latitude of the vehicle.
 * @param lon Longitude of the vehicle.
 * @return Zone name string.
 */
const char* getLocalZoneName(double lat, double lon);
