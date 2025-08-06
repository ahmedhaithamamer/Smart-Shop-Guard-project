// Smart Shop Guard - Modular Version
// Main application file

#include "config.h"
#include "system.h"
#include "sensors.h"
#include "actuators.h"
#include "display.h"
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

// Function declarations for main logic
void processNightMode();
void processDayMode();
void fanTempLCD();

void setup() {
  // Initialize system components
  initSystem();
  
  // Initialize hardware modules
  initDisplay();
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
  displayWelcomeMessage();
  displayModeStatus();
}

void loop() {
  // Reset watchdog timer
  esp_task_wdt_reset();
  
  Blynk.run();
  
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
    displayFireAlert();
  } else {
    deactivateRelay();
    displaySafeStatus();
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
    displayFireAlert();
  } else {
    deactivateRelay();
    displaySafeStatus();
  }
  
  // Check motion sensor for theft detection
  readMotion();
  if (isMotionDetected()) {
    displayThiefAlert();
    playAlertTone();
  }
}

void fanTempLCD() {
  // Read temperature and humidity using sensors module
  readTemperatureHumidity();
  
  // Control fan based on readings
  controlFan(t, h);
  
  // Display readings on LCD
  displayTemperatureHumidity(t, h);
}
