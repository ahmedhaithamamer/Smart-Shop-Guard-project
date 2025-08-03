#include "wifi_manager.h"
#include "config.h"

WiFiManager::WiFiManager(const char* ssid, const char* password) {
    this->ssid = ssid;
    this->password = password;
    this->lastReconnectAttempt = 0;
    this->reconnectInterval = 5000; // 5 seconds
    this->isConnected = false;
}

bool WiFiManager::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi...");
    return true;
}

bool WiFiManager::connect() {
    int attempts = 0;
    const int maxAttempts = 20;
    
    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("WiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        isConnected = true;
        return true;
    } else {
        Serial.println();
        Serial.println("WiFi connection failed!");
        isConnected = false;
        return false;
    }
}

bool WiFiManager::isWiFiConnected() {
    isConnected = (WiFi.status() == WL_CONNECTED);
    return isConnected;
}

void WiFiManager::checkConnection() {
    if (!isWiFiConnected()) {
        unsigned long currentTime = millis();
        if (currentTime - lastReconnectAttempt >= reconnectInterval) {
            Serial.println("WiFi disconnected. Attempting to reconnect...");
            reconnect();
            lastReconnectAttempt = currentTime;
        }
    }
}

void WiFiManager::reconnect() {
    WiFi.disconnect();
    delay(1000);
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    const int maxAttempts = 10;
    
    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("WiFi reconnected!");
        isConnected = true;
    } else {
        Serial.println();
        Serial.println("WiFi reconnection failed!");
        isConnected = false;
    }
}

String WiFiManager::getStatus() {
    if (isWiFiConnected()) {
        return "Connected";
    } else {
        return "Disconnected";
    }
}

void WiFiManager::setCredentials(const char* newSsid, const char* newPassword) {
    this->ssid = newSsid;
    this->password = newPassword;
} 