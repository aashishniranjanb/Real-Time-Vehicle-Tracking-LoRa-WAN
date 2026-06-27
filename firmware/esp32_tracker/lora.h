/**
 * =========================================================
 * LORA MODULE - HEADER
 * =========================================================
 * Handles peer-to-peer (P2P) LoRa communication using SX1278.
 * Avoids LoRaWAN overhead for simplified local telemetry.
 */

#pragma once
#include <Arduino.h>

/**
 * Initialize the LoRa transceiver module.
 * @return true if communication starts successfully.
 */
bool connectLoRa();

/**
 * Transmit a raw binary payload.
 * @param payload Pointer to the binary data buffer.
 * @param length Size of the data buffer in bytes.
 * @return true if transmission completes.
 */
bool sendPayload(uint8_t *payload, uint8_t length);

/**
 * Put the LoRa radio into sleep mode to conserve energy.
 */
void sleepLoRa();
