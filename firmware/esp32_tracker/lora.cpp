/**
 * =========================================================
 * LORA MODULE - IMPLEMENTATION
 * =========================================================
 * Uses SPI and the sandeepmistry LoRa library to control the SX1278 transceiver.
 */

#include "lora.h"
#include "config.h"
#include "logger.h"
#include <SPI.h>
#include <LoRa.h>

bool connectLoRa() {
    logInfo("Configuring SPI for LoRa: SCK=%d, MISO=%d, MOSI=%d, NSS=%d", 
            LORA_SCK_PIN, LORA_MISO_PIN, LORA_MOSI_PIN, LORA_NSS_PIN);
    
    // Set pins
    LoRa.setPins(LORA_NSS_PIN, LORA_RST_PIN, LORA_DIO0_PIN);
    
    logInfo("Starting LoRa transceiver at %.1f MHz...", LORA_FREQUENCY / 1E6);
    if (!LoRa.begin(LORA_FREQUENCY)) {
        logError("LoRa transceiver initialization failed!");
        return false;
    }
    
    // Apply configurations
    LoRa.setTxPower(LORA_TX_POWER);
    LoRa.setSpreadingFactor(LORA_SPREADING_FACTOR);
    LoRa.setSignalBandwidth(LORA_BANDWIDTH);
    
    // Set sync word to segregate traffic from other networks (0xF3 is common for P2P)
    LoRa.setSyncWord(0xF3);
    
    logInfo("LoRa P2P transceiver connected successfully.");
    return true;
}

bool sendPayload(uint8_t *payload, uint8_t length) {
    logInfo("Sending LoRa P2P package (%d bytes)...", length);
    
    unsigned long startTime = millis();
    
    LoRa.beginPacket();
    LoRa.write(payload, length);
    int status = LoRa.endPacket(); // Blocks until transmission is done
    
    if (status == 1) {
        unsigned long duration = millis() - startTime;
        logInfo("LoRa P2P package transmitted successfully in %lu ms", duration);
        return true;
    } else {
        logError("LoRa transmission failed!");
        return false;
    }
}

void sleepLoRa() {
    logInfo("Putting LoRa module to sleep mode...");
    LoRa.sleep();
}
