/**
 * =========================================================
 * REAL-TIME VEHICLE TRACKING SYSTEM
 * ESP32 + GPS + LoRaWAN
 * =========================================================
 * 
 * Firmware: Prototype / Interview-Ready
 * 
 * Features:
 *   - GPS-based location tracking
 *   - Edge filtering (movement threshold)
 *   - Adaptive sleep intervals
 *   - LoRaWAN Class A uplink
 *   - Ultra-low power design
 * 
 * Architecture:
 *   - Single-cycle execution (wake → acquire → transmit → sleep)
 *   - Manager pattern for clean separation of concerns
 * 
 * Author: [Your Name]
 * Project: LoRaWAN Vehicle Tracking for Smart Campus
 * 
 * =========================================================
 */

#include <Arduino.h>
#include "config.h"
#include "gps_manager.h"
#include "lorawan_manager.h"
#include "power_manager.h"

// =========================================================
// GLOBAL MANAGER OBJECTS
// =========================================================
GPSManager     gps;
LoRaWANManager lorawan;
PowerManager   power;

// =========================================================
// STATE VARIABLES
// =========================================================
bool     isMoving         = false;
uint32_t nextSleepInterval = SLEEP_STATIONARY_SEC;

// =========================================================
// SETUP - Runs once on power-on or deep sleep wakeup
// =========================================================
void setup() {
    
    // Initialize debug serial
    Serial.begin(DEBUG_BAUD_RATE);
    delay(1000);  // Allow serial to stabilize

    Serial.println();
    Serial.println("=========================================");
    Serial.println(" LORAWAN VEHICLE TRACKING NODE");
    Serial.println(" Starting tracking cycle...");
    Serial.println("=========================================");

    // Initialize subsystems
    power.begin();    // Also prints wakeup reason
    gps.begin();
    lorawan.begin();

    Serial.println("[MAIN] Initialization complete");
    Serial.println();
}

// =========================================================
// MAIN LOOP - Single-cycle execution
// =========================================================
/**
 * NOTE: This firmware uses single-cycle execution pattern.
 * 
 * Each iteration:
 *   1. Acquire GPS fix
 *   2. Check for significant movement (edge filtering)
 *   3. Encode and transmit payload via LoRaWAN
 *   4. Calculate adaptive sleep interval
 *   5. Enter deep sleep
 * 
 * After deep sleep, ESP32 resets and re-enters setup().
 * This is intentional and correct for low-power IoT firmware.
 */
void loop() {

    // ---------------------------------------------------------
    // STEP 1: Acquire GPS Location
    // ---------------------------------------------------------
    Serial.println("[MAIN] Step 1: Acquiring GPS fix...");
    
    GPSData currentLocation;
    bool gpsValid = gps.getLocation(currentLocation);

    if (!gpsValid) {
        // GPS failure - sleep and retry later
        Serial.println("[MAIN] GPS unavailable. Sleeping and retrying.");
        power.deepSleep(SLEEP_ERROR_SEC);
        // Note: Code below this never executes (ESP32 resets on wakeup)
    }

    // ---------------------------------------------------------
    // STEP 2: Edge Filtering - Check meaningful movement
    // ---------------------------------------------------------
    Serial.println("[MAIN] Step 2: Checking movement...");
    
    isMoving = gps.hasMovedSignificantly(currentLocation);

    if (!isMoving) {
        // No significant movement - skip transmission to save power
        Serial.println("[MAIN] No significant movement. Skipping transmission.");
        power.deepSleep(SLEEP_STATIONARY_SEC);
    }

    // ---------------------------------------------------------
    // STEP 3: Encode Payload
    // ---------------------------------------------------------
    Serial.println("[MAIN] Step 3: Encoding payload...");
    
    uint8_t payload[LORAWAN_PAYLOAD_SIZE];
    uint8_t payloadSize = lorawan.encodePayload(
        currentLocation.latitude,
        currentLocation.longitude,
        payload
    );

    // ---------------------------------------------------------
    // STEP 4: Transmit via LoRaWAN
    // ---------------------------------------------------------
    Serial.println("[MAIN] Step 4: Transmitting via LoRaWAN...");
    
    bool txSuccess = lorawan.sendUplink(payload, payloadSize);

    if (txSuccess) {
        Serial.println("[MAIN] LoRaWAN uplink successful");
    } else {
        Serial.println("[MAIN] LoRaWAN uplink failed");
        // Continue anyway - next cycle will retry
    }

    // ---------------------------------------------------------
    // STEP 5: Adaptive Sleep Decision
    // ---------------------------------------------------------
    Serial.println("[MAIN] Step 5: Calculating sleep interval...");
    
    // Adaptive tracking:
    //   - Moving:     Short sleep (30-60 sec) for frequent updates
    //   - Stationary: Long sleep (5-10 min) to conserve power
    nextSleepInterval = isMoving ? SLEEP_MOVING_SEC : SLEEP_STATIONARY_SEC;

    Serial.print("[MAIN] Next sleep interval: ");
    Serial.print(nextSleepInterval);
    Serial.println(" seconds");

    // ---------------------------------------------------------
    // STEP 6: Enter Deep Sleep
    // ---------------------------------------------------------
    Serial.println("[MAIN] Step 6: Entering deep sleep...");
    Serial.println("=========================================");
    Serial.println();
    
    power.deepSleep(nextSleepInterval);
    
    // ---------------------------------------------------------
    // NOTE: Execution never reaches here.
    // ESP32 resets on wakeup and re-runs setup().
    // ---------------------------------------------------------
}
