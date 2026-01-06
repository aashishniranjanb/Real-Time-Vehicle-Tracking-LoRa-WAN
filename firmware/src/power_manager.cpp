/**
 * =========================================================
 * POWER MANAGER - IMPLEMENTATION
 * =========================================================
 * ESP32-specific deep sleep implementation
 * Timer-based wakeup for periodic tracking
 */

#include "power_manager.h"
#include "config.h"
#include <esp_sleep.h>
#include <esp_wifi.h>
#include <esp_bt.h>

// =========================================================
// INITIALIZATION
// =========================================================
void PowerManager::begin() {
    // Disable unused radios to save power
    disableRadios();
    
    // Print wakeup reason for debugging
    printWakeupReason();
    
    Serial.println("[PWR] Power manager initialized");
}

// =========================================================
// DEEP SLEEP
// =========================================================
void PowerManager::deepSleep(uint32_t seconds) {
    
    Serial.print("[PWR] Entering deep sleep for ");
    Serial.print(seconds);
    Serial.println(" seconds...");

    // Convert seconds to microseconds
    uint64_t sleepTimeUs = (uint64_t)seconds * 1000000ULL;

    // Configure timer wakeup
    esp_sleep_enable_timer_wakeup(sleepTimeUs);

    // Optional: Configure GPIO wakeup for external interrupt
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_X, 1);

    // Flush serial output before sleeping
    Serial.flush();
    delay(100);

    // Enter deep sleep
    // NOTE: After deep sleep, ESP32 resets and starts from setup()
    esp_deep_sleep_start();

    // Code after this point will never execute
}

// =========================================================
// WAKEUP REASON
// =========================================================
void PowerManager::printWakeupReason() {
    
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    Serial.print("[PWR] Wakeup reason: ");

    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Timer");
            break;
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("External signal (RTC_IO)");
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            Serial.println("External signal (RTC_CNTL)");
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            Serial.println("Touchpad");
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            Serial.println("ULP coprocessor");
            break;
        default:
            Serial.println("Power-on or reset");
            break;
    }
}

// =========================================================
// DISABLE UNUSED RADIOS
// =========================================================
void PowerManager::disableRadios() {
    
    // Disable WiFi
    esp_wifi_stop();
    esp_wifi_deinit();
    
    // Disable Bluetooth
    esp_bt_controller_disable();
    
    Serial.println("[PWR] WiFi and Bluetooth disabled");
}
