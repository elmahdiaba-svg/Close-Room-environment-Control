 #include "Sensor_Reading.h"
#include "config/config.h"

bool sensorDue() {
  return (millis() - lastSensorRead >= SENSOR_INTERVAL);
}

void readSensor() {
  lastSensorRead  = millis();
  currentTemp     = bmp.readTemperature();
  currentPressure = bmp.readPressure() / 100.0F;

  mq135Raw = analogRead(mq135Pin);          // FE

  Serial.printf("[SENSOR] %.1f °C | %.1f hPa | SP:%.1f+/-%.1f | %s\n" | MQ135: %d,    //FE
                currentTemp, currentPressure, SP, DELTA, mq135Raw,                   // FE
                currentMode == HEATING ? "HEATING" :
                currentMode == COOLING ? "COOLING" : "IDLE");
}
