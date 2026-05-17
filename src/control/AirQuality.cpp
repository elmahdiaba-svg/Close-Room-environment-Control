#include "AirQuality.h"
#include "config/config.h"

void handleAirQuality() {
  if (mq135Raw > airQualityLimit && !flapOpen) {
    flapServo.write(180);
    flapOpen = true;
    Serial.println("[AIR] Schlechte Luftqualität -> Klappe geöffnet");
  }

  if (mq135Raw < airQualityLimit - 300 && flapOpen) {
    flapServo.write(0);
    flapOpen = false;
    Serial.println("[AIR] Luftqualität wieder besser -> Klappe geschlossen");
  }
}
