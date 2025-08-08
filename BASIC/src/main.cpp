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
const unsigned long oledUpdateInterval = 500; // Update every 500ms

// Function declarations for main logic
void processNightMode();
void processDayMode();
void fanTempLCD();

void setup() {
  // Initialize system components
  initSystem();
  
  // Initialize hardware modules
  initDisplay();      // Initialize LCD
  initOLEDDisplay();  // Initialize OLED
  initSensors();
  initActuators();
  initAudio();
  
  // Initialize WiFi and Blynk
  if (initWiFi()) {
    initBlynk();
    connectBlynk();
  }
  
  // Play startup sequence
  playStartupTone();
  
  // Show welcome on both displays
  displayWelcomeMessage();  // LCD welcome (blinking)
  // OLED welcome is handled in initOLEDDisplay() with intro animation
  
  displayModeStatus();  // Show on LCD
}

void loop() {
  // Reset watchdog timer
  esp_task_wdt_reset();
  
  Blynk.run();
  
  // Handle OLED button navigation and updates
  handleOLEDButtons();
  
  // Update OLED display at regular intervals
  if (millis() - lastOLEDUpdate > oledUpdateInterval) {
    updateOLEDDisplay();
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
    displayFireAlert();  // LCD alert
    // OLED alert is handled via fireDetected flag in OLED display
  } else {
    deactivateRelay();
    displaySafeStatus();  // LCD status
    // OLED status is handled via fireDetected flag in OLED display
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
    displayFireAlert();  // LCD alert
    // OLED alert is handled via fireDetected flag in OLED display
  } else {
    deactivateRelay();
    displaySafeStatus();  // LCD status
    // OLED status is handled via fireDetected flag in OLED display
  }
  
  // Check motion sensor for theft detection
  readMotion();
  if (isMotionDetected()) {
    displayThiefAlert();  // LCD alert
    // OLED alert is handled via motionDetected flag in OLED display
    playAlertTone();
  }
}

void fanTempLCD() {
  // Read temperature and humidity using sensors module
  readTemperatureHumidity();
  
  // Control fan based on readings
  controlFan(t, h);
  
  // Display readings on LCD (original functionality preserved)
  displayTemperatureHumidity(t, h);
  
  // OLED displays temperature/humidity in its sensor page automatically
  // via the global t and h variables
}
