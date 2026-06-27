/**
 * =========================================================
 * GPS MODULE - HEADER
 * =========================================================
 * Interfaces with the U-blox NEO-6M GPS module.
 * Responsible only for acquiring and parsing GPS data.
 */

#pragma once
#include <Arduino.h>

/**
 * Initialize GPS hardware (Serial connection).
 * @return true if interface starts successfully.
 */
bool initGPS();

/**
 * Poll GPS module and decode NMEA messages.
 * Attempts to acquire a valid location fix within a timeout.
 * @return true if a valid fix is acquired.
 */
bool getLocation();

/**
 * Get current latitude.
 * @return latitude in decimal degrees.
 */
double latitude();

/**
 * Get current longitude.
 * @return longitude in decimal degrees.
 */
double longitude();

/**
 * Get current speed.
 * @return speed in km/h.
 */
float speed();

/**
 * Get current satellite count.
 * @return number of connected satellites.
 */
uint8_t satellites();

/**
 * Get horizontal dilution of precision (HDOP) for quality filtering.
 * @return HDOP value (lower is better, < 4.0 is ideal).
 */
float hdop();
