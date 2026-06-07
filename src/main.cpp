#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include "config/config.h"
#include "control/control.h"
#include "web/webhandler/webhandler.h"

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  pinMode(HeatingRelay, OUTPUT);
  pinMode(CoolingRelay, OUTPUT);
  digitalWrite(HeatingRelay, HIGH);
  digitalWrite(CoolingRelay, HIGH);

  pinMode(mq135Pin, INPUT);          // FE
  pinMode(CourantSensor_PIN, INPUT);        // ACS712-30A current sensor
  pinMode(AirQualityFanPin, OUTPUT); // 3.3V air quality fan
  digitalWrite(AirQualityFanPin, LOW);

  // Drive all MOSFETs to 0 BEFORE calibrating the current sensor,
  // so no load current corrupts the zero-point calibration.
  ledcSetup(MosfetPetelierCh, 20000, 8);
  ledcSetup(MosfetFan1Ch, 20000, 8);
  ledcSetup(MosfetFan2Ch, 20000, 8);

  ledcAttachPin(MosfetPetelierPin, MosfetPetelierCh);
  ledcAttachPin(MosfetFan1Pin,   MosfetFan1Ch);
  ledcAttachPin(MosfetFan2Pin,   MosfetFan2Ch);

  ledcWrite(MosfetPetelierCh, 0);
  ledcWrite(MosfetFan1Ch, 0);
  ledcWrite(MosfetFan2Ch, 0);

  initHardwareIO();                 // status LEDs, NOT-AUS button + relay
  calibrateCurrentSensor();        // zero the sensor now that all loads are off

  flapServo.setPeriodHertz(50);                        // standard 50 Hz servo signal
  bool servoOK = flapServo.attach(servoPin, 500, 2400); // min/max pulse width µs
  Serial.printf("[SERVO] attach(%d) -> %s\n", servoPin, servoOK ? "OK" : "FAILED");
  flapServo.write(0);
  Serial.println("[SERVO] write(0) sent — flap closed");
  delay(1000);
  flapServo.write(90);
  Serial.println("[SERVO] write(90) sent — flap open test");
  delay(2000);
  flapServo.write(0);
  Serial.println("[SERVO] write(0) sent — back to closed");
  delay(500);

  mq135Raw = analogRead(mq135Pin);
  Serial.printf("[mq135] value: %d\n", mq135Raw);

  if (mq135Raw == 0 || mq135Raw >= 4000) {
    Serial.println("ERROR: mq135 not found!");
  } else {
    Serial.println("mq135 ok");
  }

  if (!bmpInside.begin()) {
    Serial.println("ERROR: BMP180 (inside) not found!");
    while (1);
  }
  Serial.println("BMP180 (inside) ready.");

  initoutsideTemperatureSensor();             // Wire1 + outside BMP180 (non-fatal if missing)
  initHumiditySensor();            // DHT11 on GPIO13

  if (!WiFi.softAP(ssid, password)) {
    Serial.println("AP failed!");
    while (1);
  }
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/",        handleRoot);
  server.on("/status",  handleStatus);
  server.on("/set",     handleSet);
  server.on("/notaus",  handleNotAus);
  server.on("/login",   handleLogin);
  server.onNotFound([]() {
    server.send(404, "text/plain", "404: Not Found");
  });

  server.begin();
  Serial.println("Server started");
}

// -------- LOOP --------
void loop() {
  server.handleClient();
  runControlLoop();
}
