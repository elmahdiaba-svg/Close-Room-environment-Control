#pragma once
#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// -------- PIN CONFIG --------
<<<<<<< HEAD
<<<<<<< HEAD
const int HeatingRelay      = 2;
const int CoolingRelay      = 5;
const int MosfetPetelierPin = 27;
const int MosfetFan1Pin     = 26;
const int MosfetFan2Pin     = 25;

// -------- LEDC CHANNELS (ledcWrite takes channel, not pin) --------
const int MosfetPetelierCh = 0;
const int MosfetFan1Ch     = 1;
const int MosfetFan2Ch     = 2;

=======
const int relay1    = 2;
const int relay2    = 5;
const int mosfetPin = 27;
const int fan1Pin   = 26;
const int fan2Pin   = 25;
<<<<<<< HEAD
const int mq135Pin = 34;          // Analog-Eingang ESP32; FE
const int servoPin = 33;          // PWM-fähiger Pin für Servo; FE
>>>>>>> f8c81de (Update config.h)
=======
const int mq135Pin  = 34;          // Analog-Eingang ESP32; FE
const int servoPin  = 32;          // PWM-fähiger Pin für Servo; FE
>>>>>>> 18fc97f (names bug fix)
=======
const int HeatingRelay    = 2;
const int CoolingRelay    = 5;
const int MosfetPetelierPin = 27;
const int MosfetFan1Pin   = 26;
const int MosfetFan2Pin   = 25;
const int mq135Pin  = 34;          // Analog-Eingang ESP32; FE
const int servoPin  = 32;          // PWM-fähiger Pin für Servo; FE

// -------- LEDC CHANNELS (ledcWrite takes channel, not pin) --------
const int MosfetPetelierCh = 0;
const int MosfetFan1Ch   = 1;
const int MosfetFan2Ch   = 2;

>>>>>>> 0522460 (air quality feature)
// -------- SHARED PARAMS --------
extern float SP;
extern float DELTA;
extern int   peltierPower;
extern int   fanPower;
extern int   airQualityLimits;      // FE
extern int   mq135Raw;              // FE
extern bool  flapOpen;             // FE
// -------- STATE --------
enum Mode { IDLE, HEATING, COOLING };
extern Mode  currentMode;
extern float currentTemp;
extern float currentPressure;

// -------- TIMING --------
extern unsigned long lastSensorRead;
extern unsigned long relayChangeTime;
extern bool pendingModeChange;
extern bool pendingHeating;
extern int  pendingPetelierPWM;
extern int  pendingFanPWM;

extern const unsigned long SENSOR_INTERVAL;
extern const unsigned long RELAY_DELAY;

// -------- SHARED OBJECTS --------
extern Adafruit_BMP085 bmp;
extern WebServer server;
extern Servo flapServo;          // FE

// -------- AP CREDENTIALS --------
extern const char *ssid;
extern const char *password;
