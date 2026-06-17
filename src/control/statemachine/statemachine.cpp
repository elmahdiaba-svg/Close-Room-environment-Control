#include "statemachine.h"
#include "control/element/Petelier_Polarity/Petelier_Polarity.h"
#include "config/config.h"

void stopAll() {
  ledcWrite(MosfetPetelierCh, 0);
  digitalWrite(HeatingRelay, HIGH);
  digitalWrite(CoolingRelay, HIGH);
  ledcWrite(MosfetFan1Ch, 0);
  ledcWrite(MosfetFan2Ch, 0);
  Serial.println("[RELAY] Stopped all outputs");
}

// ---- helpers for flap control during thermal venting ----
static void openFlapForVenting() {
    if (!flapOpen) {
        flapServo.write(90);
        delay(500);
        flapOpen = true;
    }
    thermalVentingActive = true;
}

static void closeFlapAfterVenting() {
    thermalVentingActive = false;
    if (flapOpen) {
        flapServo.write(0);
        delay(500);
        flapOpen = false;
    }
}

// ---- state machine ----

void runStateMachine() {
  // Air quality is highest priority — shut down and freeze temperature control while venting
  static bool prevAqVenting = false;

  // AQ or humidity venting = highest priority: shut down temperature control
  bool overrideActive = airQualityVentingActive || humidityVentingActive;

  if (overrideActive) {
    if (!prevAqVenting) {
      stopAll();                        // Peltier + fans → 0
      thermalVentingActive = false;     // cancel any free-venting state
      currentMode          = IDLE;      // re-evaluate from scratch when override ends
      prevAqVenting        = true;
      Serial.println("[SM] Venting override (AQ/Humidity) — Peltier + fans OFF.");
    }
    return;
  }

  if (prevAqVenting) {
    prevAqVenting = false;
    Serial.println("[SM] Venting override ended — temperature control resumed.");
  }

  switch (currentMode) {

    // ----------------------------------------------------------------
    case IDLE:
      if (insideTemp <= TemperatureSp - TemperatureHysteresis) {
        // Room too cold — need heating
        if (outsideTemperatureSensorOK && outsideTemp > TemperatureSp) {
          // Outside warmer than setpoint → let warm air in for free
          openFlapForVenting();
          currentMode = FREE_HEATING;
          Serial.printf(">>> FREE HEATING: outside %.1f C > SP %.1f C\n",
                        outsideTemp, TemperatureSp);
        } else {
          startModeChange(true, peltierPower, fanPower);
          currentMode = HEATING;
          Serial.println(">>> HEATING ON");
        }
      } else if (insideTemp >= TemperatureSp + TemperatureHysteresis) {
        // Room too hot — need cooling
        if (outsideTemperatureSensorOK && outsideTemp < TemperatureSp) {
          // Outside cooler than setpoint → let cool air in for free
          openFlapForVenting();
          currentMode = FREE_COOLING;
          Serial.printf(">>> FREE COOLING: outside %.1f C < SP %.1f C\n",
                        outsideTemp, TemperatureSp);
        } else {
          startModeChange(false, peltierPower, fanPower);
          currentMode = COOLING;
          Serial.println(">>> COOLING ON");
        }
      }
      break;

    // ----------------------------------------------------------------
    case HEATING:
      if (insideTemp >= TemperatureSp) {
        stopAll();
        currentMode = IDLE;
        Serial.println(">>> HEATING OFF");
      } else if (outsideTemperatureSensorOK && outsideTemp > TemperatureSp) {
        // Outside became warmer than setpoint — switch to free heating
        stopAll();
        openFlapForVenting();
        currentMode = FREE_HEATING;
        Serial.printf(">>> HEATING -> FREE HEATING: outside %.1f C > SP %.1f C\n",
                      outsideTemp, TemperatureSp);
      }
      break;

    // ----------------------------------------------------------------
    case COOLING:
      if (insideTemp <= TemperatureSp) {
        stopAll();
        currentMode = IDLE;
        Serial.println(">>> COOLING OFF");
      } else if (outsideTemperatureSensorOK && outsideTemp < TemperatureSp) {
        // Outside became cooler than setpoint — switch to free cooling
        stopAll();
        openFlapForVenting();
        currentMode = FREE_COOLING;
        Serial.printf(">>> COOLING -> FREE COOLING: outside %.1f C < SP %.1f C\n",
                      outsideTemp, TemperatureSp);
      }
      break;

    // ----------------------------------------------------------------
    case FREE_COOLING:
      if (insideTemp <= TemperatureSp) {
        // Room reached setpoint — close flap, done
        closeFlapAfterVenting();
        currentMode = IDLE;
        Serial.println(">>> FREE COOLING done -> IDLE");
      } else if (!outsideTemperatureSensorOK || outsideTemp >= insideTemp) {
        // Outside no longer cooler than inside — fall back to Peltier
        closeFlapAfterVenting();
        startModeChange(false, peltierPower, fanPower);
        currentMode = COOLING;
        Serial.println(">>> FREE COOLING -> COOLING (outside no longer effective)");
      }
      break;

    // ----------------------------------------------------------------
    case FREE_HEATING:
      if (insideTemp >= TemperatureSp) {
        // Room reached setpoint — close flap, done
        closeFlapAfterVenting();
        currentMode = IDLE;
        Serial.println(">>> FREE HEATING done -> IDLE");
      } else if (!outsideTemperatureSensorOK || outsideTemp <= insideTemp) {
        // Outside no longer warmer than inside — fall back to Peltier
        closeFlapAfterVenting();
        startModeChange(true, peltierPower, fanPower);
        currentMode = HEATING;
        Serial.println(">>> FREE HEATING -> HEATING (outside no longer effective)");
      }
      break;
  }
}
