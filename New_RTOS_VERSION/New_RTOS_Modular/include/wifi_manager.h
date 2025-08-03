#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <Arduino.h>

class WiFiManager {
private:
    const char* ssid;
    const char* password;
    unsigned long lastReconnectAttempt;
    unsigned long reconnectInterval;
    bool isConnected;

public:
    WiFiManager(const char* ssid, const char* password);
    bool begin();
    bool connect();
    bool isWiFiConnected();
    void checkConnection();
    void reconnect();
    String getStatus();
    void setCredentials(const char* newSsid, const char* newPassword);
};

#endif 