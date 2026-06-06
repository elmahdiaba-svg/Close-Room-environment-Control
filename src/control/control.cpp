#include "control.h"

void runControlLoop() {
  handlePendingRelay();

  if (!sensorDue()) return;

  readSensor();
  readoutsideTemperatureSensor();
  readHumiditySensor();
  handleCurrentSensor();
  handleAirQuality();               // FE
  handleHumiditySensor();
  runStateMachine();
}
