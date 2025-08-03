#include "actuator_manager.h"
#include "config.h"

ActuatorManager::ActuatorManager() {
    fanState = false;
    buzzerState = false;
    relayState = false;
    autoFanControl = true;
    alarmActive = false;
}

void ActuatorManager::begin() {
    pinMode(FAN_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    
    // Initialize actuators to OFF state
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
    
    Serial.println("Actuators initialized");
}

void ActuatorManager::updateActuators(float temperature, float humidity, bool flameDetected, bool motionDetected, bool isDayMode, bool acMode) {
    controlFan(temperature, humidity, acMode);
    controlBuzzer(flameDetected, motionDetected, isDayMode);
    controlRelay(flameDetected);
}

void ActuatorManager::controlFan(float temperature, float humidity, bool acMode) {
    if (autoFanControl) {
        if (temperature > TEMP_THRESHOLD || humidity > HUMIDITY_THRESHOLD || acMode) {
            setFan(true);
        } else {
            setFan(false);
        }
    }
}

void ActuatorManager::controlBuzzer(bool flameDetected, bool motionDetected, bool isDayMode) {
    if (flameDetected) {
        // Continuous alarm for flame detection
        setBuzzer(true);
        alarmActive = true;
    } else if (motionDetected && isDayMode) {
        // Short beep for motion detection during day mode
        tone(BUZZER_PIN, 1500, 200);
        alarmActive = false;
    } else {
        setBuzzer(false);
        alarmActive = false;
    }
}

void ActuatorManager::controlRelay(bool flameDetected) {
    if (flameDetected) {
        setRelay(true);
    } else {
        setRelay(false);
    }
}

void ActuatorManager::setFan(bool state) {
    fanState = state;
    digitalWrite(FAN_PIN, state ? HIGH : LOW);
}

void ActuatorManager::setBuzzer(bool state) {
    buzzerState = state;
    if (state) {
        tone(BUZZER_PIN, 1000);
    } else {
        noTone(BUZZER_PIN);
    }
}

void ActuatorManager::setRelay(bool state) {
    relayState = state;
    digitalWrite(RELAY_PIN, state ? HIGH : LOW);
}

void ActuatorManager::triggerAlarm() {
    setBuzzer(true);
    alarmActive = true;
}

void ActuatorManager::stopAlarm() {
    setBuzzer(false);
    alarmActive = false;
}

bool ActuatorManager::getFanState() {
    return fanState;
}

bool ActuatorManager::getBuzzerState() {
    return buzzerState;
}

bool ActuatorManager::getRelayState() {
    return relayState;
}

bool ActuatorManager::isAlarmActive() {
    return alarmActive;
}

void ActuatorManager::setAutoFanControl(bool enabled) {
    autoFanControl = enabled;
}

bool ActuatorManager::getAutoFanControl() {
    return autoFanControl;
} 