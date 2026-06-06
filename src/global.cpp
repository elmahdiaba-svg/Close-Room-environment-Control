#include "config/config.h"

// -------- SHARED PARAMS --------
float SP            = 22.0;
float DELTA         =  2.0;
int   peltierPower  = 180;
int   fanPower      = 255;
int   airQualityLimits   = 2500;    // lediglich Startwert --> muss noch evaluiert werden; FE
int   mq135Raw           = 0;       // FE
bool  flapOpen           = false;   // FE
bool  thermalVentingActive    = false;
bool  airQualityVentingActive = false;
bool  humidityVentingActive   = false;
float currentHumidity         = 0.0f;
int   humidityLimit           = 65;        // % RH default threshold
float currentAmps        = 0.0f;   // ACS712-30A

// -------- OUTSIDE SENSOR --------
float outsideTemp     = 0.0f;
float outsidePressure = 0.0f;
bool  outsideTemperatureSensorOK = false;

Servo flapServo;            // FE

// -------- STATE --------
Mode  currentMode     = IDLE;
float currentTemp     = 0;
float currentPressure = 0;

// -------- TIMING --------
unsigned long lastSensorRead  = 0;
unsigned long relayChangeTime = 0;
bool pendingModeChange = false;
bool pendingHeating    = false;
int  pendingPetelierPWM = 0;
int  pendingFanPWM      = 0;

const unsigned long SENSOR_INTERVAL = 1000;
const unsigned long RELAY_DELAY     = 500;

// -------- SHARED OBJECTS --------
Adafruit_BMP085 bmp;
Adafruit_BMP085 bmpOutside;
WebServer       server(80);

// -------- AP CREDENTIALS --------
const char *ssid     = "Peltier-Controller";
const char *password = "12345678";
