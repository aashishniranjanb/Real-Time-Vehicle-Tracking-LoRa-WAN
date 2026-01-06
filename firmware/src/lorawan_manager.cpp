/**
 * =========================================================
 * LORAWAN MANAGER - IMPLEMENTATION
 * =========================================================
 * Uses LMIC library for LoRaWAN Class A operation
 * ABP activation for prototype simplicity
 * 
 * NOTE: Replace keys with your TTN device credentials
 */

#include "lorawan_manager.h"
#include "config.h"
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

// =========================================================
// LORAWAN SESSION KEYS (ABP)
// =========================================================
// NOTE: These are placeholder keys. Replace with your TTN credentials.
// For production, use OTAA and secure key storage.

static const u1_t PROGMEM APPSKEY[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const u1_t PROGMEM NWKSKEY[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const u4_t DEVADDR = 0x26011111;  // Replace with your device address

// =========================================================
// LMIC PIN MAPPING (ESP32 + SX127x)
// =========================================================
const lmic_pinmap lmic_pins = {
    .nss  = LORA_NSS_PIN,
    .rxtx = LMIC_UNUSED_PIN,
    .rst  = LORA_RST_PIN,
    .dio  = {LORA_DIO0_PIN, LORA_DIO1_PIN, LORA_DIO2_PIN}
};

// =========================================================
// LMIC EVENT CALLBACK
// =========================================================
void onEvent(ev_t ev) {
    switch (ev) {
        case EV_TXCOMPLETE:
            Serial.println("[LORA] TX complete");
            if (LMIC.txrxFlags & TXRX_ACK) {
                Serial.println("[LORA] ACK received");
            }
            if (LMIC.dataLen) {
                Serial.print("[LORA] Received ");
                Serial.print(LMIC.dataLen);
                Serial.println(" bytes downlink");
            }
            break;

        case EV_TXSTART:
            Serial.println("[LORA] TX start");
            break;

        case EV_JOIN_TXCOMPLETE:
            Serial.println("[LORA] Join TX complete (no JoinAccept)");
            break;

        default:
            Serial.print("[LORA] Event: ");
            Serial.println((unsigned)ev);
            break;
    }
}

// =========================================================
// INITIALIZATION
// =========================================================
void LoRaWANManager::begin() {
    
    Serial.println("[LORA] Initializing LoRaWAN stack...");

    // Initialize LMIC
    os_init();
    LMIC_reset();

    // Set session keys for ABP
    #ifdef PROGMEM
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession(0x13, DEVADDR, nwkskey, appskey);
    #else
    LMIC_setSession(0x13, DEVADDR, NWKSKEY, APPSKEY);
    #endif

    // Configure for IN865 (India) or change to your region
    // Disable all channels except those available in your region
    
    // Disable link check validation (saves power)
    LMIC_setLinkCheckMode(0);

    // Set data rate and transmit power
    // DR_SF7 = highest data rate, lowest range
    // DR_SF12 = lowest data rate, highest range
    LMIC_setDrTxpow(DR_SF7, 14);

    // Disable ADR (Adaptive Data Rate) for prototype
    LMIC_setAdrMode(0);

    Serial.println("[LORA] LoRaWAN initialized (ABP mode)");
}

// =========================================================
// PAYLOAD ENCODING
// =========================================================
uint8_t LoRaWANManager::encodePayload(double lat, double lon, uint8_t *buffer) {
    
    /*
     * Compact GPS Encoding:
     * - Latitude:  4 bytes (fixed-point, *1e6)
     * - Longitude: 4 bytes (fixed-point, *1e6)
     * 
     * Total: 8 bytes (fits easily in LoRaWAN payload)
     * 
     * Decoder (TTN side):
     *   lat = ((b[0]<<24) | (b[1]<<16) | (b[2]<<8) | b[3]) / 1e6
     *   lon = ((b[4]<<24) | (b[5]<<16) | (b[6]<<8) | b[7]) / 1e6
     */

    int32_t lat_enc = (int32_t)(lat * 1e6);
    int32_t lon_enc = (int32_t)(lon * 1e6);

    // Encode latitude (big-endian)
    buffer[0] = (lat_enc >> 24) & 0xFF;
    buffer[1] = (lat_enc >> 16) & 0xFF;
    buffer[2] = (lat_enc >> 8)  & 0xFF;
    buffer[3] = lat_enc & 0xFF;

    // Encode longitude (big-endian)
    buffer[4] = (lon_enc >> 24) & 0xFF;
    buffer[5] = (lon_enc >> 16) & 0xFF;
    buffer[6] = (lon_enc >> 8)  & 0xFF;
    buffer[7] = lon_enc & 0xFF;

    Serial.print("[LORA] Encoded payload: ");
    for (int i = 0; i < 8; i++) {
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    return 8;  // Payload length
}

// =========================================================
// SEND UPLINK
// =========================================================
bool LoRaWANManager::sendUplink(uint8_t *data, uint8_t length) {
    
    // Check if previous transmission is still pending
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println("[LORA] TX pending, cannot send");
        return false;
    }

    // Queue unconfirmed uplink on port 1
    LMIC_setTxData2(1, data, length, 0);
    
    Serial.println("[LORA] Uplink queued");

    // Run LMIC event loop until TX completes
    // Note: In production, this could be event-driven
    unsigned long start = millis();
    while (millis() - start < 10000) {
        os_runloop_once();
        
        // Check if TX completed
        if (!(LMIC.opmode & OP_TXRXPEND)) {
            return true;
        }
        
        delay(10);
    }

    Serial.println("[LORA] TX timeout");
    return false;
}

// =========================================================
// UTILITY FUNCTIONS
// =========================================================
bool LoRaWANManager::isBusy() {
    return (LMIC.opmode & OP_TXRXPEND) != 0;
}

void LoRaWANManager::update() {
    os_runloop_once();
}
