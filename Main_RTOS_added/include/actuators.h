#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// External variables
extern Servo myServo;
extern int degree;
extern unsigned long lastTime;
extern bool AC;

// Actuator initialization
void initActuators();

// Fan control functions
void controlFan(int temperature, int humidity);
void turnOnFan();
void turnOffFan();

// Servo control functions
void initServo();
void moveServo();
void setServoPosition(int position);

// Relay control functions
void activateRelay();
void deactivateRelay();

// Buzzer control functions
void initBuzzer();
void turnOnBuzzer();
void turnOffBuzzer();

// RTOS task
void TaskActuators(void* pvParameters);

#endif // ACTUATORS_H