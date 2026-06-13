#include "CurrentSensor.h"
#include "config/config.h"

// Hardware: ACS712-30A OUT → 2/3 voltage divider → GPIO35
// Uses analogReadMilliVolts() for ESP32 built-in ADC linearity correction.
// Calibrated: zeroMV=842 mV
//   1.51A → |deltaMV|=93.5  → 62.0 mV/A
//   2.37A → |deltaMV|=164.9 → 69.6 mV/A  ← active
static const int   READ_SAMPLES   = 500;
static const float ACS_MV_PER_AMP = 65.6f;

static float zeroMV      = 842.0f;
static bool  acsPowered  = false;  // true only when calibration confirmed VCC is present

void calibrateCurrentSensor() {
    for (int i = 0; i < 200; i++) {
        analogReadMilliVolts(CourantSensor_PIN);
        delayMicroseconds(100);
    }
    long sum = 0;
    for (int i = 0; i < READ_SAMPLES; i++) {
        sum += analogReadMilliVolts(CourantSensor_PIN);
        delayMicroseconds(100);
    }
    float mv = (float)sum / READ_SAMPLES;
    Serial.printf("[ACS712] calibration read: %.1f mV\n", mv);
    if (mv > 500.0f && mv < 1200.0f) {
        zeroMV     = mv;
        acsPowered = true;
        Serial.printf("[ACS712] calibrated — zeroMV=%.1f mV\n", zeroMV);
    } else {
        acsPowered = false;
        Serial.printf("[ACS712] calibration skipped (%.1f mV) — sensor not powered\n", mv);
    }
}

void handleCurrentSensor() {
    if (!acsPowered) {
        // retry calibration each cycle until sensor VCC is present
        long sum = 0;
        for (int i = 0; i < READ_SAMPLES; i++) sum += analogReadMilliVolts(CourantSensor_PIN);
        float mv = (float)sum / READ_SAMPLES;
        if (mv > 500.0f && mv < 1200.0f) {
            zeroMV     = mv;
            acsPowered = true;
            Serial.printf("[ACS712] sensor detected — zeroMV=%.1f mV\n", zeroMV);
        } else {
            currentAmps = 0.0f;
            return;
        }
    }
    long sum = 0;
    for (int i = 0; i < READ_SAMPLES; i++) {
        sum += analogReadMilliVolts(CourantSensor_PIN);
    }
    float avgMV = (float)sum / READ_SAMPLES;
    if (avgMV < 400.0f) {
        // VCC lost — reset so next cycle re-calibrates when power returns
        acsPowered  = false;
        currentAmps = 0.0f;
        return;
    }
    float deltaMV = avgMV - zeroMV;
    currentAmps   = fabsf(deltaMV / ACS_MV_PER_AMP);
}
