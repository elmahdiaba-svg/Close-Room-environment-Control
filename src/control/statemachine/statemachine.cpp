#include "statemachine.h"
#include "control/element/Petelier_Polarity/Petelier_Polarity.h"
#include "config/config.h"

void runStateMachine() {
  switch (currentMode) {

    case IDLE:
      if (currentTemp <= SP - DELTA) {
        startModeChange(true, peltierPWM, fanPower);
        currentMode = HEATING;
        Serial.println(">>> HEATING ON");
      } else if (currentTemp >= SP + DELTA) {
        startModeChange(false, peltierPWM, fanPower);
        currentMode = COOLING;
        Serial.println(">>> COOLING ON");
      }
      break;

    case HEATING:
      if (currentTemp >= SP) {
        stopAll();
        currentMode = IDLE;
        Serial.println(">>> HEATING OFF");
      }
      break;

    case COOLING:
      if (currentTemp <= SP) {
        stopAll();
        currentMode = IDLE;
        Serial.println(">>> COOLING OFF");
      }
      break;
  }
}