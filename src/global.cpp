#include "config/config.h"

// -------- SHARED PARAMS --------
<<<<<<< HEAD
float SP         = 22.0;
float DELTA      =  2.0;
int   peltierPower = 180;
int   fanPower   = 255;
int airQualityLimit = 2500; // lediglich Startwert --> muss noch evaluiert werden; FE
int mq135Raw = 0;           // FE
bool flapOpen = false;      // FE
=======
float SP                = 22.0;
float DELTA             =  2.0;
int   peltierPWM        = 180;
int   fanPower          = 255;
int   airQualityLimits  = 2500; // lediglich Startwert --> muss noch evaluiert werden; FE
int   mq135Raw          = 0;           // FE
bool  flapOpen          = false;      // FE
>>>>>>> 18fc97f (names bug fix)

Servo flapServo;            // FE

// -------- STATE --------
Mode  currentMode     = IDLE;
float currentTemp     = 0;
float currentPressure = 0;

// -------- TIMING --------
<<<<<<< HEAD
unsigned long lastSensorRead  = 0;
unsigned long relayChangeTime = 0;
bool pendingModeChange = false;
bool pendingHeating    = false;
int  pendingPetelierPWM        = 0;
int  pendingFanPWM        = 0;
=======
unsigned long lastSensorRead    = 0;
unsigned long relayChangeTime   = 0;
bool pendingModeChange          = false;
bool pendingHeating             = false;
int  pendingPWM                 = 0;
int  pendingFan                 = 0;
>>>>>>> 18fc97f (names bug fix)

const unsigned long SENSOR_INTERVAL = 1000;
const unsigned long RELAY_DELAY     = 500;

// -------- SHARED OBJECTS --------
Adafruit_BMP085 bmp;
WebServer       server(80);

// -------- AP CREDENTIALS --------
const char *ssid     = "Peltier-Controller";
const char *password = "12345678";
