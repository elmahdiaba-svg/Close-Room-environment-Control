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

  ledcSetup(0, 20000, 8);
  ledcSetup(1, 20000, 8);
  ledcSetup(2, 20000, 8);

  ledcAttachPin(MosfetPetelierPin, MosfetPetelierCh);
  ledcAttachPin(MosfetFan1Pin,   MosfetFan1Ch);
  ledcAttachPin(MosfetFan2Pin,   MosfetFan2Ch);

  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);

  if (!bmp.begin()) {
    Serial.println("ERROR: BMP180 not found!");
    while (1);
  }
  Serial.println("BMP180 Ready.");

  if (!WiFi.softAP(ssid, password)) {
    Serial.println("AP failed!");
    while (1);
  }
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/",       handleRoot);
  server.on("/status", handleStatus);
  server.on("/set",    handleSet);
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