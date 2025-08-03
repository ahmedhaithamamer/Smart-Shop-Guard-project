#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Arduino.h>

// Forward declarations
class WiFiManager;
class BlynkManager;
class SensorManager;
class ActuatorManager;
class DisplayManager;

class TaskManager {
private:
    WiFiManager* wifiManager;
    BlynkManager* blynkManager;
    SensorManager* sensorManager;
    ActuatorManager* actuatorManager;
    DisplayManager* displayManager;
    
    // Task handles
    TaskHandle_t blynkTaskHandle;
    TaskHandle_t sensorTaskHandle;
    TaskHandle_t actuatorTaskHandle;
    TaskHandle_t displayTaskHandle;
    TaskHandle_t systemTaskHandle;
    
    // Task status
    bool tasksCreated;
    bool systemRunning;

public:
    TaskManager(WiFiManager* wifi, BlynkManager* blynk, SensorManager* sensor, 
                ActuatorManager* actuator, DisplayManager* display);
    void begin();
    void createTasks();
    void startTasks();
    void stopTasks();
    
    // Task functions (static for RTOS compatibility)
    static void blynkTask(void* parameter);
    static void sensorTask(void* parameter);
    static void actuatorTask(void* parameter);
    static void displayTask(void* parameter);
    static void systemTask(void* parameter);
    
    // System control
    bool isSystemRunning();
    void restartSystem();
    void emergencyStop();
    
    // Task monitoring
    void checkTaskHealth();
    void printTaskStatus();
};

#endif 