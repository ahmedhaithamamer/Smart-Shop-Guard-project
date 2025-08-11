// Smart Shop Guard - Dual Display Version
// Main application file with both LCD and OLED support

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
#include "esp_task_wdt.h"

// Global hardware objects
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
DHT dht(DHTPIN, DHTTYPE);
Servo myServo;

// WiFi credentials
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

// Global variables
int h = 0;              // Humidity
int t = 0;              // Temperature
long duration = 0;
int distance = 0;
int degree = 0;
int Servodegree = 0;
unsigned long lastTime = 0;
unsigned long startTime = 0;
bool isDay = false;
bool AC = false;
unsigned long lastStatusPrint = 0;
unsigned long lastOLEDUpdate = 0;

// OLED update interval (reduce CPU usage)
const unsigned long oledUpdateInterval = 1000; // Update every 1000ms (reduced since we have immediate updates)

// Function declarations for main logic
void processNightMode();
void processDayMode();
void fanTempLCD();

void setup() {
  // Configure watchdog timer for stability
  esp_task_wdt_init(10, true);  // 10 second timeout
  esp_task_wdt_add(NULL);       // Add current task to watchdog
  
  // Add small delay for power stabilization
  delay(1000);
  
  // Initialize system components
  initSystem();
  
  // Initialize hardware modules with watchdog resets
  Serial.println("Initializing displays...");
  initDisplay();      // Initialize LCD
  esp_task_wdt_reset();
  
  initOLEDDisplay();  // Initialize OLED
  esp_task_wdt_reset();
  
  Serial.println("Initializing sensors...");
  initSensors();
  esp_task_wdt_reset();
  
  initActuators();
  esp_task_wdt_reset();
  
  initAudio();
  esp_task_wdt_reset();
  
  // Initialize WiFi and Blynk with power management
  Serial.println("Attempting WiFi connection (5 second timeout)...");
  esp_task_wdt_reset();
  
  if (initWiFi()) {
    Serial.println("WiFi connected, initializing Blynk...");
    esp_task_wdt_reset();
    initBlynk();
    connectBlynk();
    esp_task_wdt_reset();
    Serial.println("Cloud services initialized successfully!");
  } else {
    Serial.println("Starting in offline mode...");
    Serial.println("System will attempt WiFi reconnection every 30 seconds");
  }
  
  // Play startup sequence
  playStartupTone();
  esp_task_wdt_reset();
  
  // Show welcome on both displays (with reduced delays)
  Serial.println("Showing welcome messages...");
  displayWelcomeMessage();  // LCD welcome (blinking)
  esp_task_wdt_reset();
  // OLED welcome is handled in initOLEDDisplay() with intro animation
  
  displayModeStatus();  // Show on LCD
  esp_task_wdt_reset();
  displayOLEDModeStatus();  // Show on OLED
  esp_task_wdt_reset();
  
  // Print system information for debugging
  Serial.println("=== System Information ===");
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("CPU frequency: %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Flash size: %d bytes\n", ESP.getFlashChipSize());
  
  // Enhanced PSRAM detection
  Serial.printf("PSRAM size: %d bytes\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
  Serial.printf("PSRAM found: %s\n", psramFound() ? "YES" : "NO");
  
  #ifdef BOARD_HAS_PSRAM
    Serial.println("PSRAM support compiled in");
  #else
    Serial.println("PSRAM support NOT compiled in");
  #endif
  
  Serial.println("Setup complete!");
}

void loop() {
  // Reset watchdog timer
  esp_task_wdt_reset();
  
  // Handle WiFi reconnection in background
  handleWiFiReconnection();
  
  // Only run Blynk if WiFi is connected
  if (isWiFiConnected()) {
    Blynk.run();
  }
  
  // Handle OLED button navigation and updates
  handleOLEDButtons();
  
  // Update OLED display at regular intervals (for page navigation)
  // Critical updates (alerts, temp changes) are handled immediately via displayOLED* functions
  if (millis() - lastOLEDUpdate > oledUpdateInterval) {
    // Only update if no hazards are active (to prevent overriding hazard display)
    extern bool fireDetected, motionDetected;
    if (!fireDetected && !motionDetected) {
      updateOLEDDisplay();
    }
    lastOLEDUpdate = millis();
  }
  
  if (!isDay) {
    processNightMode();
  } else {
    processDayMode();
  }
  
  // Small delay to prevent watchdog timeout and reduce CPU usage
  delay(10);
  yield(); // Allow other tasks to run
}

void processNightMode() {
  // Read sensors and update display
  fanTempLCD();
  
  // Check flame sensor
  readFlameSensor();
  if (isFlameDetected()) {
    playAlertTone();
    activateRelay();
    
    // SYNC BOTH DISPLAYS - Fire Alert
    displayFireAlert();      // LCD: FIRE ALERT! + EVACUATE NOW!
    displayOLEDFireAlert();  // OLED: Switches to alerts page, shows fire alert
    
    Serial.println("=== FIRE HAZARD ACTIVE ===");
    Serial.println("LCD: Fire alert displayed");
    Serial.println("OLED: Fire alert displayed - page cycling paused");
  } else {
    deactivateRelay();
    
    // Check if fire was previously detected (using OLED variable)
    extern bool fireDetected;
    if (fireDetected) {
      // SYNC BOTH DISPLAYS - Safe Status
      displaySafeStatus();      // LCD: Smart Shop Guard + temp/humidity
      displayOLEDSafeStatus();  // OLED: Resumes normal operation
      
      Serial.println("=== FIRE HAZARD CLEARED ===");
      Serial.println("LCD: Safe status displayed");
      Serial.println("OLED: Safe status - resuming normal operation");
    }
  }
  
  // Handle ultrasonic and servo for automatic door
  triggerUltrasonicSensor();
  moveServo();
}

void processDayMode() {
  // Read sensors and update display
  fanTempLCD();
  
  // Check flame sensor
  readFlameSensor();
  if (isFlameDetected()) {
    playAlertTone();
    activateRelay();
    
    // SYNC BOTH DISPLAYS - Fire Alert
    displayFireAlert();      // LCD: FIRE ALERT! + EVACUATE NOW!
    displayOLEDFireAlert();  // OLED: Switches to alerts page, shows fire alert
    
    Serial.println("=== FIRE HAZARD ACTIVE ===");
    Serial.println("LCD: Fire alert displayed");
    Serial.println("OLED: Fire alert displayed - page cycling paused");
  } else {
    deactivateRelay();
    
    // Check if fire was previously detected (using OLED variable)
    extern bool fireDetected;
    if (fireDetected) {
      // SYNC BOTH DISPLAYS - Safe Status
      displaySafeStatus();      // LCD: Smart Shop Guard + temp/humidity
      displayOLEDSafeStatus();  // OLED: Resumes normal operation
      
      Serial.println("=== FIRE HAZARD CLEARED ===");
      Serial.println("LCD: Safe status displayed");
      Serial.println("OLED: Safe status - resuming normal operation");
    }
  }
  
  // Check motion sensor for theft detection
  readMotion();
  if (isMotionDetected()) {
    // SYNC BOTH DISPLAYS - Thief Alert
    displayThiefAlert();      // LCD: THIEF ALERT! + Security Breach!
    displayOLEDThiefAlert();  // OLED: Switches to alerts page, shows thief alert
    
    Serial.println("=== MOTION HAZARD ACTIVE ===");
    Serial.println("LCD: Thief alert displayed");
    Serial.println("OLED: Thief alert displayed - page cycling paused");
    
    playAlertTone();
  } else {
    // Clear motion alert when no motion is detected (for both displays)
    extern bool motionDetected;
    if (motionDetected) {
      // SYNC BOTH DISPLAYS - Safe Status
      displayNormalStatus();  // LCD - return to normal display
      updateOLEDDisplay();  // OLED - update to show clear status
      
      Serial.println("=== MOTION HAZARD CLEARED ===");
      Serial.println("LCD: Normal status displayed");
      Serial.println("OLED: Normal status displayed");
    }
  }
}

void fanTempLCD() {
  // Read temperature and humidity using sensors module
  readTemperatureHumidity();
  
  // Control fan based on readings
  controlFan(t, h);
  
  // Only update LCD display if there are no active alerts
  // This prevents flickering between alert and normal states
  if (!isFlameDetected() && !isMotionDetected()) {
    displayNormalStatus();  // LCD - shows "Smart Shop Guard" + temp/humidity
  } else {
    // Debug output to track alert states
    if (isFlameDetected()) {
      Serial.println("Fire detected - LCD display locked to fire alert");
    }
    if (isMotionDetected()) {
      Serial.println("Motion detected - LCD display locked to thief alert");
    }
  }
  
  // Always update OLED (it handles its own alert states)
  displayOLEDTemperatureHumidity(t, h);  // OLED (immediate sync)
}
