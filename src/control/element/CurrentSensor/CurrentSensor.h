#pragma once

void calibrateCurrentSensor();   // call once in setup() with no load connected
void handleCurrentSensor();      // call each sensor cycle to update currentAmps
