/**
 * =========================================================
 * LORAWAN VEHICLE TRACKING SYSTEM - CONFIGURATION
 * =========================================================
 * Central configuration file for all hardware pins, constants,
 * keys, and thresholds.
 */

#pragma once

// =========================================================
// SYSTEM DEBUG CONFIGURATION
// =========================================================
#define DEBUG_SERIAL          1      // 1 to enable serial logging, 0 to disable
#define DEBUG_BAUD_RATE       115200

// =========================================================
// GPS HARDWARE CONFIGURATION
// =========================================================
#define GPS_RX_PIN            16     // ESP32 RX pin (connects to GPS TX)
#define GPS_TX_PIN            17     // ESP32 TX pin (connects to GPS RX)
#define GPS_BAUD_RATE         9600   // NEO-6M default baud rate
#define GPS_FIX_TIMEOUT_MS    15000  // Max time to wait for a GPS lock

// =========================================================
// LORA P2P MODULE CONFIGURATION (SX1278 / RFM95)
// =========================================================
#define LORA_SCK_PIN          5
#define LORA_MISO_PIN         19
#define LORA_MOSI_PIN         27
#define LORA_NSS_PIN          18     // Chip CS pin
#define LORA_RST_PIN          14     // Reset pin
#define LORA_DIO0_PIN         26     // DIO0 Interrupt pin

// LoRa RF Settings (Region-dependent)
#define LORA_FREQUENCY        865.0E6 // Frequency in Hz (e.g., 865.0E6 for India, 915.0E6 for US, 868.0E6 for EU)
#define LORA_TX_POWER         14      // Transmit power in dBm (up to 20)
#define LORA_SPREADING_FACTOR 7       // Spreading factor (6 to 12)
#define LORA_BANDWIDTH        125E3   // Bandwidth in Hz

// =========================================================
// LORAWAN KEYS & CONFIG (Keep for Experiments/Future Upgrade)
// =========================================================
#define REGION_IN865          1
#define REGION_US915          0
#define REGION_EU868          0

// Placeholder OTAA Keys (represented as hex bytes)
// Replace these with your actual TTN/ChirpStack keys
#define DEV_EUI   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define APP_EUI   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define APP_KEY   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

// =========================================================
// ADAPTIVE SLEEP INTERVALS (in seconds)
// =========================================================
#define SLEEP_MOVING_SEC      30     // Update frequency when moving (30 sec)
#define SLEEP_STATIONARY_SEC  300    // Heartbeat frequency when parked (5 min)
#define SLEEP_ERROR_SEC       120    // Wake up and retry on GPS/sensor failure (2 min)

// =========================================================
// SENSOR & DECISION ENGINE THRESHOLDS
// =========================================================
#define MOVEMENT_THRESHOLD_M  10.0   // Distance threshold (meters) to classify as movement
#define HDOP_MAX_THRESHOLD    4.0    // Maximum acceptable horizontal dilution of precision (lower is better)

// =========================================================
// POWER MANAGEMENT CONFIGURATION
// =========================================================
#define BATTERY_ADC_PIN       35     // ESP32 ADC pin connected to battery divider
#define BATTERY_DIVIDER_RATIO 2.0    // Resistor divider ratio (e.g. 100k/100k)
#define LOW_BATT_THRESHOLD_V  3.4    // Minimum voltage before entering low-power hibernation
