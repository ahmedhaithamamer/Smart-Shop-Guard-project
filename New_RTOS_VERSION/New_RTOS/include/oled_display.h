#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>

void initOLED();
void startOLEDTask();
void updateOLEDStatus(const char* wifiStatus, const char* blynkStatus, const char* mode = "Idle");

#endif
