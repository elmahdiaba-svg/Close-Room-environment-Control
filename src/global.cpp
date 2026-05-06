#include "config/config.h"

// -------- SHARED PARAMS --------
float SP         = 22.0;
float DELTA      =  2.0;
int   peltierPWM = 180;
int   fanPower   = 255;

// -------- STATE --------
Mode  currentMode     = IDLE;
float currentTemp     = 0;
float currentPressure = 0;

// -------- TIMING --------
unsigned long lastSensorRead  = 0;
unsigned long relayChangeTime = 0;
bool pendingModeChange = false;
bool pendingHeating    = false;
int  pendingPWM        = 0;
int  pendingFan        = 0;

const unsigned long SENSOR_INTERVAL = 1000;
const unsigned long RELAY_DELAY     = 500;

// -------- SHARED OBJECTS --------
Adafruit_BMP085 bmp;
WebServer       server(80);

// -------- AP CREDENTIALS --------
const char *ssid     = "Peltier-Controller";
const char *password = "12345678";