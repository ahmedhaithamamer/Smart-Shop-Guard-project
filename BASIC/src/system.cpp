#include "system.h"

void initSystem() {
    initSerial();
    setStartTime();
}

void initSerial() {
    Serial.begin(SERIAL_BAUD_RATE);
}

bool initWiFi() {
    WiFi.begin(ssid, pass);
    
    unsigned long wifiConnectStart = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - wifiConnectStart < WIFI_TIMEOUT) {
        delay(200);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected");
        return true;
    } else {
        Serial.println("WiFi not connected");
        return false;
    }
}

bool isWiFiConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

void printWiFiStatus() {
    if (isWiFiConnected()) {
        Serial.print("WiFi connected to: ");
        Serial.println(WiFi.SSID());
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi not connected");
    }
}

void setStartTime() {
    startTime = millis();
}

unsigned long getUptime() {
    return millis() - startTime;
}