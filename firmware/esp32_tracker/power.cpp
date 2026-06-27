/**
 * =========================================================
 * POWER MODULE - IMPLEMENTATION
 * =========================================================
 * Integrates ESP32 native SDK sleep registers and ADC measurement.
 */

#include "power.h"
#include "config.h"
#include "logger.h"
#include <esp_sleep.h>
#include <esp_wifi.h>
#include <esp_bt.h>

void enterDeepSleep(uint32_t seconds) {
    logInfo("Preparing for deep sleep...");
    
    // 1. Disable radio peripherals to minimize sleep leakage
    esp_wifi_stop();
    esp_wifi_deinit();
    esp_bt_controller_disable();
    logDebug("Radios (WiFi + BT) disabled.");

    // 2. Configure sleep wakeup timer
    uint64_t sleepUs = (uint64_t)seconds * 1000000ULL;
    esp_sleep_enable_timer_wakeup(sleepUs);
    logInfo("Configured wake-up timer: %d seconds.", seconds);

    // 3. Flush Serial buffers completely
    Serial.flush();
    delay(100);

    // 4. Enter sleep
    esp_deep_sleep_start();
}

void wakeReason() {
    esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();
    
    switch (reason) {
        case ESP_SLEEP_WAKEUP_TIMER:
            logInfo("Wakeup source: TIMER (Periodic tracking heartbeat)");
            break;
        case ESP_SLEEP_WAKEUP_EXT0:
            logInfo("Wakeup source: EXT0 (External signal - potential motion sensor trigger)");
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            logInfo("Wakeup source: EXT1 (RTC IO trigger)");
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            logInfo("Wakeup source: TOUCHPAD");
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            logInfo("Wakeup source: ULP Coprocessor");
            break;
        default:
            logInfo("Wakeup source: POWER-ON / HARD RESET");
            break;
    }
}

float batteryVoltage() {
    // 1. Perform ADC reading
    int adcRaw = analogRead(BATTERY_ADC_PIN);
    
    // 2. Convert raw ADC (12-bit, 0-4095) to voltage (0-3.3V reference)
    // and account for the resistor divider ratio (typically 2 for 100k/100k)
    float cellVoltage = (float)adcRaw / 4095.0f * 3.3f * BATTERY_DIVIDER_RATIO;
    
    logDebug("Battery reading: Raw ADC = %d, Cell Voltage = %.2f V", adcRaw, cellVoltage);
    return cellVoltage;
}

bool lowBatteryProtection() {
    float voltage = batteryVoltage();
    
    if (voltage < LOW_BATT_THRESHOLD_V) {
        logError("CRITICAL: Battery level is too low (%.2fV < %.2fV)!", voltage, LOW_BATT_THRESHOLD_V);
        logError("Entering indefinite shutdown hibernation to protect Li-Po cell from over-discharge.");
        
        // Disable timer wakeups
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
        
        // Flush serial and sleep indefinitely (or until manual power cycle / charger connected)
        Serial.flush();
        delay(100);
        esp_deep_sleep_start();
        return true;
    }
    
    logInfo("Battery status healthy: %.2f V", voltage);
    return false;
}
