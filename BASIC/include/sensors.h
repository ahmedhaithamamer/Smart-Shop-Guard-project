#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"
#include <DHT.h>
#include <Arduino.h>

// External variables that need to be accessed by sensors
extern DHT dht;
extern int h, t;
extern long duration;
extern int distance;

// Sensor initialization
void initSensors();

// DHT sensor functions
void readTemperatureHumidity();
int getTemperature();
int getHumidity();

// Flame sensor functions
void readFlameSensor();
bool isFlameDetected();

// PIR motion sensor functions
void readMotion();
bool isMotionDetected();

// Ultrasonic sensor functions
void triggerUltrasonicSensor();
int getDistance();

#endif // SENSORS_H