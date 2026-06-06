#pragma once

#include "control/element/Petelier_Polarity/Petelier_Polarity.h"
#include "control/element/Sensor_Reading/Sensor_Reading.h"
#include "control/statemachine/statemachine.h"
#include "control/element/AirQuality/AirQuality.h"              // FE
#include "control/element/CurrentSensor/CurrentSensor.h"
#include "control/element/outsideTemperatureSensor/outsideTemperatureSensor.h"
#include "control/element/HumiditySensor/HumiditySensor.h"

void runControlLoop();
