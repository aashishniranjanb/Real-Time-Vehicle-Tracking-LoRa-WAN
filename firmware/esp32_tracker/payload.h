/**
 * =========================================================
 * PAYLOAD MODULE - HEADER
 * =========================================================
 * Formats GPS and system telemetry into a highly compressed
 * binary payload to minimize airtime and comply with duty cycles.
 */

#pragma once
#include <Arduino.h>

struct TrackingPayload {
    double latitude;
    double longitude;
    float  speed;
    float  heading;        // Vehicle heading / course in degrees
    float  batteryVoltage; // Battery voltage in Volts
};

// Size of the encoded binary payload in bytes
#define PAYLOAD_SIZE 14

/**
 * Serialize TrackingPayload data into a compact binary format.
 * @param data Telemetry source data.
 * @param buffer Output byte buffer. Must be at least PAYLOAD_SIZE bytes.
 * @return The size of the encoded payload.
 */
uint8_t encodeBinaryPayload(const TrackingPayload &data, uint8_t *buffer);
