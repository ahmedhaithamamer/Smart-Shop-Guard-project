#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Shared sensor readings payload
typedef struct {
  int temperatureC;
  int humidityPct;
  bool flame;
  int distanceCm;
  bool pirMotion;
  uint32_t tsMs;
} SensorData;

// Externs from main.cpp
extern DHT dht;
extern int t;
extern int h;
extern long duration;
extern int distance;

extern volatile bool pirFlag; // set from ISR

void initSensors();
void readTemperatureHumidity();
int getTemperature();
int getHumidity();
void readFlameSensor();
bool isFlameDetected();
void readMotion();
bool isMotionDetected();
void triggerUltrasonicSensor();
int getDistance();

// RTOS tasks
void TaskSensorPoll(void* pvParameters);
void IRAM_ATTR PIR_ISR();

#endif // SENSORS_H