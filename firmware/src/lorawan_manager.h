/**
 * =========================================================
 * LORAWAN MANAGER - HEADER
 * =========================================================
 * Handles LoRaWAN communication using LMIC library
 * Uses ABP activation for prototype simplicity
 */

#pragma once
#include <Arduino.h>

// =========================================================
// LORAWAN MANAGER CLASS
// =========================================================
class LoRaWANManager {
public:
    /**
     * Initialize LMIC stack and configure session keys
     */
    void begin();

    /**
     * Encode GPS coordinates into compact binary payload
     * Uses fixed-point encoding to minimize packet size
     * @param lat Latitude in degrees
     * @param lon Longitude in degrees
     * @param buffer Output buffer for encoded payload
     * @return Number of bytes written to buffer
     */
    uint8_t encodePayload(double lat, double lon, uint8_t *buffer);

    /**
     * Transmit data via LoRaWAN uplink
     * Uses unconfirmed uplink on port 1
     * @param data Pointer to payload buffer
     * @param length Payload length in bytes
     * @return true if transmission queued successfully
     */
    bool sendUplink(uint8_t *data, uint8_t length);

    /**
     * Check if radio is currently busy
     */
    bool isBusy();

    /**
     * Process LMIC events (call in loop if needed)
     */
    void update();
};
