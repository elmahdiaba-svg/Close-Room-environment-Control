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

<<<<<<< HEAD
<<<<<<< HEAD
  pinMode(HeatingRelay, OUTPUT);
  pinMode(CoolingRelay, OUTPUT);
  digitalWrite(HeatingRelay, HIGH);
  digitalWrite(CoolingRelay, HIGH);
=======
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
=======
  pinMode(HeatingRelay, OUTPUT);
  pinMode(CoolingRelay, OUTPUT);
>>>>>>> 0522460 (air quality feature)
  pinMode(mq135Pin, INPUT);        // FE
  digitalWrite(HeatingRelay, HIGH);
  digitalWrite(CoolingRelay, HIGH);

<<<<<<< HEAD
  flapServo.attach(servoPin);      // FE
  flapServo.write(0);              // Klappe geschlossen; FE
>>>>>>> 85be5bb (Update main.cpp)
=======
  ledcSetup(MosfetPetelierCh, 20000, 8);
  ledcSetup(MosfetFan1Ch, 20000, 8);
  ledcSetup(MosfetFan2Ch, 20000, 8);
>>>>>>> 0522460 (air quality feature)

  ledcAttachPin(MosfetPetelierPin, MosfetPetelierCh);
  ledcAttachPin(MosfetFan1Pin,   MosfetFan1Ch);
  ledcAttachPin(MosfetFan2Pin,   MosfetFan2Ch);

<<<<<<< HEAD
  ledcAttachPin(MosfetPetelierPin, MosfetPetelierCh);
  ledcAttachPin(MosfetFan1Pin,   MosfetFan1Ch);
  ledcAttachPin(MosfetFan2Pin,   MosfetFan2Ch);
=======
  ledcWrite(MosfetPetelierCh, 0);
  ledcWrite(MosfetFan1Ch, 0);
  ledcWrite(MosfetFan2Ch, 0);

  flapServo.attach(servoPin); // FE 
  flapServo.write(0);         // Klappe geschlossen; FE
  delay(1000);   

  mq135Raw = analogRead(mq135Pin);
  Serial.printf("[mq135] value: %d\n", mq135Raw);

  if (mq135Raw == 0 || mq135Raw >= 4000 ) {
    Serial.println("ERROR: mq135 not found!");
  } else 
  {
    Serial.println("mq135 ok");
  } 
>>>>>>> 0522460 (air quality feature)


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
