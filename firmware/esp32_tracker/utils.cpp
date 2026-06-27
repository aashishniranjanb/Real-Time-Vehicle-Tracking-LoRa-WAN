/**
 * =========================================================
 * UTILITY HELPERS - IMPLEMENTATION
 * =========================================================
 * Implements math and format helpers.
 */

#include "utils.h"

const char* getCompassDirection(float heading) {
    if (heading < 0.0f || heading > 360.0f) {
        return "N/A";
    }
    
    // Map degrees into 8 compass sectors (45 deg each, offset by 22.5 deg)
    const char* directions[] = {
        "North", "Northeast", "East", "Southeast",
        "South", "Southwest", "West", "Northwest"
    };
    
    int index = (int)((heading + 22.5f) / 45.0f) % 8;
    return directions[index];
}
