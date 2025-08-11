#include "actuators.h"

void initActuators() {
    // Set pin modes for actuators
    pinMode(FAN_PIN, OUTPUT);
    // BUZZER_PIN initialization handled by audio.cpp to avoid conflicts
    pinMode(RELAY_PIN, OUTPUT);
    
    // Initialize servo
    myServo.attach(SERVO_PIN);
    
    // Initialize all actuators to OFF state
    turnOffFan();
    // turnOffBuzzer(); // Buzzer handled by audio.cpp
    deactivateRelay();
}

void controlFan(int temperature, int humidity) {
    if (temperature > TEMP_THRESHOLD || humidity > HUMIDITY_THRESHOLD || AC) {
        turnOnFan();
    } else {
        turnOffFan();
    }
}

void turnOnFan() {
    digitalWrite(FAN_PIN, HIGH);
}

void turnOffFan() {
    digitalWrite(FAN_PIN, LOW);
}

void initServo() {
    myServo.attach(SERVO_PIN);
}

void moveServo() {
    extern int distance;  // From sensors module
    
    if (distance <= DISTANCE_THRESHOLD) {
        degree = 180;
        lastTime = millis();
    }

    if (millis() - lastTime >= SERVO_DELAY) {
        degree = 0;
    }

    myServo.write(degree);
    // Non-blocking: removed delay(15)
}

void setServoPosition(int position) {
    degree = position;
    myServo.write(degree);
    // Non-blocking: removed delay(15)
}

void activateRelay() {
    digitalWrite(RELAY_PIN, HIGH);
}

void deactivateRelay() {
    digitalWrite(RELAY_PIN, LOW);
}

void initBuzzer() {
    pinMode(BUZZER_PIN, OUTPUT);
    turnOffBuzzer();
}

void turnOnBuzzer() {
    digitalWrite(BUZZER_PIN, HIGH);
}

void turnOffBuzzer() {
    digitalWrite(BUZZER_PIN, LOW);
    noTone(BUZZER_PIN);
}