#include "config/config.h"

// -------- SHARED PARAMS --------
float TemperatureSp = 22.0;
float TemperatureHysteresis =  2.0;
int   peltierPower  = 180;
int   fanPower      = 255;
int   airQualitySp       = 2500;    // lediglich Startwert --> muss noch evaluiert werden; FE
int   airQualityHysteresis = 300;   // flap/fan turn off when mq135 < limit - hysteresis
int   mq135Raw           = 0;       // FE
bool  flapOpen           = false;   // FE
bool  emergencyStop      = false;   // NOT-AUS flag
bool  settingsLoggedIn   = false;   // true once authenticated for the Settings page
const char *settingsPassword = "admin";  // change to your preferred password
bool  thermalVentingActive    = false;
bool  airQualityVentingActive = false;
bool  humidityVentingActive   = false;
float currentHumidity         = 0.0f;
int   humiditySp              = 65;        // % RH default threshold
float currentAmps        = 0.0f;   // ACS712-30A

// -------- SENSOR HEALTH FLAGS --------
bool  insideTemperatureSensorOK      = true;
bool  airQualitySensorOK  = true;
bool  humiditySensorOK    = true;

// -------- OUTSIDE SENSOR --------
float outsideTemp     = 0.0f;
float outsidePressure = 0.0f;
bool  outsideTemperatureSensorOK = false;

Servo flapServo;            // FE

// -------- STATE --------
Mode  currentMode     = IDLE;
float insideTemp      = 0;
float insidePressure  = 0;

// -------- TIMING --------
unsigned long lastSensorRead  = 0;
unsigned long relayChangeTime = 0;
bool pendingModeChange = false;
bool pendingHeating    = false;
int  pendingPetelierPWM = 0;
int  pendingFanPWM      = 0;

EnergyMode currentEnergyMode = ACTIVE_MODE;          // System startet grundsätzlich im ACTIVE MODE

unsigned long lastUserAccess = 0;                    // Zeitstempel des letzten Benutzerzugriffs auf das Dashboard

const unsigned long USER_ACTIVE_TIMEOUT    = 60000;  // Benutzer gilt nach 60 Sekunden ohne Zugriff als inaktiv
const unsigned long SENSOR_INTERVAL_ACTIVE = 1000;   // Sensorintervall 1 s
const unsigned long SENSOR_INTERVAL_ECO    = 10000;  // Sensorintervall10 s
const unsigned long RELAY_DELAY     = 500;

// -------- SHARED OBJECTS --------
Adafruit_BMP085 bmpInside;
Adafruit_BMP085 bmpOutside;
WebServer       server(80);

// -------- AP CREDENTIALS --------
const char *ssid     = "Peltier-Controller";
const char *password = "12345678";
