#ifndef SYSTEM_H
#define SYSTEM_H

#include "config.h"
#include <WiFi.h>
#include <Arduino.h>

// External variables
extern char ssid[];
extern char pass[];
extern unsigned long startTime;

// System initialization
void initSystem();

// WiFi functions
bool initWiFi();
bool isWiFiConnected();
void printWiFiStatus();

// Serial communication
void initSerial();

// System timing
void setStartTime();
unsigned long getUptime();

#endif // SYSTEM_H