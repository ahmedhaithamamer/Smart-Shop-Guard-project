// 🏪 Smart Shop Guard - ESP32 FreeRTOS Implementation
// 🚀 Dual-core architecture for optimal performance and reliability
// 🛡️ Advanced security monitoring with real-time sensor processing

#include "config.h"
#include "system.h"
#include "sensors.h"
#include "actuators.h"
#include "display.h"        // LCD display functions
#include "oled_display.h"   // OLED display functions
#include "audio.h"
#include "blynk_handlers.h"

#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP32Servo.h>         
#include <WiFi.h>
#include "blynk_instance.h"
// #include "esp_task_wdt.h" // Removed - watchdog disabled for stability

// 🔧 Hardware Interface Objects
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);  // 16x2 LCD Display
DHT dht(DHTPIN, DHTTYPE);                                    // Temperature & Humidity Sensor
Servo myServo;                                               // Security Door Control

// 🌐 Network Configuration
char ssid[] = WIFI_SSID;      // WiFi Network Name
char pass[] = WIFI_PASSWORD;  // WiFi Password

// 📊 System State Variables
int h = 0;              // Current Humidity Percentage
int t = 0;              // Current Temperature (°C)
long duration = 0;      // Ultrasonic Sensor Timing
int distance = 0;       // Distance Measurement (cm)
int degree = 0;         // Fan Rotation Angle
int Servodegree = 0;    // Servo Motor Position
unsigned long lastTime = 0;        // Last Sensor Update
unsigned long startTime = 0;       // System Uptime Counter
bool isDay = false;                // Day/Night Mode Flag
bool AC = false;                   // Air Conditioning Status
unsigned long lastStatusPrint = 0; // Last Status Display Update
unsigned long lastOLEDUpdate = 0;  // Last OLED Screen Update

// 🚨 Alert State Management
static bool fireAlertActive = false;    // Fire Detection Active Flag
static bool motionAlertActive = false;  // Motion Detection Active Flag

// 🔄 FreeRTOS Communication Resources
QueueHandle_t sensorDataQueue = nullptr;  // Sensor Data Transfer Queue
QueueHandle_t eventQueue = nullptr;       // Event Notification Queue
SemaphoreHandle_t i2cMutex = nullptr;    // I2C Bus Access Protection
SemaphoreHandle_t dataMutex = nullptr;   // Shared Data Access Protection

// ⏱️ Performance Optimization Settings
const unsigned long oledUpdateInterval = 1000;  // OLED Refresh Rate (ms)

// 📋 Task Management Handles
static TaskHandle_t hTaskSensors = nullptr;    // Sensor Polling Task
static TaskHandle_t hTaskActuators = nullptr;  // Actuator Control Task
static TaskHandle_t hTaskLCD = nullptr;        // LCD Display Task
static TaskHandle_t hTaskOLED = nullptr;       // OLED Display Task
static TaskHandle_t hTaskWiFi = nullptr;       // WiFi & Blynk Task
static TaskHandle_t hTaskSysMon = nullptr;     // System Monitor Task

// 📚 Legacy Function Declarations (FreeRTOS Migration)
// These functions have been replaced by RTOS task-based architecture
// void processNightMode();    // Now handled by TaskActuators
// void processDayMode();      // Now handled by TaskActuators  
// void fanTempLCD();          // Now handled by TaskLCD

