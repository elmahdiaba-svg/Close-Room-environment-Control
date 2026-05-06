#pragma once
#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <WebServer.h>

// -------- PIN CONFIG --------
const int relay1    = 2;
const int relay2    = 5;
const int mosfetPin = 27;
const int fan1Pin   = 26;
const int fan2Pin   = 25;

// -------- SHARED PARAMS --------
extern float SP;
extern float DELTA;
extern int   peltierPWM;
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
extern int  pendingPWM;
extern int  pendingFan;

extern const unsigned long SENSOR_INTERVAL;
extern const unsigned long RELAY_DELAY;

// -------- SHARED OBJECTS --------
extern Adafruit_BMP085 bmp;
extern WebServer server;

// -------- AP CREDENTIALS --------
extern const char *ssid;
extern const char *password;