#include "AirQuality.h"
#include "config/config.h"

void handleAirQuality() {
  if (mq135Raw > airQualityLimits && !flapOpen) {
    flapServo.write(90);
    delay(1000);
    flapOpen = true;
    Serial.println("[AIR] Schlechte Luftqualität -> Klappe geöffnet");
  }

  if (mq135Raw < airQualityLimits - 300 && flapOpen) {
    flapServo.write(0);
    delay(1000);
    flapOpen = false;
    Serial.println("[AIR] Luftqualität wieder besser -> Klappe geschlossen");
  }
}
