#include "CurrentSensor.h"
#include "config/config.h"

// Hardware: ACS712-30A OUT → GPIO35 (direct, no voltage divider)
// ACS712 powered from 3.3V → quiescent output = 1.65V at 0A → ADC ≈ 2048.
// Sensitivity: 66 mV/A (30A version).

static const int   CAL_SAMPLES   = 500;
static const int   READ_SAMPLES  = 200;
static const float ADC_VREF      = 3.3f;
static const float ADC_MAX       = 4095.0f;
static const float DIVIDER_RATIO = 1.0f;        // no voltage divider
static const float ACS_V_PER_AMP = 0.066f;      // 66 mV/A for the 30A version

// Wide window — accepts any plausible quiescent value
static const int ZERO_RAW_MIN     = 500;
static const int ZERO_RAW_MAX     = 3800;
static const int ZERO_RAW_DEFAULT = 2048;        // 1.65V / 3.3V × 4095

static int zeroRaw = ZERO_RAW_DEFAULT;

void calibrateCurrentSensor() {
    delay(500);   // let ACS712 power rail stabilise

    // Warm-up reads — first few ESP32 ADC samples can be unreliable
    for (int i = 0; i < 20; i++) {
        analogRead(CourantSensor_PIN);
        delayMicroseconds(100);
    }

    Serial.printf("[ACS712] single raw read before cal: %d\n", analogRead(CourantSensor_PIN));

    long sum = 0;
    for (int i = 0; i < CAL_SAMPLES; i++) {
        sum += analogRead(CourantSensor_PIN);
        delayMicroseconds(200);
    }
    int raw = (int)(sum / CAL_SAMPLES);

    if (raw < ZERO_RAW_MIN || raw > ZERO_RAW_MAX) {
        Serial.printf("[ACS712] WARNING: zeroRaw=%d is out of range [%d-%d]."
                      " Using default %d.\n",
                      raw, ZERO_RAW_MIN, ZERO_RAW_MAX, ZERO_RAW_DEFAULT);
        zeroRaw = ZERO_RAW_DEFAULT;
    } else {
        zeroRaw = raw;
        Serial.printf("[ACS712] calibrated — zeroRaw=%d  (%.3f V at pin)\n",
                      zeroRaw, zeroRaw * ADC_VREF / ADC_MAX);
    }
}

void handleCurrentSensor() {
    long sum = 0;
    for (int i = 0; i < READ_SAMPLES; i++) {
        sum += analogRead(CourantSensor_PIN);
    }
    float avgRaw   = (float)sum / READ_SAMPLES;
    float deltaRaw = avgRaw - (float)zeroRaw;
    float deltaV   = deltaRaw * (ADC_VREF / ADC_MAX) / DIVIDER_RATIO;
    currentAmps    = fabsf(deltaV / ACS_V_PER_AMP);
}
