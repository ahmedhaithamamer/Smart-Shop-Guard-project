#ifndef ACTUATOR_MANAGER_H
#define ACTUATOR_MANAGER_H

#include <Arduino.h>

class ActuatorManager {
private:
    // Actuator states
    bool fanState;
    bool buzzerState;
    bool relayState;
    
    // Control variables
    bool autoFanControl;
    bool alarmActive;

public:
    ActuatorManager();
    void begin();
    void updateActuators(float temperature, float humidity, bool flameDetected, bool motionDetected, bool isDayMode, bool acMode);
    
    // Individual actuator control
    void controlFan(float temperature, float humidity, bool acMode);
    void controlBuzzer(bool flameDetected, bool motionDetected, bool isDayMode);
    void controlRelay(bool flameDetected);
    
    // Manual control methods
    void setFan(bool state);
    void setBuzzer(bool state);
    void setRelay(bool state);
    void triggerAlarm();
    void stopAlarm();
    
    // Getters
    bool getFanState();
    bool getBuzzerState();
    bool getRelayState();
    bool isAlarmActive();
    
    // Configuration
    void setAutoFanControl(bool enabled);
    bool getAutoFanControl();
};

#endif 