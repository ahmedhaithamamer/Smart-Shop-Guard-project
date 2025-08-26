#ifndef BLYNK_HANDLERS_H
#define BLYNK_HANDLERS_H

#include "config.h"
#include "blynk_instance.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "system.h"  // For AudioEvent struct

// External variables
extern bool isDay;
extern bool AC;
extern int t; // Temperature
extern int h; // Humidity
extern SemaphoreHandle_t i2cMutex;
extern QueueHandle_t audioQueue;

// Blynk initialization
void initBlynk();

// Blynk connection functions
bool connectBlynk();
bool isBlynkConnected();

// Data sending functions
void sendSensorDataToBlynk(int temperature, int humidity, bool flame, bool motion);

#endif // BLYNK_HANDLERS_H