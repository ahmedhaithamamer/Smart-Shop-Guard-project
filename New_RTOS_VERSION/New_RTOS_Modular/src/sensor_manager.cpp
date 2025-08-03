#include "sensor_manager.h"
#include "config.h"

SensorManager::SensorManager() {
    dht = new DHT(DHTPIN, DHTTYPE);
    servo = new Servo();
    
    temperature = 0.0;
    humidity = 0.0;
    flameStatus = 0;
    motionStatus = 0;
    distance = 0;
    servoAngle = 0;
    
    lastServoTime = 0;
    lastSensorRead = 0;
}

void SensorManager::begin() {
    dht->begin();
    servo->attach(SERVO_PIN);
    
    // Initialize pins
    pinMode(FLAME_SENSOR_PIN, INPUT);
    pinMode(PIR_PIN, INPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    Serial.println("Sensors initialized");
}

void SensorManager::updateSensors() {
    readTemperatureHumidity();
    readFlameSensor();
    readMotionSensor();
    readUltrasonicSensor();
    updateServo();
}

void SensorManager::readTemperatureHumidity() {
    humidity = dht->readHumidity();
    temperature = dht->readTemperature();
    
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
        humidity = 0.0;
        temperature = 0.0;
    }
}

void SensorManager::readFlameSensor() {
    flameStatus = digitalRead(FLAME_SENSOR_PIN);
}

void SensorManager::readMotionSensor() {
    motionStatus = digitalRead(PIR_PIN);
}

void SensorManager::readUltrasonicSensor() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;
    
    if (distance <= DISTANCE_THRESHOLD) {
        setServoAngle(180);
        lastServoTime = millis();
    }
}

void SensorManager::updateServo() {
    if (millis() - lastServoTime >= SERVO_TIMEOUT) {
        resetServo();
    }
    servo->write(servoAngle);
}

float SensorManager::getTemperature() {
    return temperature;
}

float SensorManager::getHumidity() {
    return humidity;
}

int SensorManager::getFlameStatus() {
    return flameStatus;
}

int SensorManager::getMotionStatus() {
    return motionStatus;
}

int SensorManager::getDistance() {
    return distance;
}

int SensorManager::getServoAngle() {
    return servoAngle;
}

void SensorManager::setServoAngle(int angle) {
    servoAngle = angle;
    servo->write(servoAngle);
}

void SensorManager::resetServo() {
    servoAngle = 0;
    servo->write(servoAngle);
}

bool SensorManager::isFlameDetected() {
    return (flameStatus == 0); // Flame sensor is active low
}

bool SensorManager::isMotionDetected() {
    return (motionStatus == 1); // PIR sensor is active high
}

bool SensorManager::isObjectNearby() {
    return (distance <= DISTANCE_THRESHOLD);
} 