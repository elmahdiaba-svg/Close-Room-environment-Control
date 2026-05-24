#pragma once
#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <WebServer.h>

// -------- PIN CONFIG --------
const int HeatingRelay    = 2;
const int CoolingRelay    = 5;
const int MosfetPetelierPin = 27;
const int MosfetFan1Pin   = 26;
const int MosfetFan2Pin   = 25;

// -------- LEDC CHANNELS (ledcWrite takes channel, not pin) --------
const int MosfetPetelierCh = 0;
const int MosfetFan1Ch   = 1;
const int MosfetFan2Ch   = 2;

// -------- SHARED PARAMS --------
extern float SP;
extern float DELTA;
extern int   peltierPower;
extern int   fanPower;

// -------- STATE --------
enum Mode { IDLE, HEATING, COOLING };
extern Mode currentMode;
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

// -------- AP CREDENTIALS --------
extern const char *ssid;
extern const char *password;