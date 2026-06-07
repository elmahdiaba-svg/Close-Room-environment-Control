#include "AirQuality.h"
#include "config/config.h"

// Flap closes only when ALL venting conditions are clear
static bool canCloseFlap() {
    return !thermalVentingActive && !humidityVentingActive;
}

void handleAirQuality() {
  // Open flap + start fan — takes priority over temperature control
  if (mq135Raw > airQualitySp && !airQualityVentingActive) {
    airQualityVentingActive = true;
    if (!flapOpen) {
        flapServo.write(90);
        delay(1000);
        flapOpen = true;
    }
    digitalWrite(AirQualityFanPin, HIGH);
    Serial.println("[AIR] Schlechte Luftqualität -> Klappe + Lüfter AN (Priorität)");
  }

  // Air quality recovered — clear flag, close flap only if nothing else needs it
  if (mq135Raw < airQualitySp - airQualityHysteresis && airQualityVentingActive) {
    airQualityVentingActive = false;
    Serial.println("[AIR] Luftqualität OK.");
    if (flapOpen && canCloseFlap()) {
      digitalWrite(AirQualityFanPin, LOW);
      flapServo.write(0);
      delay(1000);
      flapOpen = false;
      Serial.println("[AIR] Lüfter AUS + Klappe geschlossen.");
    }
  }
}
