#include "CurrentSensor.h"
#include "config/config.h"

// Hardware: ACS712-05A OUT → 2/3 voltage divider → GPIO35
// Uses analogReadMilliVolts() for ESP32 built-in ADC linearity correction.
// Divider 2/3: quiescent = 2.5V * 2/3 = 1667 mV | sensitivity = 185mV/A * 2/3 = 123 mV/A (theoretical)
// Empirical value (139.1) accounts for ESP32 ADC nonlinearity at this voltage range.

static const int   READ_SAMPLES   = 500;
static const float ACS_MV_PER_AMP = 125.9f;  // empirical — real GPIO slope ~65 mV/A, ADC nonlinearity inflates delta

static float zeroMV     = 1667.0f;  // 2.5V * 2/3 — overwritten by calibration at boot
static bool  acsPowered = false;

void calibrateCurrentSensor() {
    for (int i = 0; i < 200; i++) {
        analogReadMilliVolts(currentSensor_PIN);
        delayMicroseconds(100);
    }
    long sum = 0;
    for (int i = 0; i < READ_SAMPLES; i++) {
        sum += analogReadMilliVolts(currentSensor_PIN);
        delayMicroseconds(100);
    }
    float mv = (float)sum / READ_SAMPLES;
    Serial.printf("[ACS712] calibration read: %.1f mV\n", mv);
    if (mv > 1200.0f && mv < 2000.0f) {
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
        long sum = 0;
        for (int i = 0; i < READ_SAMPLES; i++) sum += analogReadMilliVolts(currentSensor_PIN);
        float mv = (float)sum / READ_SAMPLES;
        if (mv > 1200.0f && mv < 2000.0f) {
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
        sum += analogReadMilliVolts(currentSensor_PIN);
    }
    float avgMV = (float)sum / READ_SAMPLES;
    if (avgMV < 900.0f) {
        // VCC lost — reset so next cycle re-calibrates when power returns
        acsPowered  = false;
        currentAmps = 0.0f;
        return;
    }
    float deltaMV = avgMV - zeroMV;
    currentAmps   = fabsf(deltaMV / ACS_MV_PER_AMP);
}
