/**
 * =========================================================
 * SERIAL LOGGER - IMPLEMENTATION
 * =========================================================
 * Implements formatted serial logging with ANSI/text headers.
 */

#include "logger.h"
#include "config.h"
#include <stdarg.h>

void logInit() {
#if DEBUG_SERIAL
    Serial.begin(DEBUG_BAUD_RATE);
    delay(500); // Allow UART to stabilize
    Serial.println();
    Serial.println("=========================================");
    Serial.println("   ESP32 LORA VEHICLE TRACKER ACTIVE     ");
    Serial.println("=========================================");
#endif
}

void logInfo(const char* format, ...) {
#if DEBUG_SERIAL
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.print("[INFO] ");
    Serial.println(buffer);
#endif
}

void logWarn(const char* format, ...) {
#if DEBUG_SERIAL
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.print("[WARN] ");
    Serial.println(buffer);
#endif
}

void logError(const char* format, ...) {
#if DEBUG_SERIAL
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.print("[ERROR] ");
    Serial.println(buffer);
#endif
}

void logDebug(const char* format, ...) {
#if DEBUG_SERIAL
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.print("[DEBUG] ");
    Serial.println(buffer);
#endif
}
