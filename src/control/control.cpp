#include "control.h"

void runControlLoop() {
  handlePendingRelay();

  if (!sensorDue()) return;

  readSensor();
  runStateMachine();
}