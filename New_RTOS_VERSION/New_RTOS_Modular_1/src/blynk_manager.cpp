#include "blynk_manager.h"
#include "config.h"
#include <BlynkSimpleEsp32.h>

BlynkManager* globalBlynkManager = nullptr;

BlynkManager::BlynkManager(const char* authToken) {
    this->authToken = authToken;
    this->isConnected = false;
    this->lastReconnectAttempt = 0;
    this->reconnectInterval = 10000; // 10 seconds
    this->isDay = false;
    this->AC = false;
}

bool BlynkManager::begin() {
    Blynk.config(authToken.c_str());
    Serial.println("Blynk configured");
    return true;
}

bool BlynkManager::connect() {
    if (Blynk.connect()) {
        Serial.println("Blynk connected!");
        isConnected = true;
        return true;
    } else {
        Serial.println("Blynk connection failed!");
        isConnected = false;
        return false;
    }
}

bool BlynkManager::getDayMode() {
    return isDay;
}

bool BlynkManager::getACMode() {
    return AC;
}


bool BlynkManager::isBlynkConnected() {
    isConnected = Blynk.connected();
    return isConnected;
}

void BlynkManager::checkConnection() {
    if (!isBlynkConnected()) {
        unsigned long currentTime = millis();
        if (currentTime - lastReconnectAttempt >= reconnectInterval) {
            Serial.println("Blynk disconnected. Attempting to reconnect...");
            reconnect();
            lastReconnectAttempt = currentTime;
        }
    }
}

void BlynkManager::reconnect() {
    if (Blynk.connect()) {
        Serial.println("Blynk reconnected!");
        isConnected = true;
    } else {
        Serial.println("Blynk reconnection failed!");
        isConnected = false;
    }
}

String BlynkManager::getStatus() {
    if (isBlynkConnected()) {
        return "Connected";
    } else {
        return "Disconnected";
    }
}

void BlynkManager::run() {
    Blynk.run();
}

void BlynkManager::writeVirtualPin(int pin, int value) {
    if (isBlynkConnected()) {
        Blynk.virtualWrite(pin, value);
    }
}

void BlynkManager::writeVirtualPin(int pin, float value) {
    if (isBlynkConnected()) {
        Blynk.virtualWrite(pin, value);
    }
}

void BlynkManager::writeVirtualPin(int pin, const char* value) {
    if (isBlynkConnected()) {
        Blynk.virtualWrite(pin, value);
    }
}

void BlynkManager::setDayMode(bool dayMode) {
    this->isDay = dayMode;
    Serial.print("Day mode changed to: ");
    Serial.println(dayMode ? "Night" : "Day");
}

void BlynkManager::setACMode(bool acMode) {
    this->AC = acMode;
    Serial.print("AC mode changed to: ");
    Serial.println(acMode ? "ON" : "OFF");
}

// Blynk button handlers - must be in the same file as Blynk include
BLYNK_WRITE(V5) {  // VIRTUAL_PIN_DAY_MODE = 5
    if (globalBlynkManager != nullptr) {
        globalBlynkManager->setDayMode(param.asInt());
    }
}

BLYNK_WRITE(V6) {  // VIRTUAL_PIN_AC = 6
    if (globalBlynkManager != nullptr) {
        globalBlynkManager->setACMode(param.asInt());
    }
}
 