/**
 * =========================================================
 * LORAWAN VEHICLE TRACKING - CONFIGURATION
 * =========================================================
 * Central configuration file for all firmware parameters
 */

#pragma once

// =========================================================
// SLEEP INTERVALS (Adaptive Tracking)
// =========================================================
#define SLEEP_MOVING_SEC      30    // When vehicle is moving
#define SLEEP_STATIONARY_SEC  600   // When vehicle is stationary (10 min)
#define SLEEP_ERROR_SEC       300   // On GPS failure (5 min)

// =========================================================
// GPS CONFIGURATION
// =========================================================
#define GPS_RX_PIN            16    // ESP32 RX pin (connects to GPS TX)
#define GPS_TX_PIN            17    // ESP32 TX pin (connects to GPS RX)
#define GPS_BAUD_RATE         9600
#define GPS_FIX_TIMEOUT_MS    15000 // Max time to wait for GPS fix
#define MOVEMENT_THRESHOLD_M  10.0  // Minimum movement to trigger transmission

// =========================================================
// LORAWAN CONFIGURATION
// =========================================================
#define LORAWAN_PAYLOAD_SIZE  16

// SPI Pin Mapping for SX127x LoRa Module
#define LORA_NSS_PIN          18
#define LORA_RST_PIN          14
#define LORA_DIO0_PIN         26
#define LORA_DIO1_PIN         33
#define LORA_DIO2_PIN         32

// =========================================================
// DEBUG OPTIONS
// =========================================================
#define DEBUG_SERIAL          1     // Enable serial debug output
#define DEBUG_BAUD_RATE       115200
