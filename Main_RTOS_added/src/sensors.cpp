#include "sensors.h"
#include "blynk_instance.h"
#include "system.h"  // For Event struct and EventType

// 🔍 PIR Motion Detection State Tracking
// Moved from ISR to task-based debouncing for better stability
static bool lastPirState = false;        // Previous PIR sensor state
static int pirStabilityCounter = 0;      // State stability counter for debouncing

// 🔧 Sensor System Initialization
// Sets up all monitoring sensors and configures pin modes
void initSensors() {
    dht.begin();  // Initialize DHT11 temperature/humidity sensor
    
    // 📌 Pin Configuration for All Sensors
    pinMode(FLAME_SENSOR_PIN, INPUT);   // Flame detection sensor (active low)
    pinMode(PIR_PIN, INPUT);            // Passive Infrared motion sensor
    pinMode(TRIG_PIN, OUTPUT);          // Ultrasonic sensor trigger pin
    pinMode(ECHO_PIN, INPUT);           // Ultrasonic sensor echo pin
    
    // 🔍 PIR Initial State Setup
    lastPirState = digitalRead(PIR_PIN);  // Capture initial motion sensor state
}

// 🌡️ Temperature & Humidity Reading
// Updates global temperature and humidity variables from DHT11 sensor
void readTemperatureHumidity() {
    h = dht.readHumidity();      // Read humidity percentage
    t = dht.readTemperature();   // Read temperature in Celsius
    
    // Note: Blynk data transmission now handled by TaskWiFiBlynk for better performance
}

// 📊 Sensor Data Access Functions
int getTemperature() {
    return t;  // Return current temperature reading
}

int getHumidity() {
    return h;  // Return current humidity reading
}

// 🔥 Flame Detection Functions
void readFlameSensor() {
    // Sensor reading function - actual data transmission handled by TaskWiFiBlynk
    // Current flame status available via isFlameDetected()
}

bool isFlameDetected() {
    return (digitalRead(FLAME_SENSOR_PIN) == 0);  // Flame sensor is active low
}

// 🚶 Motion Detection with Intelligent Debouncing
// Implements day/night aware motion detection with stability filtering
void readMotion() {
    bool currentPirState = digitalRead(PIR_PIN);  // Read current PIR sensor state
    
    // 🔍 PIR State Change Detection & Debouncing
    if (currentPirState != lastPirState) {
        pirStabilityCounter++;  // Increment stability counter
        if (pirStabilityCounter >= 2) {  // Require 2 stable samples for state change
            // State has stabilized, process motion event
            extern QueueHandle_t eventQueue;
            extern bool isDay;  // Access day/night mode flag
            
            if (eventQueue) {
                // 🌙 Night Mode Motion Detection (Thief Alert)
                if (currentPirState && isDay) {
                    Event e{EVENT_MOTION_DETECTED, (uint32_t)millis()};
                    xQueueSend(eventQueue, &e, 0);  // Non-blocking event queue
                    Serial.println("🚨 Motion detected at NIGHT - Thief alert triggered");
                } else if (currentPirState && isDay) {
                    // ☀️ Day Mode Motion Detection (Normal Operation)
                    Serial.println("👥 Motion detected during DAY - No thief alert (normal operation)");
                } else if (!currentPirState) {
                    // ✅ Motion Cleared (Any Mode)
                    Event e{EVENT_MOTION_CLEARED, (uint32_t)millis()};
                    xQueueSend(eventQueue, &e, 0);  // Non-blocking event queue
                    Serial.println("✅ Motion cleared");
                }
            }
            lastPirState = currentPirState;     // Update stable state
            pirStabilityCounter = 0;            // Reset stability counter
        }
    } else {
        pirStabilityCounter = 0;  // Reset counter if state remains unchanged
    }
}

// 📊 Motion Status Access
bool isMotionDetected() {
    return (digitalRead(PIR_PIN) == HIGH);  // Return current motion detection status
}

// 📏 Ultrasonic Distance Measurement
// Triggers ultrasonic sensor and calculates distance with timeout protection
void triggerUltrasonicSensor() {
    // 🚀 Ultrasonic Pulse Generation
    digitalWrite(TRIG_PIN, LOW);           // Clear trigger pin
    delayMicroseconds(2);                  // Stabilization delay
    digitalWrite(TRIG_PIN, HIGH);          // Send trigger pulse
    delayMicroseconds(10);                 // Pulse width (10μs minimum)
    digitalWrite(TRIG_PIN, LOW);           // End trigger pulse

    // 📡 Echo Detection with Timeout Protection
    duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout (~5m max range)
    if (duration == 0) {
        // Store result in global duration variable, distance calculation done in main.cpp
        duration = 0;  // Indicate no echo received (out of range)
    }
    // duration now contains the pulse time, distance calculation handled in main.cpp
}

// 📏 Distance Data Access
int getDistance() {
    // Calculate distance from duration (speed of sound calculation)
    if (duration == 0) {
        return 999;  // Out of range
    } else {
        return duration * 0.034 / 2;  // Convert to cm
    }
}