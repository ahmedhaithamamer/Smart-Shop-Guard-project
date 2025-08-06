#include "sensors.h"
#include "blynk_instance.h"

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
    
    // Send to Blynk if connected
    if (Blynk.connected()) {
        Blynk.virtualWrite(VPIN_TEMPERATURE, t);
        Blynk.virtualWrite(VPIN_HUMIDITY, h);
    }
}

int getTemperature() {
    return t;
}

int getHumidity() {
    return h;
}

void readFlameSensor() {
    int flameValue = digitalRead(FLAME_SENSOR_PIN);
    
    // Send to Blynk if connected (inverted because sensor is active low)
    if (Blynk.connected()) {
        Blynk.virtualWrite(VPIN_FLAME, !flameValue);
    }
}

bool isFlameDetected() {
    return (digitalRead(FLAME_SENSOR_PIN) == 0);  // Active low
}

void readMotion() {
    int pirState = digitalRead(PIR_PIN);
    
    // Send to Blynk if connected
    if (Blynk.connected()) {
        Blynk.virtualWrite(VPIN_MOTION, pirState);
    }
    
    delay(50); // Reduced from 200ms to 50ms
    yield(); // Allow other tasks to run
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