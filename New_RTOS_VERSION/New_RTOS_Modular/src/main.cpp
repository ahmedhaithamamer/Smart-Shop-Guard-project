#include <Arduino.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Include all managers
#include "config.h"
#include "wifi_manager.h"
#include "blynk_manager.h"
#include "sensor_manager.h"
#include "actuator_manager.h"
#include "display_manager.h"
#include "task_manager.h"

// Global manager instances
WiFiManager* wifiManager;
BlynkManager* blynkManager;
SensorManager* sensorManager;
ActuatorManager* actuatorManager;
DisplayManager* displayManager;
TaskManager* taskManager;

// Blynk button handlers (need to be defined here for Blynk library)
BLYNK_WRITE(VIRTUAL_PIN_DAY_MODE) {
    BlynkManager::onDayModeChange(request);
}

BLYNK_WRITE(VIRTUAL_PIN_AC) {
    BlynkManager::onACChange(request);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Smart Shop Guard - Modular Version");
    Serial.println("Initializing system...");
    
    // Initialize Wire for I2C communication
    Wire.begin();
    
    // Create manager instances
    wifiManager = new WiFiManager(WIFI_SSID, WIFI_PASSWORD);
    blynkManager = new BlynkManager(BLYNK_AUTH_TOKEN);
    sensorManager = new SensorManager();
    actuatorManager = new ActuatorManager();
    displayManager = new DisplayManager();
    
    // Initialize all managers
    wifiManager->begin();
    blynkManager->begin();
    sensorManager->begin();
    actuatorManager->begin();
    displayManager->begin();
    
    // Create task manager
    taskManager = new TaskManager(wifiManager, blynkManager, sensorManager, actuatorManager, displayManager);
    taskManager->begin();
    
    // Connect to WiFi
    Serial.println("Connecting to WiFi...");
    if (wifiManager->connect()) {
        Serial.println("WiFi connected successfully");
        
        // Connect to Blynk
        Serial.println("Connecting to Blynk...");
        if (blynkManager->connect()) {
            Serial.println("Blynk connected successfully");
        } else {
            Serial.println("Blynk connection failed");
        }
    } else {
        Serial.println("WiFi connection failed");
    }
    
    // Show startup message on displays
    displayManager->showMessage("System Ready", 0);
    displayManager->showStatus("Starting...");
    
    delay(2000); // Give displays time to show startup message
    
    // Start all tasks
    Serial.println("Starting RTOS tasks...");
    taskManager->startTasks();
    
    Serial.println("System initialization complete!");
}

void loop() {
    // Main loop is empty - all functionality handled by RTOS tasks
    // This ensures non-blocking operation
    
    // Optional: Add any main loop logic here if needed
    // For example, emergency stop button monitoring
    
    delay(1000); // Small delay to prevent watchdog issues
} 