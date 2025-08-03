#include "task_manager.h"
#include "wifi_manager.h"
#include "blynk_manager.h"
#include "sensor_manager.h"
#include "actuator_manager.h"
#include "display_manager.h"
#include "config.h"

// Global pointers for task access
static WiFiManager* g_wifiManager = nullptr;
static BlynkManager* g_blynkManager = nullptr;
static SensorManager* g_sensorManager = nullptr;
static ActuatorManager* g_actuatorManager = nullptr;
static DisplayManager* g_displayManager = nullptr;
static TaskManager* g_taskManager = nullptr; // For accessing non-static methods
static bool g_systemRunning = false; // Static system running flag

TaskManager::TaskManager(WiFiManager* wifi, BlynkManager* blynk, SensorManager* sensor, 
                        ActuatorManager* actuator, DisplayManager* display) {
    wifiManager = wifi;
    blynkManager = blynk;
    sensorManager = sensor;
    actuatorManager = actuator;
    displayManager = display;
    
    // Initialize global pointers
    g_wifiManager = wifi;
    g_blynkManager = blynk;
    g_sensorManager = sensor;
    g_actuatorManager = actuator;
    g_displayManager = display;
    g_taskManager = this;
    
    tasksCreated = false;
    systemRunning = false;
    g_systemRunning = false;
}

void TaskManager::begin() {
    Serial.println("Task Manager initialized");
}

void TaskManager::createTasks() {
    if (tasksCreated) return;
    
    // Create tasks with appropriate priorities and stack sizes
    xTaskCreatePinnedToCore(
        blynkTask, "BlynkTask", TASK_STACK_SIZE_MEDIUM, NULL, TASK_PRIORITY_HIGH, &blynkTaskHandle, 1
    );
    
    xTaskCreatePinnedToCore(
        sensorTask, "SensorTask", TASK_STACK_SIZE_MEDIUM, NULL, TASK_PRIORITY_MEDIUM, &sensorTaskHandle, 1
    );
    
    xTaskCreatePinnedToCore(
        actuatorTask, "ActuatorTask", TASK_STACK_SIZE_SMALL, NULL, TASK_PRIORITY_MEDIUM, &actuatorTaskHandle, 1
    );
    
    xTaskCreatePinnedToCore(
        displayTask, "DisplayTask", TASK_STACK_SIZE_SMALL, NULL, TASK_PRIORITY_LOW, &displayTaskHandle, 1
    );
    
    xTaskCreatePinnedToCore(
        systemTask, "SystemTask", TASK_STACK_SIZE_SMALL, NULL, TASK_PRIORITY_LOW, &systemTaskHandle, 1
    );
    
    tasksCreated = true;
    systemRunning = true;
    g_systemRunning = true;
    Serial.println("All tasks created successfully");
}

void TaskManager::startTasks() {
    if (!tasksCreated) {
        createTasks();
    }
    systemRunning = true;
    g_systemRunning = true;
}

void TaskManager::stopTasks() {
    systemRunning = false;
    g_systemRunning = false;
    
    if (blynkTaskHandle != NULL) {
        vTaskDelete(blynkTaskHandle);
        blynkTaskHandle = NULL;
    }
    
    if (sensorTaskHandle != NULL) {
        vTaskDelete(sensorTaskHandle);
        sensorTaskHandle = NULL;
    }
    
    if (actuatorTaskHandle != NULL) {
        vTaskDelete(actuatorTaskHandle);
        actuatorTaskHandle = NULL;
    }
    
    if (displayTaskHandle != NULL) {
        vTaskDelete(displayTaskHandle);
        displayTaskHandle = NULL;
    }
    
    if (systemTaskHandle != NULL) {
        vTaskDelete(systemTaskHandle);
        systemTaskHandle = NULL;
    }
    
    tasksCreated = false;
    Serial.println("All tasks stopped");
}

// Task implementations
void TaskManager::blynkTask(void* parameter) {
    while (g_blynkManager != nullptr && g_systemRunning) {
        if (g_wifiManager != nullptr) {
            g_wifiManager->checkConnection();
        }
        
        if (g_blynkManager != nullptr) {
            g_blynkManager->checkConnection();
            g_blynkManager->run();
        }
        
        vTaskDelay(pdMS_TO_TICKS(BLYNK_DELAY));
    }
    vTaskDelete(NULL);
}

void TaskManager::sensorTask(void* parameter) {
    while (g_sensorManager != nullptr && g_systemRunning) {
        g_sensorManager->updateSensors();
        
        // Send sensor data to Blynk
        if (g_blynkManager != nullptr) {
            g_blynkManager->writeVirtualPin(VIRTUAL_PIN_TEMP, g_sensorManager->getTemperature());
            g_blynkManager->writeVirtualPin(VIRTUAL_PIN_HUMIDITY, g_sensorManager->getHumidity());
            g_blynkManager->writeVirtualPin(VIRTUAL_PIN_MOTION, g_sensorManager->getMotionStatus());
            g_blynkManager->writeVirtualPin(VIRTUAL_PIN_FLAME, !g_sensorManager->getFlameStatus());
        }
        
        vTaskDelay(pdMS_TO_TICKS(TEMP_HUMIDITY_DELAY));
    }
    vTaskDelete(NULL);
}

