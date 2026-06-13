#pragma once
#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// -------- PIN CONFIG --------
const int HeatingRelay          = 2;
const int CoolingRelay          = 5;
const int MosfetPetelierPin     = 27;
const int MosfetFan1Pin         = 26;
const int MosfetFan2Pin         = 25;
const int mq135Pin              = 34;          // Analog-Eingang ESP32; FE
const int servoPin              = 32;          // PWM-fähiger Pin für Servo; FE
const int currentSensor_PIN     = 35;          // ACS712-30A current sensor
const int AirQualityFanPin      = 4;           // relay-driven ventilation fan — runs during air quality venting
const int HumiditySensor_PIN    = 13;          // DHT11 humidity sensor data pin

// -------- LEDS --------
const int LED_HEATING_PIN  = 14;   // orange LED — HEATING mode
const int LED_COOLING_PIN  = 18;   // blue/cyan LED — COOLING mode
const int LED_FREE_PIN     = 19;   // green LED — FREE_COOLING / FREE_HEATING
const int LED_READY_PIN    = 23;   // green LED — server running (off = NOT-AUS)

// -------- NOT-AUS --------
const int NotAusButton_PIN = 15;   // push-button to GND (internal pull-up used)

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
extern float TemperatureSp;
extern float TemperatureHysteresis;
extern int   peltierPower;
extern int   fanPower;

// -------- INSIDE BMP Sensor  --------
extern float insideTemp;
extern float insidePressure;

// -------- Air Quality --------
extern int   mq135Raw;                // FE
extern int   airQualitySp;            // FE
extern int   airQualityHysteresis;    // gap below limit before flap/fan turn off again

//-------- Current sensor
extern float currentAmps;            // ACS712-30A

// -------- SENSOR HEALTH FLAGS (true while readings look plausible) --------
extern bool  insideTemperatureSensorOK;          // inside BMP180 (temperature + pressure)
extern bool  airQualitySensorOK;      // MQ135 analog air-quality sensor
extern bool  humiditySensorOK;        // DHT11 humidity sensor

// -------- SETPOINT / LIMIT BOUNDS (clamped server-side, enforced in the UI) --------
const float TemperatureSp_MIN = 10.0;   // °C — lowest allowed Inside Temperature Setpoint
const float TemperatureSp_MAX = 35.0;   // °C — highest allowed Inside Temperature Setpoint
const int   AirqualitySp_MIN = 500;    // raw MQ135 units — lowest allowed Air Quality Setpoint
const int   AirqualitySp_MAX = 4000;   // raw MQ135 units — highest allowed Air Quality Setpoint
const int   HumiditySp_MIN   = 20;     // % RH — lowest allowed Humidity Setpoint 
const int   HumiditySp_MAX   = 90;     // % RH — highest allowed Humidity Setpoint 

// -------- HUMIDITY SENSOR (DHT11) --------
extern float currentHumidity;        // % RH
extern int   humiditySp;             // % RH — open flap above this value

// -------- OUTSIDE BMP SENSOR --------
extern float outsideTemp;
extern float outsidePressure;
extern bool  outsideTemperatureSensorOK;

// -------- STATE --------
enum Mode { IDLE, HEATING, COOLING, FREE_COOLING, FREE_HEATING };
extern Mode  currentMode;
enum EnergyMode { ACTIVE_MODE, ECO_MODE };
extern EnergyMode currentEnergyMode;  // Aktuell aktiver Energiemodus

// -------- Others
extern bool  flapOpen;                // Servo flap state for air quality venting — true while flap is open;
extern bool  emergencyStop;           // NOT-AUS: halts all temperature control
extern bool  settingsLoggedIn;        // true once the user has authenticated for the Settings page
extern const char *settingsPassword;  // password required to open Settings
extern bool  thermalVentingActive;    // true while FREE_COOLING / FREE_HEATING
extern bool  airQualityVentingActive; // true while flap is open for air quality — highest priority
extern bool  humidityVentingActive;   // true while flap is open due to high humidity

// -------- TIMING --------
extern unsigned long lastSensorRead;
extern unsigned long relayChangeTime;
extern bool pendingModeChange;
extern bool pendingHeating;
extern int  pendingPetelierPWM;
extern int  pendingFanPWM;

extern unsigned long lastUserAccess;                    // Zeitstempel des letzten Benutzerzugriffs auf das Dashboard

extern const unsigned long USER_ACTIVE_TIMEOUT;         // Zeitraum, nach dem ein Benutzer als "inaktiv" gilt
extern const unsigned long SENSOR_INTERVAL_ACTIVE;      // Sensorabfrageintervall im ACTIVE MODE
extern const unsigned long SENSOR_INTERVAL_ECO;         // Sensorabfrageintervall im ECO MODE   

unsigned long getCurrentSensorInterval();               // Ermittelt das aktuell gültige Sensorintervall
extern const unsigned long RELAY_DELAY;

// -------- SHARED OBJECTS --------
extern Adafruit_BMP085 bmpInside;
extern Adafruit_BMP085 bmpOutside;
extern WebServer server;
extern Servo flapServo;              // FE

// -------- AP CREDENTIALS --------
extern const char *ssid;
extern const char *password;
