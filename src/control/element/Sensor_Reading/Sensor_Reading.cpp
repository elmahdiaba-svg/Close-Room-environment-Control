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

  const char* modeStr = currentMode == HEATING      ? "HEATING"
                      : currentMode == COOLING      ? "COOLING"
                      : currentMode == FREE_COOLING ? "FREE_COOL"
                      : currentMode == FREE_HEATING ? "FREE_HEAT"
                      : "IDLE";
  Serial.printf("[INSIDE] %.1f C°/ SP:%.1fC°+/-%.1f | %.1f hPa | Hum:%.1f %%/ SP:%d %% | %s | MQ135:%d/ SP:%d\n",
                currentTemp, SP, DELTA, currentPressure, currentHumidity, humidityLimit,
               modeStr, mq135Raw, airQualityLimits);
}