void TaskManager::actuatorTask(void* parameter) {
    while (g_actuatorManager != nullptr && g_sensorManager != nullptr && g_systemRunning) {
        g_actuatorManager->updateActuators(
            g_sensorManager->getTemperature(),
            g_sensorManager->getHumidity(),
            g_sensorManager->isFlameDetected(),
            g_sensorManager->isMotionDetected(),
            g_blynkManager->getDayMode(), // Get day mode from BlynkManager
            g_blynkManager->getACMode()   // Get AC mode from BlynkManager
        );
        
        vTaskDelay(pdMS_TO_TICKS(FLAME_DELAY));
    }
    vTaskDelete(NULL);
}

void TaskManager::displayTask(void* parameter) {
    while (g_displayManager != nullptr && g_systemRunning) {
        if (g_sensorManager != nullptr && g_wifiManager != nullptr && g_blynkManager != nullptr) {
            g_displayManager->updateDisplays(
                g_sensorManager->getTemperature(),
                g_sensorManager->getHumidity(),
                g_blynkManager->getDayMode(), // Get day mode from BlynkManager
                g_wifiManager->getStatus(),
                g_blynkManager->getStatus(),
                "Running"
            );
        }
        
        vTaskDelay(pdMS_TO_TICKS(LCD_DELAY));
    }
    vTaskDelete(NULL);
}

void TaskManager::systemTask(void* parameter) {
    while (g_systemRunning) {
        // System health monitoring
        if (g_taskManager != nullptr) {
            g_taskManager->checkTaskHealth();
        }
        
        // Print system status every 30 seconds
        static unsigned long lastStatusPrint = 0;
        if (millis() - lastStatusPrint > 30000) {
            if (g_taskManager != nullptr) {
                g_taskManager->printTaskStatus();
            }
            lastStatusPrint = millis();
        }
        
        vTaskDelay(pdMS_TO_TICKS(5000)); // Check every 5 seconds
    }
    vTaskDelete(NULL);
}

bool TaskManager::isSystemRunning() {
    return systemRunning;
}

void TaskManager::restartSystem() {
    Serial.println("Restarting system...");
    stopTasks();
    delay(1000);
    startTasks();
}

void TaskManager::emergencyStop() {
    Serial.println("EMERGENCY STOP!");
    systemRunning = false;
    g_systemRunning = false;
    
    // Stop all actuators
    if (g_actuatorManager != nullptr) {
        g_actuatorManager->setFan(false);
        g_actuatorManager->setBuzzer(false);
        g_actuatorManager->setRelay(false);
    }
    
    stopTasks();
}

void TaskManager::checkTaskHealth() {
    // Check if tasks are still running
    if (blynkTaskHandle != NULL && eTaskGetState(blynkTaskHandle) == eDeleted) {
        Serial.println("Warning: Blynk task deleted");
    }
    
    if (sensorTaskHandle != NULL && eTaskGetState(sensorTaskHandle) == eDeleted) {
        Serial.println("Warning: Sensor task deleted");
    }
    
    if (actuatorTaskHandle != NULL && eTaskGetState(actuatorTaskHandle) == eDeleted) {
        Serial.println("Warning: Actuator task deleted");
    }
    
    if (displayTaskHandle != NULL && eTaskGetState(displayTaskHandle) == eDeleted) {
        Serial.println("Warning: Display task deleted");
    }
}

void TaskManager::printTaskStatus() {
    Serial.println("=== System Status ===");
    Serial.print("System Running: "); Serial.println(systemRunning ? "YES" : "NO");
    Serial.print("Tasks Created: "); Serial.println(tasksCreated ? "YES" : "NO");
    
    if (g_sensorManager != nullptr) {
        Serial.print("Temperature: "); Serial.print(g_sensorManager->getTemperature()); Serial.println("°C");
        Serial.print("Humidity: "); Serial.print(g_sensorManager->getHumidity()); Serial.println("%");
        Serial.print("Flame Detected: "); Serial.println(g_sensorManager->isFlameDetected() ? "YES" : "NO");
        Serial.print("Motion Detected: "); Serial.println(g_sensorManager->isMotionDetected() ? "YES" : "NO");
    }
    
    if (g_actuatorManager != nullptr) {
        Serial.print("Fan State: "); Serial.println(g_actuatorManager->getFanState() ? "ON" : "OFF");
        Serial.print("Alarm Active: "); Serial.println(g_actuatorManager->isAlarmActive() ? "YES" : "NO");
    }
    
    Serial.println("===================");
} 