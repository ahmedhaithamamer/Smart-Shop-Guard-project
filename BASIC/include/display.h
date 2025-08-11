#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// External variables
extern LiquidCrystal_I2C lcd;
extern bool isDay;
extern int t;  // Temperature
extern int h;  // Humidity

// Display initialization
void initDisplay();

// Basic display functions
void clearDisplay();
void setDisplayCursor(int col, int row);
void printToDisplay(const char* text);
void printToDisplay(int value);

// Specific display functions
void displayTemperatureHumidity(int temperature, int humidity);
void displayWelcomeMessage();
void displayModeStatus();
void displayNormalStatus();
void displayFireAlert();
void displaySafeStatus();
void displayThiefAlert();

// RTOS tasks
void TaskLCD(void* pvParameters);

#endif // DISPLAY_H