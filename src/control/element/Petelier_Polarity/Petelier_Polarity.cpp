#include "Petelier_Polarity.h"
#include "config/config.h"

void startModeChange(bool heating, int pwmValue, int fanValue) {
  ledcWrite(MosfetPetelierCh, 0);         // cut power before flipping relay
  pendingModeChange = true;
  pendingHeating    = heating;
  pendingPetelierPWM        = pwmValue;
  pendingFanPWM        = fanValue;
  relayChangeTime   = millis();
}

void stopAll() {
  ledcWrite(MosfetPetelierCh, 0);
  digitalWrite(HeatingRelay, HIGH);
  digitalWrite(CoolingRelay, HIGH);
  ledcWrite(MosfetFan1Ch, 0);
  ledcWrite(MosfetFan2Ch, 0);
  Serial.println("[RELAY] Stopped all outputs");
}

void handlePendingRelay() {
  if (!pendingModeChange) return;
  if (millis() - relayChangeTime < RELAY_DELAY) return;

  pendingModeChange = false;

  if (pendingHeating) {
    digitalWrite(HeatingRelay, LOW);
    digitalWrite(CoolingRelay, HIGH);
    Serial.println("[RELAY] Switched to HEATING");
  } else {
    digitalWrite(HeatingRelay, HIGH);
    digitalWrite(CoolingRelay, LOW);
    Serial.println("[RELAY] Switched to COOLING");
  }

  ledcWrite(MosfetFan1Ch,   pendingFanPWM);
  ledcWrite(MosfetFan2Ch,   pendingFanPWM);
  ledcWrite(MosfetPetelierCh, pendingPetelierPWM);
}