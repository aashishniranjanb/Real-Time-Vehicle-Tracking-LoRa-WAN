/**
 * ====================================================================
 * INDUSTRIAL FLEET TRACKING SYSTEM
 * ESP32 + GPS (NEO-6M) + LoRa P2P Telemetry
 * ====================================================================
 * 
 * Target Application: Autonomous Factory Logistics & Fleet Yard Tracking
 * Architecture: Event-driven modular firmware with single-cycle execution.
 * 
 * Execution Loop Flow:
 *   Wake-up → Init Peripherals → Read Sensors → Evaluate Geofence/Motion 
 *   → Binary Serialization → LoRa P2P Uplink → Sleep peripheral → Deep Sleep
 * 
 * Note: When waking up from deep sleep, ESP32 resets program counter and 
 * starts execution fresh from setup(). This is the industry-standard 
 * pattern for ultra-low power battery-operated nodes.
 */

#include "config.h"
#include "logger.h"
#include "gps.h"
#include "lora.h"
#include "payload.h"
#include "power.h"
#include "tracking.h"
#include "geofence.h"
#include "utils.h"

// ====================================================================
// SETUP - Executes once on power-on or deep sleep wakeup
// ====================================================================
void setup() {
    // 1. Initialize Serial debugging and print welcome header
    logInit();
    
    // 2. Identify why the chip woke up (Timer, Pin reset, etc.)
    wakeReason();

    // 3. Perform low battery brownout protection check
    // If battery is below 3.4V, this will halt the MCU in sleep mode
    if (lowBatteryProtection()) {
        return; 
    }

    // 4. Initialize low-level peripherals (GPS UART & LoRa SPI pins)
    initGPS();
    connectLoRa();
    
    logInfo("Node initialization completed successfully.");
    logInfo("-----------------------------------------");
}

// ====================================================================
// LOOP - Single-cycle task execution
// ====================================================================
void loop() {
    logInfo("Starting tracking telemetry cycle...");

    // 1. Sample battery cell voltage
    float currentVolts = batteryVoltage();

    // 2. Acquire a valid GPS signal (within GPS_FIX_TIMEOUT_MS)
    bool hasFix = getLocation();

    if (!hasFix) {
        // Fallback Strategy: If GPS fails (e.g. vehicle parked inside warehouse),
        // we wake up and try again after a short retry interval.
        logWarn("GPS signal unavailable. Caching last known position: Lat: %.6f, Lon: %.6f", 
                getLastLat(), getLastLon());
        
        // Put transceiver to sleep to save power
        sleepLoRa();
        
        // Enter deep sleep for retry duration
        enterDeepSleep(SLEEP_ERROR_SEC);
        return; 
    }

    // 3. Package telemetry variables into structured payload object
    TrackingPayload telemetry;
    telemetry.latitude = latitude();
    telemetry.longitude = longitude();
    telemetry.speed = speed();
    telemetry.heading = 90.0f; // Mock heading (East) or parse from GPS course
    telemetry.batteryVoltage = currentVolts;

    // 4. Execute edge-based local geofence detection
    const char* activeZone = getLocalZoneName(telemetry.latitude, telemetry.longitude);
    const char* directionStr = getCompassDirection(telemetry.heading);
    logInfo("Location telemetry check: Zone = '%s', Heading = %s", activeZone, directionStr);

    // 5. Evaluate movement status (Edge filtering to discard GPS jitter/drift)
    bool isMoving = evaluateMovement(telemetry.latitude, telemetry.longitude, telemetry.speed);

    // 6. Transmit if moving, or if parked wait for periodic heartbeat report
    if (isMoving) {
        logInfo("Vehicle activity detected. Encoding telemetry data...");
        
        // Compress data into 14-byte binary payload
        uint8_t buffer[PAYLOAD_SIZE];
        uint8_t len = encodeBinaryPayload(telemetry, buffer);

        // Upload telemetry package via P2P LoRa RF
        sendPayload(buffer, len);
    } else {
        logInfo("Vehicle is stationary (idle in yard). Skipped radio upload to save battery.");
    }

    // 7. Put LoRa radio transceiver to sleep
    sleepLoRa();

    // 8. Determine sleep time using adaptive logic (30s if moving, 5 min if stationary)
    uint32_t sleepDuration = getSleepInterval(isMoving);
    
    logInfo("Telemetry cycle complete. Shutting down into deep sleep...");
    logInfo("=========================================================");
    
    // 9. Enter low-power Deep Sleep
    enterDeepSleep(sleepDuration);
}
