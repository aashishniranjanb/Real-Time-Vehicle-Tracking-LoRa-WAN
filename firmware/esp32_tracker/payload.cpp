/**
 * =========================================================
 * PAYLOAD MODULE - IMPLEMENTATION
 * =========================================================
 * Serializes raw float values into fixed-point integers
 * and packs them byte-by-byte in big-endian representation.
 */

#include "payload.h"
#include "logger.h"

uint8_t encodeBinaryPayload(const TrackingPayload &data, uint8_t *buffer) {
    // 1. Convert floats to fixed-point representations
    int32_t lat_enc = (int32_t)(data.latitude * 1e6);  // Preserves 6 decimal places (approx. 11 cm precision)
    int32_t lon_enc = (int32_t)(data.longitude * 1e6); // Preserves 6 decimal places
    uint16_t speed_enc = (uint16_t)(data.speed * 100.0); // Speed in km/h, 2 decimals
    uint16_t head_enc = (uint16_t)(data.heading * 100.0); // Heading in degrees, 2 decimals
    uint16_t batt_enc = (uint16_t)(data.batteryVoltage * 1000.0); // Battery in millivolts (e.g. 3.7V -> 3700)

    // 2. Pack bytes (Big-Endian network order)
    
    // Latitude (Bytes 0-3)
    buffer[0] = (lat_enc >> 24) & 0xFF;
    buffer[1] = (lat_enc >> 16) & 0xFF;
    buffer[2] = (lat_enc >> 8)  & 0xFF;
    buffer[3] = lat_enc & 0xFF;

    // Longitude (Bytes 4-7)
    buffer[4] = (lon_enc >> 24) & 0xFF;
    buffer[5] = (lon_enc >> 16) & 0xFF;
    buffer[6] = (lon_enc >> 8)  & 0xFF;
    buffer[7] = lon_enc & 0xFF;

    // Speed (Bytes 8-9)
    buffer[8] = (speed_enc >> 8) & 0xFF;
    buffer[9] = speed_enc & 0xFF;

    // Heading (Bytes 10-11)
    buffer[10] = (head_enc >> 8) & 0xFF;
    buffer[11] = head_enc & 0xFF;

    // Battery (Bytes 12-13)
    buffer[12] = (batt_enc >> 8) & 0xFF;
    buffer[13] = batt_enc & 0xFF;

    // Log the payload bytes
    logDebug("Serialized Payload: [ %02X %02X %02X %02X | %02X %02X %02X %02X | %02X %02X | %02X %02X | %02X %02X ]",
             buffer[0], buffer[1], buffer[2], buffer[3],
             buffer[4], buffer[5], buffer[6], buffer[7],
             buffer[8], buffer[9], buffer[10], buffer[11],
             buffer[12], buffer[13]);

    return PAYLOAD_SIZE;
}
