#include "control.h"
#include "config/config.h"

void runControlLoop() {
  readNotAusButton();    // always: debounced hardware NOT-AUS button
  updateStatusLEDs();    // always: mirror mode + emergency state to physical LEDs + relay

  if (emergencyStop) return;   // NOT-AUS: all control suspended

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
