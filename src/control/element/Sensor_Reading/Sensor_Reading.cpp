 #include "Sensor_Reading.h"
#include "config/config.h"

bool sensorDue() {
  return (millis() - lastSensorRead >= SENSOR_INTERVAL);
}

void readSensor() {
  lastSensorRead  = millis();

  float t = bmpInside.readTemperature();
  float p = bmpInside.readPressure() / 100.0F;
  insideTemperatureSensorOK = !isnan(t) && !isnan(p) &&
                   t > -40.0f && t < 85.0f &&
                   p > 300.0f && p < 1100.0f;
  if (insideTemperatureSensorOK) {
    insideTemp      = t;
    insidePressure  = p;
  } else {
    Serial.println("[BMP180-IN] WARNING: implausible reading — sensor may be disconnected.");
  }

  mq135Raw = analogRead(mq135Pin);          // FE
  airQualitySensorOK = !(mq135Raw == 0 || mq135Raw >= 4000);
  if (!airQualitySensorOK) {
    Serial.println("[MQ135] WARNING: out-of-range reading — sensor may be disconnected.");
  }

  const char* modeStr = currentMode == HEATING      ? "HEATING"
                      : currentMode == COOLING      ? "COOLING"
                      : currentMode == FREE_COOLING ? "FREE_COOL"
                      : currentMode == FREE_HEATING ? "FREE_HEAT"
                      : "IDLE";
  Serial.printf("[INSIDE] %.1f C°/ SP:%.1fC°+/-%.1f | %.1f hPa | Hum:%.1f %%/ SP:%d %% | %s | MQ135:%d/ SP:%d\n",
                insideTemp, TemperatureSp, TemperatureHysteresis, insidePressure, currentHumidity, humiditySp,
               modeStr, mq135Raw, airQualitySp);
}
