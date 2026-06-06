#include "outsideTemperatureSensor.h"
#include "config/config.h"
#include <Wire.h>

// Outside BMP180 is connected to the ESP32's second I2C bus (Wire1).
// Wire1 SDA = GPIO16, SCL = GPIO17  (defined as OutsideTemperatureSDA_PIN / OutsideTemperatureSCL_PIN in config.h)
// This avoids I2C address conflict with the inside BMP180 (both use 0x77).

void initoutsideTemperatureSensor() {
    Wire1.begin(OutsideTemperatureSDA_PIN, OutsideTemperatureSCL_PIN);

    if (!bmpOutside.begin(BMP085_STANDARD, &Wire1)) {
        outsideTemperatureSensorOK = false;
        Serial.println("[OUTSIDE] BMP180 not found — natural ventilation disabled.");
    } else {
        outsideTemperatureSensorOK = true;
        outsideTemp     = bmpOutside.readTemperature();
        outsidePressure = bmpOutside.readPressure() / 100.0F;
        Serial.printf("[OUTSIDE] BMP180 ready. %.1f °C | %.1f hPa\n",
                      outsideTemp, outsidePressure);
    }
}

void readoutsideTemperatureSensor() {
    if (!outsideTemperatureSensorOK) return;

    outsideTemp     = bmpOutside.readTemperature();
    outsidePressure = bmpOutside.readPressure() / 100.0F;

    Serial.printf("[OUTSIDE] %.1f °C | %.1f hPa\n", outsideTemp, outsidePressure);
}
