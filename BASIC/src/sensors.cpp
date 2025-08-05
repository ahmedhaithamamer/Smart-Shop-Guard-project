#include "sensors.h"

void initSensors() {
    // Initialize DHT sensor
    dht.begin();
    
    // Set pin modes for sensors
    pinMode(FLAME_SENSOR_PIN, INPUT);
    pinMode(PIR_PIN, INPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

void readTemperatureHumidity() {
    h = dht.readHumidity();
    t = dht.readTemperature();
    
    // Note: Blynk data sending is handled in main.cpp to avoid multiple definition issues
}

int getTemperature() {
    return t;
}

int getHumidity() {
    return h;
}

void readFlameSensor() {
    // Sensor reading logic is handled in main.cpp
    // This function kept for compatibility
}

bool isFlameDetected() {
    return (digitalRead(FLAME_SENSOR_PIN) == 0);  // Active low
}

void readMotion() {
    // Motion detection logic is handled in main.cpp
    // This function kept for compatibility
    delay(200);
}

bool isMotionDetected() {
    return (digitalRead(PIR_PIN) == HIGH);
}

void triggerUltrasonicSensor() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
}

int getDistance() {
    return distance;
}