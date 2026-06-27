/**
 * =========================================================
 * GPS MODULE - IMPLEMENTATION
 * =========================================================
 * Uses TinyGPS++ to parse NMEA stream from HardwareSerial.
 */

#include "gps.h"
#include "config.h"
#include "logger.h"
#include <TinyGPS++.h>
#include <HardwareSerial.h>

static TinyGPSPlus gps;
static HardwareSerial GPSSerial(1); // Use UART1 for GPS

static double   _latitude = 0.0;
static double   _longitude = 0.0;
static float    _speed = 0.0;
static uint8_t  _satellites = 0;
static float    _hdop = 99.0;

bool initGPS() {
    logInfo("Initializing GPS module on UART1 (RX=%d, TX=%d, Baud=%d)", GPS_RX_PIN, GPS_TX_PIN, GPS_BAUD_RATE);
    GPSSerial.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    return true;
}

bool getLocation() {
    logInfo("Acquiring GPS fix...");
    unsigned long start = millis();
    bool fixAcquired = false;

    // Clear serial buffers
    while(GPSSerial.available()) {
        GPSSerial.read();
    }

    while (millis() - start < GPS_FIX_TIMEOUT_MS) {
        while (GPSSerial.available() > 0) {
            char c = GPSSerial.read();
            gps.encode(c);
        }

        if (gps.location.isUpdated() && gps.location.isValid()) {
            _latitude = gps.location.lat();
            _longitude = gps.location.lng();
            _speed = gps.speed.kmph();
            _satellites = gps.satellites.value();
            _hdop = gps.hdop.hdop();

            // Check fix quality using HDOP
            if (_hdop <= HDOP_MAX_THRESHOLD) {
                logInfo("GPS lock acquired! Lat: %.6f, Lon: %.6f, Sats: %d, HDOP: %.2f", 
                        _latitude, _longitude, _satellites, _hdop);
                fixAcquired = true;
                break;
            } else {
                logWarn("GPS coordinates updated but HDOP %.2f too high, waiting for better signal...", _hdop);
            }
        }
        delay(10); // Small yield
    }

    if (!fixAcquired) {
        logError("GPS Fix Timeout! No valid signal acquired.");
    }
    
    return fixAcquired;
}

double latitude() {
    return _latitude;
}

double longitude() {
    return _longitude;
}

float speed() {
    return _speed;
}

uint8_t satellites() {
    return _satellites;
}

float hdop() {
    return _hdop;
}
