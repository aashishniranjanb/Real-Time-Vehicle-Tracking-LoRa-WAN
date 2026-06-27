/**
 * =========================================================
 * POWER MODULE - HEADER
 * =========================================================
 * Manages ESP32 low-power deep sleep cycles, battery level
 * detection, and low battery brownout protection.
 */

#pragma once
#include <Arduino.h>

/**
 * Configure the wake-up timer and put the ESP32 into deep sleep.
 * @param seconds Sleep duration in seconds.
 */
void enterDeepSleep(uint32_t seconds);

/**
 * Identify and print the wake-up cause to Serial.
 */
void wakeReason();

/**
 * Measure and calculate the battery cell voltage.
 * @return battery voltage in Volts (typically 3.0V to 4.2V).
 */
float batteryVoltage();

/**
 * Check battery health. If cell voltage falls below threshold,
 * enters deep sleep indefinitely to prevent lithium cell degradation.
 * @return true if system is shutting down due to low battery.
 */
bool lowBatteryProtection();
