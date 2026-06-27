/**
 * =========================================================
 * SERIAL LOGGER - HEADER
 * =========================================================
 * Custom logging module to provide styled serial logs
 * and clean telemetry output.
 */

#pragma once
#include <Arduino.h>

/**
 * Initialize the serial logging module.
 */
void logInit();

/**
 * Print informational messages.
 */
void logInfo(const char* format, ...);

/**
 * Print warning messages.
 */
void logWarn(const char* format, ...);

/**
 * Print error messages.
 */
void logError(const char* format, ...);

/**
 * Print debug-specific messages if enabled in config.
 */
void logDebug(const char* format, ...);
