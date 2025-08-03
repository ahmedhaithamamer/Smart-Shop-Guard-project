#include "blynk_manager.h"
#include "config.h"

// Global variables for Blynk callbacks
bool isDay = false;
bool AC = false;

BlynkManager::BlynkManager(const char* authToken) {
    this->authToken = authToken;
    this->isConnected = false;
    this->lastReconnectAttempt = 0;
    this->reconnectInterval = 10000; // 10 seconds
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

// Blynk button handlers
void BlynkManager::onDayModeChange(BlynkReq& request) {
    isDay = request.getParam().asInt();
    Serial.print("Day mode changed to: ");
    Serial.println(isDay ? "Night" : "Day");
}

void BlynkManager::onACChange(BlynkReq& request) {
    AC = request.getParam().asInt();
    Serial.print("AC mode changed to: ");
    Serial.println(AC ? "ON" : "OFF");
} 