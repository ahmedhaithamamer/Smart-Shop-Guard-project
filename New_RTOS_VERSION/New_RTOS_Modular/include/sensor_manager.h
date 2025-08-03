#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <DHT.h>
#include <ESP32Servo.h>
#include <Arduino.h>

class SensorManager {
private:
    DHT* dht;
    Servo* servo;
    
    // Sensor data
    float temperature;
    float humidity;
    int flameStatus;
    int motionStatus;
    int distance;
    int servoAngle;
    
    // Timing
    unsigned long lastServoTime;
    unsigned long lastSensorRead;

public:
    SensorManager();
    void begin();
    void updateSensors();
    
    // Individual sensor readings
    void readTemperatureHumidity();
    void readFlameSensor();
    void readMotionSensor();
    void readUltrasonicSensor();
    void updateServo();
    
    // Getters
    float getTemperature();
    float getHumidity();
    int getFlameStatus();
    int getMotionStatus();
    int getDistance();
    int getServoAngle();
    
    // Control methods
    void setServoAngle(int angle);
    void resetServo();
    
    // Status methods
    bool isFlameDetected();
    bool isMotionDetected();
    bool isObjectNearby();
};

#endif 