/**
 * =========================================================
 * POWER MANAGER - HEADER
 * =========================================================
 * Handles ESP32 power management and deep sleep
 */

#pragma once
#include <Arduino.h>

// =========================================================
// POWER MANAGER CLASS
// =========================================================
class PowerManager {
public:
    /**
     * Initialize power management subsystem
     */
    void begin();

    /**
     * Enter deep sleep for specified duration
     * Uses timer wakeup source
     * @param seconds Sleep duration in seconds
     */
    void deepSleep(uint32_t seconds);

    /**
     * Get the reason for last wakeup
     * Useful for debugging and state restoration
     */
    void printWakeupReason();

    /**
     * Disable WiFi and Bluetooth to save power
     */
    void disableRadios();
};
