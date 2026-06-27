/**
 * =========================================================
 * TRACKING MODULE - HEADER
 * =========================================================
 * Implements adaptive tracking algorithms.
 * Calculates sleep intervals based on vehicle motion state
 * and filters out GPS drift.
 */

#pragma once
#include <Arduino.h>

/**
 * Check if the vehicle is currently moving by comparing the current GPS 
 * coordinate against the last stored coordinate.
 * Uses RTC-backed memory to persist location across deep sleep cycles.
 * 
 * @param currentLat Current latitude.
 * @param currentLon Current longitude.
 * @param speed Current speed in km/h.
 * @return true if vehicle has moved significantly.
 */
bool evaluateMovement(double currentLat, double currentLon, float speed);

/**
 * Determine the deep sleep duration based on the vehicle movement status.
 * @param isMoving Whether the vehicle is currently moving.
 * @return Sleep interval in seconds.
 */
uint32_t getSleepInterval(bool isMoving);

/**
 * Get the last known latitude stored in RTC memory.
 */
double getLastLat();

/**
 * Get the last known longitude stored in RTC memory.
 */
double getLastLon();
