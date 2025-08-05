#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"

// External variables
extern LiquidCrystal_I2C lcd;
extern bool isDay;

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
void displayFireAlert();
void displaySafeStatus();
void displayThiefAlert();

#endif // DISPLAY_H