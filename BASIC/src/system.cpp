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
    Serial.print("Connecting to WiFi");
    
    // Try for 15 seconds instead of default timeout
    while (WiFi.status() != WL_CONNECTED && millis() - wifiConnectStart < 15000) {
        delay(500);
        Serial.print(".");
        yield(); // Allow other tasks to run
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("WiFi connected successfully!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        Serial.println();
        Serial.println("WiFi connection timeout after 15 seconds");
        Serial.println("Continuing without internet connection...");
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

// Global variables for WiFi reconnection
unsigned long lastWiFiReconnectAttempt = 0;
const unsigned long wifiReconnectInterval = 30000; // 30 seconds
bool wifiReconnecting = false;

void handleWiFiReconnection() {
    // Only try reconnection if not currently connected and enough time has passed
    if (!isWiFiConnected() && 
        (millis() - lastWiFiReconnectAttempt > wifiReconnectInterval) &&
        !wifiReconnecting) {
        
        wifiReconnecting = true;
        lastWiFiReconnectAttempt = millis();
        
        Serial.println("Attempting WiFi reconnection...");
        
        // Quick reconnection attempt (5 seconds max)
        WiFi.disconnect();
        delay(100);
        WiFi.begin(ssid, pass);
        
        unsigned long reconnectStart = millis();
        while (WiFi.status() != WL_CONNECTED && 
               millis() - reconnectStart < 5000) {
            delay(250);
            yield(); // Allow other tasks to run
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("WiFi reconnected successfully!");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("WiFi reconnection failed, will try again in 30 seconds");
        }
        
        wifiReconnecting = false;
    }
}

String getWiFiStatus() {
    if (wifiReconnecting) {
        return "WiFi: Connecting...";
    } else if (isWiFiConnected()) {
        return "WiFi: Connected";
    } else {
        // Show countdown to next attempt
        unsigned long timeSinceLastAttempt = millis() - lastWiFiReconnectAttempt;
        if (timeSinceLastAttempt < wifiReconnectInterval) {
            unsigned long timeToNext = (wifiReconnectInterval - timeSinceLastAttempt) / 1000;
            return "WiFi: Retry in " + String(timeToNext) + "s";
        } else {
            return "WiFi: Disconnected";
        }
    }
}