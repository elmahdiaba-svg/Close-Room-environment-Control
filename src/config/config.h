#pragma once
#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// -------- PIN CONFIG --------
const int HeatingRelay      = 2;
const int CoolingRelay      = 5;
const int MosfetPetelierPin = 27;
const int MosfetFan1Pin     = 26;
const int MosfetFan2Pin     = 25;
const int mq135Pin          = 34;          // Analog-Eingang ESP32; FE
const int servoPin          = 32;          // PWM-fähiger Pin für Servo; FE
const int CouranrSensor_PIN        = 35;          // ACS712-30A current sensor
const int AirQualityFanPin  = 4;           // 3.3V ventilation fan — runs during air quality venting
const int HumiditySensor_PIN         = 13;          // DHT11 humidity sensor data pin

// -------- I2C BUS 1 — inside BMP180 (Wire, default SDA=21 SCL=22) --------
// -------- I2C BUS 2 — outside BMP180 (Wire1) --------
const int OutsideTemperatureSDA_PIN          = 16;
const int OutsideTemperatureSCL_PIN          = 17;

// -------- LEDC CHANNELS --------
// Servo (ESP32Servo) claims channels 0-3 (timer 0/1).
// MOSFETs use channels 4, 5, 6 (timer 2) to avoid timer conflict.
const int MosfetPetelierCh = 4;
const int MosfetFan1Ch     = 5;
const int MosfetFan2Ch     = 6;

// -------- SHARED PARAMS --------
extern float SP;
extern float DELTA;
extern int   peltierPower;
extern int   fanPower;
extern int   airQualityLimits;        // FE
extern int   mq135Raw;                // FE
extern bool  flapOpen;                // FE
extern bool  thermalVentingActive;    // true while FREE_COOLING / FREE_HEATING
extern bool  airQualityVentingActive; // true while flap is open for air quality — highest priority
extern bool  humidityVentingActive;   // true while flap is open due to high humidity
extern float currentAmps;            // ACS712-30A

// -------- HUMIDITY SENSOR (DHT11) --------
extern float currentHumidity;        // % RH
extern int   humidityLimit;          // % RH — open flap above this value

// -------- OUTSIDE SENSOR --------
extern float outsideTemp;
extern float outsidePressure;
extern bool  outsideTemperatureSensorOK;

// -------- STATE --------
enum Mode { IDLE, HEATING, COOLING, FREE_COOLING, FREE_HEATING };
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
extern Adafruit_BMP085 bmpOutside;
extern WebServer server;
extern Servo flapServo;              // FE

// -------- AP CREDENTIALS --------
extern const char *ssid;
extern const char *password;
