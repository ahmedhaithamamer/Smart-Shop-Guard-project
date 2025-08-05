// Smart Shop Guard - Modular Version
// Main application file

#include "config.h"
#include "system.h"
#include "sensors.h"
#include "actuators.h"
#include "display.h"
#include "audio.h"
#include <BlynkSimpleEsp32.h>



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

// Blynk virtual pin handlers
BLYNK_WRITE(VPIN_DAY_NIGHT) {
  isDay = param.asInt();
  playModeSwitchTone();
  displayModeStatus();
}

BLYNK_WRITE(VPIN_AC_CONTROL) {
  AC = param.asInt();
}

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
    Blynk.config(BLYNK_AUTH_TOKEN);
    Blynk.connect();
  }
  
  // Play startup sequence
  playStartupTone();
  displayWelcomeMessage();
  displayModeStatus();
}

void loop() {
  Blynk.run();
  
  if (!isDay) {
    processNightMode();
  } else {
    processDayMode();
  }
}

void processNightMode() {
  // Read sensors and update display
  fanTempLCD();
  
  // Check flame sensor
  bool flameDetected = isFlameDetected();
  if (flameDetected) {
    playAlertTone();
    activateRelay();
    displayFireAlert();
  } else {
    deactivateRelay();
    displaySafeStatus();
  }
  
  // Send flame status to Blynk
  if (Blynk.connected()) {
    Blynk.virtualWrite(VPIN_FLAME, flameDetected);
  }
  
  // Handle ultrasonic and servo for automatic door
  triggerUltrasonicSensor();
  moveServo();
}

void processDayMode() {
  // Read sensors and update display
  fanTempLCD();
  
  // Check flame sensor
  bool flameDetected = isFlameDetected();
  if (flameDetected) {
    playAlertTone();
    activateRelay();
    displayFireAlert();
  } else {
    deactivateRelay();
    displaySafeStatus();
  }
  
  // Check motion sensor for theft detection
  bool motionDetected = isMotionDetected();
  if (motionDetected) {
    displayThiefAlert();
    playAlertTone();
  }
  
  // Send sensor data to Blynk
  if (Blynk.connected()) {
    Blynk.virtualWrite(VPIN_FLAME, flameDetected);
    Blynk.virtualWrite(VPIN_MOTION, motionDetected);
  }
  
  delay(200);
}

void fanTempLCD() {
  // Read temperature and humidity using sensors module
  readTemperatureHumidity();
  
  // Control fan based on readings
  controlFan(t, h);
  
  // Display readings on LCD
  displayTemperatureHumidity(t, h);
  
  // Send to Blynk if connected
  if (Blynk.connected()) {
    Blynk.virtualWrite(VPIN_TEMPERATURE, t);
    Blynk.virtualWrite(VPIN_HUMIDITY, h);
  }
}
