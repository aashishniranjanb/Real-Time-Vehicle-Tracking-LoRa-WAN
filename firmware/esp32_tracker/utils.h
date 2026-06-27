/**
 * =========================================================
 * UTILITY HELPERS - HEADER
 * =========================================================
 * Defines utility functions like compass direction lookup.
 */

#pragma once
#include <Arduino.h>

/**
 * Translate a numeric heading in degrees (0-360) to compass direction.
 * @param heading Course angle in degrees.
 * @return String representation (e.g. "North", "Southeast").
 */
const char* getCompassDirection(float heading);
