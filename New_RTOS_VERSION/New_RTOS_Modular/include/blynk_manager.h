#ifndef BLYNK_MANAGER_H
#define BLYNK_MANAGER_H

#include <BlynkSimpleEsp32.h>
#include <Arduino.h>

class BlynkManager {
private:
    bool isConnected;
    unsigned long lastReconnectAttempt;
    unsigned long reconnectInterval;
    String authToken;

public:
    BlynkManager(const char* authToken);
    bool begin();
    bool connect();
    bool isBlynkConnected();
    void checkConnection();
    void reconnect();
    String getStatus();
    void run();
    
    // Virtual pin operations
    void writeVirtualPin(int pin, int value);
    void writeVirtualPin(int pin, float value);
    void writeVirtualPin(int pin, const char* value);
    
    // Blynk button handlers
    static void onDayModeChange(BlynkReq& request);
    static void onACChange(BlynkReq& request);
};

// Global variables for Blynk callbacks
extern bool isDay;
extern bool AC;

#endif 