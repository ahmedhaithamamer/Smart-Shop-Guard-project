#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "config.h"

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

#endif // ACTUATORS_H