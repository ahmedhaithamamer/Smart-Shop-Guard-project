#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include "config.h"
#include "icons.h"
#include <SH1106Wire.h>
#include <Arduino.h>
#include <Wire.h>

// External variables from main.cpp
extern int t;  // Temperature
extern int h;  // Humidity
extern bool isDay;
extern bool AC;

// OLED Display object
extern SH1106Wire display;

// Page types for OLED navigation
enum PageType {
  PAGE_STATUS = 0,
  PAGE_SENSORS = 1,
  PAGE_SYSTEM = 2,
  PAGE_ALERTS = 3,
  PAGE_SETTINGS = 4
};

// Menu state
enum MenuState {
  STATE_PAGES = 0,
  STATE_SETTINGS = 1
};

// Configuration structure (simplified - no theme)
struct OLEDConfig {
  bool auto_swipe;
};

// Global OLED variables
extern int currentPage;
extern int currentSetting;
extern MenuState currentState;
extern OLEDConfig oledConfig;
extern bool alertFlashing;
extern bool fireDetected;
extern bool motionDetected;
extern bool doorOpen;

// OLED Display initialization
void initOLEDDisplay();
void initOLEDButtons();

// Main display functions (OLED-specific to avoid LCD conflicts)
void clearOLEDDisplay();
void displayOLEDTemperatureHumidity(int temperature, int humidity);
void displayOLEDWelcomeMessage();
void displayOLEDModeStatus();
void displayOLEDFireAlert();
void displayOLEDSafeStatus();
void displayOLEDThiefAlert();
void displayOLEDMotionCleared();

// OLED-specific functions
void showIntro();
void handleOLEDButtons();
void updateOLEDDisplay();
void showCurrentPage();
void handleButtons();
void toggleSettings();
void executeSetting();

// Individual page functions
void showStatusPage();
void showSensorPage();
void showSystemPage();
void showAlertsPage();
void showSettingsPage();

// Utility functions
void drawProgressBar(int x, int y, int width, int height, int percentage);
void animateAlert();

#endif // OLED_DISPLAY_H