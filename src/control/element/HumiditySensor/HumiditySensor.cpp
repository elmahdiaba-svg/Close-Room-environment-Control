#include "HumiditySensor.h"
#include "config/config.h"
#include <DHT.h>

static DHT dht(HumiditySensor_PIN, DHT11);

// Helper: returns true when NO venting condition requires the flap to stay open
static bool canCloseFlap() {
    return !airQualityVentingActive && !thermalVentingActive;
}

void initHumiditySensor() {
    dht.begin();
    Serial.println("[DHT11] Humidity sensor ready.");
}

void readHumiditySensor() {
    float h = dht.readHumidity();
    if (isnan(h)) {
        Serial.println("[DHT11] WARNING: read failed.");
        return;
    }
    currentHumidity = h;
}

void handleHumiditySensor() {
    // Open flap when humidity exceeds the setpoint
    if (currentHumidity > humidityLimit && !humidityVentingActive) {
        humidityVentingActive = true;
        if (!flapOpen) {
            flapServo.write(90);
            delay(500);
            flapOpen = true;
        }
        digitalWrite(AirQualityFanPin, HIGH);   // ventilation fan ON
        Serial.printf("[DHT11] Humidity %.1f%% > limit %d%% -> flap + fan ON\n",
                      currentHumidity, humidityLimit);
    }

    // Close only when humidity recovered AND no other condition needs the flap
    if (currentHumidity < humidityLimit - 5.0f && humidityVentingActive) {
        humidityVentingActive = false;
        Serial.println("[DHT11] Humidity OK.");
        if (flapOpen && canCloseFlap()) {
            digitalWrite(AirQualityFanPin, LOW);
            flapServo.write(0);
            delay(500);
            flapOpen = false;
            Serial.println("[DHT11] Flap + fan closed.");
        }
    }
}
