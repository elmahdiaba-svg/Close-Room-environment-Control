#include "Petelier_Polarity.h"
#include "config/config.h"

void startModeChange(bool heating, int pwmValue, int fanValue) {
  ledcWrite(mosfetPin, 0);        // cut power before flipping relay
  pendingModeChange = true;
  pendingHeating    = heating;
  pendingPWM        = pwmValue;
  pendingFan        = fanValue;
  relayChangeTime   = millis();
}

void stopAll() {
  ledcWrite(mosfetPin, 0);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  ledcWrite(fan1Pin, 0);
  ledcWrite(fan2Pin, 0);
  Serial.println("[RELAY] Stopped all outputs");
}

void handlePendingRelay() {
  if (!pendingModeChange) return;
  if (millis() - relayChangeTime < RELAY_DELAY) return;

  pendingModeChange = false;

  if (pendingHeating) {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, HIGH);
    Serial.println("[RELAY] Switched to HEATING");
  } else {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, LOW);
    Serial.println("[RELAY] Switched to COOLING");
  }

  ledcWrite(fan1Pin,   pendingFan);
  ledcWrite(fan2Pin,   pendingFan);
  ledcWrite(mosfetPin, pendingPWM);
}