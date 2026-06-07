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

void handlePendingRelay() {
  // ---- fan kick-start: settle to target after 1 s at full speed ----
  static bool          fanKickActive = false;
  static unsigned long fanKickEnd    = 0;
  static int           fanKickTarget = 0;

  if (fanKickActive && millis() >= fanKickEnd) {
    fanKickActive = false;
    ledcWrite(MosfetFan1Ch, fanKickTarget);
    ledcWrite(MosfetFan2Ch, fanKickTarget);
    Serial.printf("[FAN] Kick done -> %d PWM\n", fanKickTarget);
  }

  // ---- relay / PWM switch ----
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

  ledcWrite(MosfetPetelierCh, pendingPetelierPWM);

  // Start fans: kick full for 1 s if target < 255, otherwise go direct
  if (pendingFanPWM < 255) {
    fanKickActive = true;
    fanKickEnd    = millis() + 1000;
    fanKickTarget = pendingFanPWM;
    ledcWrite(MosfetFan1Ch, 255);
    ledcWrite(MosfetFan2Ch, 255);
    Serial.printf("[FAN] Kick start: 255 PWM for 1 s -> %d PWM\n", pendingFanPWM);
  } else {
    ledcWrite(MosfetFan1Ch, pendingFanPWM);
    ledcWrite(MosfetFan2Ch, pendingFanPWM);
  }
}