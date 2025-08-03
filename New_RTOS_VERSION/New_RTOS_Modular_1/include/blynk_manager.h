#ifndef BLYNK_MANAGER_H
#define BLYNK_MANAGER_H

#include <Arduino.h>

// Forward declarations to avoid including Blynk headers in header file

class BlynkManager {
private:
    bool isConnected;
    unsigned long lastReconnectAttempt;
    unsigned long reconnectInterval;
    String authToken;
    bool isDay;
    bool AC;

public:
    BlynkManager(const char* authToken);
    bool begin();
    bool connect();
    bool isBlynkConnected();
    void checkConnection();
    void reconnect();
    String getStatus();
    void run();
    bool getDayMode();
    bool getACMode();
    void setDayMode(bool dayMode);
    void setACMode(bool acMode);

    
    // Virtual pin operations
    void writeVirtualPin(int pin, int value);
    void writeVirtualPin(int pin, float value);
    void writeVirtualPin(int pin, const char* value);
    
    // Blynk button handlers are now implemented as BLYNK_WRITE callbacks in the .cpp file
};



#endif 