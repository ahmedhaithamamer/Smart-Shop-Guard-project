#ifndef SYSTEM_H
#define SYSTEM_H

#include "config.h"
#include <WiFi.h>
#include <Arduino.h>

// External variables
extern char ssid[];
extern char pass[];
extern unsigned long startTime;

// WiFi reconnection variables
extern unsigned long lastWiFiReconnectAttempt;
extern const unsigned long wifiReconnectInterval;
extern bool wifiReconnecting;
extern bool wifiReconnectionEnabled;

// System initialization
void initSystem();

// WiFi functions
bool initWiFi();
bool isWiFiConnected();
void printWiFiStatus();
void handleWiFiReconnection();  // Background WiFi reconnection
void setWiFiReconnectionEnabled(bool enabled);  // Enable/disable background reconnection
void triggerWiFiReconnection();  // Manually trigger WiFi reconnection
String getWiFiStatus();         // Get WiFi status for OLED display

// Serial communication
void initSerial();

// System timing
void setStartTime();
unsigned long getUptime();

#endif // SYSTEM_H