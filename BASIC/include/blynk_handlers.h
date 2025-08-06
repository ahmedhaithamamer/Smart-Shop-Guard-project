#ifndef BLYNK_HANDLERS_H
#define BLYNK_HANDLERS_H

#include "config.h"
#include "blynk_instance.h"

// External variables
extern bool isDay;
extern bool AC;

// Blynk initialization
void initBlynk();

// Blynk connection functions
bool connectBlynk();
bool isBlynkConnected();

// Blynk virtual pin handlers (these are called automatically by Blynk)
BLYNK_WRITE(VPIN_DAY_NIGHT);
BLYNK_WRITE(VPIN_AC_CONTROL);

// Data sending functions
void sendSensorDataToBlynk(int temperature, int humidity, bool flame, bool motion);

#endif // BLYNK_HANDLERS_H