// 🌐 Network Management Task (Core 0)
// Handles WiFi connectivity, Blynk cloud communication, and sensor data transmission
void TaskWiFiBlynk(void* pvParameters) {
  // esp_task_wdt_add(NULL);  // Watchdog disabled
  Serial.printf("[CORE %d] TaskWiFiBlynk started\n", xPortGetCoreID());
  SensorData latest{};     // Latest sensor data buffer
  unsigned long lastBlynkSend = 0;           // Last transmission timestamp
  const unsigned long blynkSendInterval = 2000;  // Cloud data update interval (ms)
  
  for(;;) {
    // Handle WiFi reconnection
    handleWiFiReconnection();
    
    // Run Blynk if connected
    if (isWiFiConnected()) {
      Blynk.run();
      
      // Send sensor data periodically
      if (millis() - lastBlynkSend >= blynkSendInterval) {
        if (xQueuePeek(sensorDataQueue, &latest, 0) == pdTRUE) {
          sendSensorDataToBlynk(latest.temperatureC, latest.humidityPct, latest.flame, latest.pirMotion);
          lastBlynkSend = millis();
        }
      }
    }
    
    // esp_task_wdt_reset(); // Watchdog disabled
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// 📺 LCD Display Management Task (Core 1)
// Manages 16x2 LCD screen updates and status display coordination
void TaskLCD(void* pvParameters) {
  // esp_task_wdt_add(NULL);  // Watchdog disabled
  Serial.printf("[CORE %d] TaskLCD started\n", xPortGetCoreID());
  static unsigned long lastNormalUpdate = 0;  // Normal status update timestamp
  
  for(;;) {
    // Only update normal status if no alerts are active
    if (!fireAlertActive && !motionAlertActive && (millis() - lastNormalUpdate > 5000)) {
      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        displayNormalStatus();
        xSemaphoreGive(i2cMutex);
        lastNormalUpdate = millis();
      }
    }
    
    // esp_task_wdt_reset(); // Watchdog disabled
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// 🖥️ OLED Display Management Task (Core 1)
// Handles 128x64 OLED screen updates, button navigation, and page management
void TaskOLED(void* pvParameters) {
  // esp_task_wdt_add(NULL);  // Watchdog disabled
  Serial.printf("[CORE %d] TaskOLED started\n", xPortGetCoreID());
  for(;;) {
    handleOLEDButtons();
    // refresh page if no hazards (updateOLEDDisplay internally draws current page)
    if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
      updateOLEDDisplay();
      xSemaphoreGive(i2cMutex);
    }
    // esp_task_wdt_reset(); // Watchdog disabled
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

// 🔍 Sensor Data Collection Task (Core 1)
// Continuously monitors all sensors with intelligent debouncing and data processing
void TaskSensorPoll(void* pvParameters) {
  // esp_task_wdt_add(NULL);  // Watchdog disabled
  Serial.printf("[CORE %d] TaskSensorPoll started\n", xPortGetCoreID());
  SensorData msg{};         // Sensor data message buffer
  // 🔥 Fire Detection Stability Tracking
  static bool fireStable = false;        // Stable fire state
  static int fireChangeCounter = 0;      // Fire state change counter
  for(;;) {
    readTemperatureHumidity();
    msg.temperatureC = t;
    msg.humidityPct = h;

    readFlameSensor();
    bool fireRaw = isFlameDetected();

    // Debounce fire: require 3 consecutive samples to change state
    if (fireRaw != fireStable) {
      fireChangeCounter++;
      if (fireChangeCounter >= 3) {
        fireStable = fireRaw;
        Event e{fireStable ? EVENT_FIRE_DETECTED : EVENT_FIRE_CLEARED, (uint32_t)millis()};
        xQueueSend(eventQueue, &e, 0);
        fireChangeCounter = 0;
      }
    } else {
      fireChangeCounter = 0;
    }

    msg.flame = fireStable;

    // Read motion with debouncing (this will send events internally)
    readMotion();
    
    triggerUltrasonicSensor();
    msg.distanceCm = getDistance();
    msg.pirMotion = isMotionDetected();
    msg.tsMs = millis();
    xQueueOverwrite(sensorDataQueue, &msg);

    // esp_task_wdt_reset(); // Watchdog disabled
    vTaskDelay(pdMS_TO_TICKS(250));
  }
}

// ⚡ Actuator Control & Alert Management Task (Core 1)
// Processes emergency events and controls all output devices (relay, fan, servo)
void TaskActuators(void* pvParameters) {
  // esp_task_wdt_add(NULL);  // Watchdog disabled
  Serial.printf("[CORE %d] TaskActuators started\n", xPortGetCoreID());
  SensorData latest{};      // Latest sensor data buffer
  Event ev{};               // Event notification buffer
  for(;;) {
    if (xQueueReceive(eventQueue, &ev, pdMS_TO_TICKS(10)) == pdTRUE) {
      switch (ev.type) {
        case EVENT_FIRE_DETECTED:
          fireAlertActive = true;
          // Queue fire alert audio for Core 0
          if (audioQueue) {
            AudioEvent fireAudio{AUDIO_FIRE_ALERT, (uint32_t)millis()};
            xQueueSend(audioQueue, &fireAudio, 0);
          }
          activateRelay();
          if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            displayFireAlert();
            displayOLEDFireAlert();
            xSemaphoreGive(i2cMutex);
          }
          Serial.println("[CORE 1] FIRE ALERT ACTIVATED - Display locked, audio queued");
          break;
        case EVENT_FIRE_CLEARED:
          fireAlertActive = false;
          deactivateRelay();
          if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            displaySafeStatus();
            displayOLEDSafeStatus();
            xSemaphoreGive(i2cMutex);
          }
          Serial.println("[CORE 1] FIRE ALERT CLEARED - Display returned to normal");
          break;
        case EVENT_MOTION_DETECTED:
          motionAlertActive = true;
          // Queue motion alert audio for Core 0
          if (audioQueue) {
            AudioEvent motionAudio{AUDIO_MOTION_ALERT, (uint32_t)millis()};
            xQueueSend(audioQueue, &motionAudio, 0);
          }
          if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            displayThiefAlert();
            displayOLEDThiefAlert();
            xSemaphoreGive(i2cMutex);
          }
          Serial.println("[CORE 1] MOTION ALERT ACTIVATED - Display locked, audio queued");
          break;
        case EVENT_MOTION_CLEARED:
          motionAlertActive = false;
          if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            displayNormalStatus();
            displayOLEDMotionCleared();
            xSemaphoreGive(i2cMutex);
          }
          Serial.println("[CORE 1] MOTION ALERT CLEARED - Display returned to normal");
          break;
        default: break;
      }
    }
    // consume latest sensor sample for fan control
    if (xQueuePeek(sensorDataQueue, &latest, 0) == pdTRUE) {
      controlFan(latest.temperatureC, latest.humidityPct);
      moveServo();
    }
    // esp_task_wdt_reset(); // Watchdog disabled
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

// PIR ISR removed - motion detection now handled in TaskSensorPoll with debouncing

// Audio event queue for Core 0 audio processing
QueueHandle_t audioQueue = nullptr;

// 📊 System Health Monitoring & Audio Processing Task (Core 0)
// Monitors system resources, processes audio events, and provides system diagnostics
void TaskSystemMonitor(void* pvParameters) {
  // esp_task_wdt_add(NULL);  // Watchdog disabled
  Serial.printf("[CORE %d] TaskSystemMonitor started\n", xPortGetCoreID());
  AudioEvent audioEvent{};
  unsigned long lastHeapReport = 0;
  unsigned long lastTaskReport = 0;
  
  for(;;) {
    // Process audio events
    if (xQueueReceive(audioQueue, &audioEvent, pdMS_TO_TICKS(10)) == pdTRUE) {
      switch (audioEvent.type) {
        case AUDIO_STARTUP:
          Serial.println("[CORE 0] Playing startup tone");
          playStartupTone();
          break;
        case AUDIO_MODE_SWITCH:
          Serial.println("[CORE 0] Playing mode switch tone");
          playModeSwitchTone();
          break;
        case AUDIO_FIRE_ALERT:
          Serial.println("[CORE 0] Playing fire alert tone");
          playAlertTone();
          break;
        case AUDIO_MOTION_ALERT:
          Serial.println("[CORE 0] Playing motion alert tone");
          playAlertTone();
          break;
        default: break;
      }
    }
    
    // System monitoring (every 30 seconds)
    if (millis() - lastHeapReport > 30000) {
      Serial.printf("[CORE 0] Free heap: %d bytes, Min free: %d bytes\n", 
                    ESP.getFreeHeap(), ESP.getMinFreeHeap());
      lastHeapReport = millis();
    }
    
    // Task stack monitoring (every 60 seconds)
    if (millis() - lastTaskReport > 60000) {
      if (hTaskSensors) {
        Serial.printf("[CORE 0] TaskSensor stack high water: %d\n", 
                      uxTaskGetStackHighWaterMark(hTaskSensors));
      }
      if (hTaskActuators) {
        Serial.printf("[CORE 0] TaskActuators stack high water: %d\n", 
                      uxTaskGetStackHighWaterMark(hTaskActuators));
      }
      if (hTaskWiFi) {
        Serial.printf("[CORE 0] TaskWiFi stack high water: %d\n", 
                      uxTaskGetStackHighWaterMark(hTaskWiFi));
      }
      lastTaskReport = millis();
    }
    
    // esp_task_wdt_reset(); // Watchdog disabled
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// 🚀 System Initialization & Task Creation
// Sets up hardware, creates FreeRTOS resources, and launches all system tasks
void setup() {
  // 🛡️ Watchdog Timer Configuration - DISABLED
  // esp_task_wdt_init(10, true);  // Disabled for better stability
  // Note: Software watchdog removed, hardware watchdog still provides protection
  
  // ⏱️ Power Stabilization Delay
  vTaskDelay(pdMS_TO_TICKS(1000));  // Allow power supply to stabilize
  
  // 🔧 System Component Initialization
  initSystem();  // Initialize core system functions
  
  // 🔄 FreeRTOS Resource Creation
  sensorDataQueue = xQueueCreate(1, sizeof(SensorData));    // Sensor data transfer queue
  eventQueue = xQueueCreate(10, sizeof(Event));             // Event notification queue
  audioQueue = xQueueCreate(5, sizeof(AudioEvent));         // Audio event processing queue
  i2cMutex = xSemaphoreCreateMutex();                       // I2C bus access protection
  dataMutex = xSemaphoreCreateMutex();                      // Shared data access protection
  
  // 📺 Display System Initialization
  Serial.println("🖥️ Initializing display systems...");
  initDisplay();             // Initialize 16x2 LCD display
  initOLEDDisplay();         // Initialize 128x64 OLED display
  
  // 🎉 Welcome Message Display
  Serial.println("📝 Displaying welcome messages...");
  displayWelcomeMessage();      // LCD welcome message
  displayOLEDWelcomeMessage();  // OLED welcome animation
  
  // 🔍 Sensor System Initialization
  Serial.println("🔍 Initializing sensor systems...");
  initSensors();  // Initialize all monitoring sensors
  // Note: PIR interrupt removed - motion detection now handled in TaskSensorPoll
  
  // ⚡ Actuator & Audio System Setup
  initActuators();  // Initialize relay, fan, and servo motors
  initAudio();      // Initialize buzzer and audio system
  
  // 🔊 Startup Audio Queue
  AudioEvent startupAudio{AUDIO_STARTUP, (uint32_t)millis()};
  if (audioQueue) {
    xQueueSend(audioQueue, &startupAudio, 0);
    Serial.println("🔊 Startup tone queued for Core 0 audio processing");
  }
  
  // 🌐 Network & Cloud Services Setup
  Serial.println("🌐 Attempting WiFi connection (5 second timeout)...");
  if (initWiFi()) {
    Serial.println("✅ WiFi connected, initializing Blynk cloud service...");
    initBlynk();
    connectBlynk();
    Serial.println("☁️ Cloud services initialized successfully!");
  } else {
    Serial.println("⚠️ Starting in offline mode...");
    Serial.println("🔄 System will attempt WiFi reconnection every 30 seconds");
  }
  
  // 🎯 Task Creation & Core Assignment
  Serial.println("=== 🚀 Task Creation with Optimized Core Assignment ===");
  Serial.println("🔥 Core 0: WiFi + Blynk, system monitoring, alerts/buzzer");
  Serial.println("⚡ Core 1: Sensors, LCD, OLED, actuators, motion processing");
  
  // 🔥 Core 1: Real-time Processing Tasks (High Priority)
  xTaskCreatePinnedToCore(TaskSensorPoll, "tSensors", 4096, nullptr, 4, &hTaskSensors, 1);
  xTaskCreatePinnedToCore(TaskActuators,  "tAct",     4096, nullptr, 5, &hTaskActuators, 1);
  xTaskCreatePinnedToCore(TaskLCD,        "tLCD",     3072, nullptr, 2, &hTaskLCD,      1);
  xTaskCreatePinnedToCore(TaskOLED,       "tOLED",    3072, nullptr, 2, &hTaskOLED,     1);
  
  // 🌐 Core 0: Network & System Services (Background Priority)
  xTaskCreatePinnedToCore(TaskWiFiBlynk,     "tWiFi",   4096, nullptr, 3, &hTaskWiFi,   0);
  xTaskCreatePinnedToCore(TaskSystemMonitor, "tSysMon", 3072, nullptr, 1, &hTaskSysMon, 0);
  
  // 📊 System Resource Information
  Serial.printf("💾 Free heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("⚡ CPU0 freq: %d MHz, CPU1 freq: %d MHz\n", ESP.getCpuFreqMHz(), ESP.getCpuFreqMHz());
  Serial.println("🎯 Setup complete! System ready for operation.");
}

// 🔄 Main Loop (Minimal in FreeRTOS Design)
// In RTOS architecture, most work is done by dedicated tasks
// This loop provides system heartbeat
void loop() {
  // esp_task_wdt_reset();  // Watchdog disabled for stability
  vTaskDelay(pdMS_TO_TICKS(1000));  // 1-second system heartbeat
}
