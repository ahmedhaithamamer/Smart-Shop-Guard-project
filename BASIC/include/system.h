#ifndef SYSTEM_H
#define SYSTEM_H

#include "config.h"
#include <WiFi.h>
#include <Arduino.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

// External variables
extern char ssid[];
extern char pass[];
extern unsigned long startTime;

// RTOS resources
extern QueueHandle_t sensorDataQueue;   // queue of SensorData
extern QueueHandle_t eventQueue;        // queue of Event
extern QueueHandle_t audioQueue;        // queue of AudioEvent (Core 1 -> Core 0)
extern SemaphoreHandle_t i2cMutex;      // shared I2C bus protection
extern SemaphoreHandle_t dataMutex;     // protects shared sensor state

// Event types for inter-task notifications
enum EventType {
  EVENT_NONE = 0,
  EVENT_MOTION_DETECTED,
  EVENT_MOTION_CLEARED,
  EVENT_FIRE_DETECTED,
  EVENT_FIRE_CLEARED
};

struct Event {
  EventType type;
  uint32_t tsMs;
};

// Audio event types for Core 0 audio processing
enum AudioEventType {
  AUDIO_STARTUP = 0,
  AUDIO_MODE_SWITCH,
  AUDIO_FIRE_ALERT,
  AUDIO_MOTION_ALERT
};

struct AudioEvent {
  AudioEventType type;
  uint32_t timestamp;
};

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

// RTOS task entrypoints
void TaskWiFiBlynk(void* pvParameters);

// WiFi reconnection variables
extern unsigned long lastWiFiReconnectAttempt;
extern const unsigned long wifiReconnectInterval;
extern bool wifiReconnecting;
extern bool wifiReconnectionEnabled;

#endif // SYSTEM_H