// 🏪 Smart Shop Guard - ESP32 FreeRTOS Implementation
// 🚀 Dual-core architecture for optimal performance and reliability
// 🛡️ Advanced security monitoring with real-time sensor processing
// Arduino IDE Compatible Version - Single File Structure

// ========== BLYNK CONFIGURATION (MUST BE FIRST) ==========
#define BLYNK_TEMPLATE_ID "TMPL2jt8pOqfP"
#define BLYNK_TEMPLATE_NAME "Smart Secure Smart Shop"
#define BLYNK_AUTH_TOKEN "LVODBytZ9qETpIjcYFFx5-4b4UFfRUdg"

// ========== INCLUDES ==========
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP32Servo.h>         
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <SH1106Wire.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"

// ========== CONFIG.H EQUIVALENT ==========
// Blynk Configuration (already defined above)
// WiFi Configuration
#define WIFI_SSID "Amer_ABF"
#define WIFI_PASSWORD "Amer_Ham_2020"
#define WIFI_TIMEOUT 10000  // WiFi connection timeout in milliseconds

// Pin Definitions
#define DHTPIN 40           // DHT sensor pin
#define FAN_PIN 12          // Fan control pin
#define FLAME_SENSOR_PIN 4  // Flame sensor pin
#define BUZZER_PIN 17       // Buzzer pin
#define RELAY_PIN 1         // Relay pin
#define PIR_PIN 5           // PIR motion sensor pin
#define SERVO_PIN 7         // Servo motor pin
#define TRIG_PIN 42         // Ultrasonic sensor trigger pin
#define ECHO_PIN 41         // Ultrasonic sensor echo pin

// DHT Sensor Configuration
#define DHTTYPE DHT11       // DHT sensor type

// LCD Configuration
#define LCD_ADDRESS 0x27    // I2C address for LCD
#define LCD_COLUMNS 16      // LCD columns
#define LCD_ROWS 2          // LCD rows

// OLED Display Configuration (1.3" 128x64)
#define OLED_SDA_PIN 8     // SDA pin for OLED (ESP32)
#define OLED_SCL_PIN 9     // SCL pin for OLED (ESP32)
#define OLED_ADDRESS 0x3C   // I2C address for OLED

// Button Definitions
#define BUTTON_NEXT 47
#define BUTTON_PREV 48

// Temperature and Humidity Thresholds
#define TEMP_THRESHOLD 23   // Temperature threshold in Celsius
#define HUMIDITY_THRESHOLD 60  // Humidity threshold in percentage

// Ultrasonic Sensor Configuration
#define DISTANCE_THRESHOLD 12  // Distance threshold in cm for servo activation
#define SERVO_DELAY 3000      // Servo return delay in milliseconds

// Timing Configuration
#define SERIAL_BAUD_RATE 9600  // Serial communication baud rate
#define STARTUP_DISPLAY_DELAY 400  // Startup message display delay
#define MODE_DISPLAY_DELAY 1000    // Mode status display delay

// Blynk Virtual Pins
#define VPIN_TEMPERATURE V0
#define VPIN_HUMIDITY V1
#define VPIN_MOTION V3
#define VPIN_FLAME V4
#define VPIN_DAY_NIGHT V5
#define VPIN_AC_CONTROL V6

// ========== HARDWARE INTERFACE OBJECTS ==========
// 🔧 Hardware Interface Objects
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);  // 16x2 LCD Display
DHT dht(DHTPIN, DHTTYPE);                                    // Temperature & Humidity Sensor
Servo myServo;                                               // Security Door Control
SH1106Wire display(OLED_ADDRESS, OLED_SDA_PIN, OLED_SCL_PIN); // OLED Display object - 1.3" 128x64 display

// 🌐 Network Configuration
char ssid[] = WIFI_SSID;      // WiFi Network Name
char pass[] = WIFI_PASSWORD;  // WiFi Password

// ========== SYSTEM STATE VARIABLES ==========
// 📊 System State Variables
int h = 0;              // Current Humidity Percentage
int t = 0;              // Current Temperature (°C)
long duration = 0;      // Ultrasonic Sensor Timing
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
QueueHandle_t audioQueue = nullptr;       // Audio event queue for Core 0 audio processing
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

// PIR Motion Detection State Tracking
static bool lastPirState = false;
static int pirStabilityCounter = 0;

// LCD Display Slide Counter
static int lcdSlide = 0;

// OLED Display Variables
int currentPage = 0;
int currentSetting = 0;
const int totalPages = 5;
const int totalSettings = 2;
bool alertFlashing = false;
bool fireDetected = false;
bool motionDetected = false;
bool doorOpen = false;
unsigned long lastAnimation = 0;
unsigned long lastAutoSwipe = 0;
unsigned long lastDebounce = 0;
unsigned long buttonHoldStart = 0;
const unsigned long debounceDelay = 300;
const unsigned long longPressDelay = 1000;
const unsigned long autoSwipeDelay = 15000;

// Audio/Buzzer
static bool buzzerInitialized = false;

// FreeRTOS Resources
QueueHandle_t sensorDataQueue = nullptr;
QueueHandle_t eventQueue = nullptr;
QueueHandle_t audioQueue = nullptr;
SemaphoreHandle_t i2cMutex = nullptr;
SemaphoreHandle_t dataMutex = nullptr;

// Task Handles
static TaskHandle_t hTaskSensors = nullptr;
static TaskHandle_t hTaskActuators = nullptr;
static TaskHandle_t hTaskLCD = nullptr;
static TaskHandle_t hTaskOLED = nullptr;
static TaskHandle_t hTaskWiFi = nullptr;
static TaskHandle_t hTaskSysMon = nullptr;

// ========== DATA STRUCTURES ==========
// Shared sensor readings payload
typedef struct {
  int temperatureC;
  int humidityPct;
  bool flame;
  int distanceCm;
  bool pirMotion;
  uint32_t tsMs;
} SensorData;

// Event types for inter-task notifications
enum EventType {
  EVENT_NONE = 0,
  EVENT_MOTION_DETECTED,
  EVENT_MOTION_CLEARED,
  EVENT_FIRE_DETECTED,
  EVENT_FIRE_CLEARED
};

struct Event {
  EventType type;
  uint32_t tsMs;
};

// Audio event types for Core 0 audio processing
enum AudioEventType {
  AUDIO_STARTUP = 0,
  AUDIO_MODE_SWITCH,
  AUDIO_FIRE_ALERT,
  AUDIO_MOTION_ALERT
};

struct AudioEvent {
  AudioEventType type;
  uint32_t timestamp;
};

// OLED Page types
enum PageType {
  PAGE_STATUS = 0,
  PAGE_SENSORS = 1,
  PAGE_SYSTEM = 2,
  PAGE_ALERTS = 3,
  PAGE_SETTINGS = 4
};

enum MenuState {
  STATE_PAGES = 0,
  STATE_SETTINGS = 1
};

struct OLEDConfig {
  bool auto_swipe;
};

MenuState currentState = STATE_PAGES;
OLEDConfig oledConfig = {true};

// ========== ICON DEFINITIONS ==========
// Temperature Icon
const unsigned char temp_icon[] PROGMEM = {
  0x00, 0x00, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01,
  0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0xc0, 0x03, 0xe0, 0x07, 0xf0, 0x0f,
  0xf0, 0x0f, 0xe0, 0x07, 0xc0, 0x03, 0x00, 0x00
};

// System Icon
const unsigned char system_icon[] PROGMEM = {
  0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 
  0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xC0, 0x03, 0xC0, 0x03, 0xF8, 0x1F, 0xFC, 0x3F
};

// Shield Icon
const unsigned char shield_icon[] PROGMEM = {
  0x80, 0x01, 0xF0, 0x0F, 0x7C, 0x3F, 0x1F, 0xFF, 0x07, 0xFF, 0x03, 0xFF, 
  0x03, 0xFF, 0x07, 0x7F, 0x06, 0x7F, 0x06, 0x7F, 0x0C, 0x3F, 0x1C, 0x3F, 
  0x38, 0x1F, 0x70, 0x0F, 0xE0, 0x07, 0xC0, 0x03
};

// Alert Icon
const unsigned char alert_icon[] PROGMEM = {
  0xC0, 0x00, 0xE0, 0x01, 0xE0, 0x01, 0xF0, 0x01, 0xF0, 0x03, 0xF0, 0x03, 
  0x38, 0x07, 0x38, 0x07, 0x3C, 0x0F, 0x3C, 0x0F, 0xFE, 0x0F, 0xFE, 0x1F, 
  0x3E, 0x1F, 0x3F, 0x3F, 0xFF, 0x3F, 0xFF, 0x3F
};

// Settings Icon
const unsigned char settings_icon[] PROGMEM = {
  0x80, 0x01, 0xC0, 0x03, 0xDC, 0x3B, 0xFC, 0x3F, 0xFC, 0x3F, 0x3C, 0x1C, 
  0x1E, 0x78, 0x1F, 0xF8, 0x1F, 0xF8, 0x1E, 0x78, 0x3C, 0x1C, 0xFC, 0x3F, 
  0xFC, 0x3F, 0xFC, 0x3F, 0xC0, 0x03, 0x80, 0x01
};

// WiFi Icons
const unsigned char wifi_icon_connected[] PROGMEM = {
  0x07, 0x1F, 0x3C, 0x73, 0x6F, 0xEC, 0xD9, 0xDB
};

const unsigned char wifi_icon_disconnected[] PROGMEM = {
  0xBC, 0xC2, 0xE1, 0x01, 0x01, 0x01, 0x02, 0x3C
};

const unsigned char Bluetooth_icon[] PROGMEM = {
  0x0C, 0x1C, 0x2E, 0x1C, 0x1C, 0x2E, 0x1C, 0x0C
};

const unsigned char Bluetooth_icon_disconnected[] PROGMEM = {
  0x42, 0xE7, 0x7E, 0x3C, 0x3C, 0x7E, 0xE7, 0x42
};

// ========== SENSOR FUNCTIONS ==========
void initSensors() {
    dht.begin();
    pinMode(FLAME_SENSOR_PIN, INPUT);
    pinMode(PIR_PIN, INPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    lastPirState = digitalRead(PIR_PIN);
}

void readTemperatureHumidity() {
    h = dht.readHumidity();
    t = dht.readTemperature();
}

int getTemperature() {
    return t;
}

int getHumidity() {
    return h;
}

void readFlameSensor() {
    // Function placeholder
}

bool isFlameDetected() {
    return (digitalRead(FLAME_SENSOR_PIN) == 0);
}

void readMotion() {
    bool currentPirState = digitalRead(PIR_PIN);
    
    if (currentPirState != lastPirState) {
        pirStabilityCounter++;
        if (pirStabilityCounter >= 2) {
            if (eventQueue) {
                if (currentPirState && !isDay) {  // Fixed: Night mode detection
                    Event e{EVENT_MOTION_DETECTED, (uint32_t)millis()};
                    xQueueSend(eventQueue, &e, 0);
                    Serial.println("🚨 Motion detected at NIGHT - Thief alert triggered");
                } else if (currentPirState && isDay) {
                    Serial.println("👥 Motion detected during DAY - No thief alert (normal operation)");
                } else if (!currentPirState) {
                    Event e{EVENT_MOTION_CLEARED, (uint32_t)millis()};
                    xQueueSend(eventQueue, &e, 0);
                    Serial.println("✅ Motion cleared");
                }
            }
            lastPirState = currentPirState;
            pirStabilityCounter = 0;
        }
    } else {
        pirStabilityCounter = 0;
    }
}

bool isMotionDetected() {
    return (digitalRead(PIR_PIN) == HIGH);
}

void triggerUltrasonicSensor() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH, 30000);
    if (duration == 0) {
        duration = 0;
    }
}

int getDistance() {
    if (duration == 0) {
        return 999;
    } else {
        return duration * 0.034 / 2;
    }
}

// ========== ACTUATOR FUNCTIONS ==========
void initActuators() {
    pinMode(FAN_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    myServo.attach(SERVO_PIN);
    turnOffFan();
    deactivateRelay();
}

void controlFan(int temperature, int humidity) {
    if (temperature > TEMP_THRESHOLD || humidity > HUMIDITY_THRESHOLD || AC) {
        turnOnFan();
    } else {
        turnOffFan();
    }
}

void turnOnFan() {
    digitalWrite(FAN_PIN, HIGH);
}

void turnOffFan() {
    digitalWrite(FAN_PIN, LOW);
}

void moveServo() {
    SensorData latest{};
    if (xQueuePeek(sensorDataQueue, &latest, 0) == pdTRUE) {
        if (latest.distanceCm <= DISTANCE_THRESHOLD) {
            degree = 180;
            lastTime = millis();
        }

        if (millis() - lastTime >= SERVO_DELAY) {
            degree = 0;
        }

        myServo.write(degree);
    }
}

void setServoPosition(int position) {
    degree = position;
    myServo.write(degree);
}

void activateRelay() {
    digitalWrite(RELAY_PIN, HIGH);
}

void deactivateRelay() {
    digitalWrite(RELAY_PIN, LOW);
}

// ========== AUDIO FUNCTIONS ==========
static void buzzerAttach() {
    if (buzzerInitialized) return;
    
    // Configure LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .freq_hz          = 2000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // Configure LEDC channel
    ledc_channel_config_t ledc_channel = {
        .channel    = LEDC_CHANNEL_0,
        .duty       = 0,
        .gpio_num   = BUZZER_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_0
    };
    ledc_channel_config(&ledc_channel);
    
    buzzerInitialized = true;
}

static void writeTone(int frequency) {
    buzzerAttach();
    if (frequency <= 0) {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    } else {
        ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, frequency);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512); // 50% duty cycle
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }
}

void initAudio() {
    Serial.printf("Initializing audio on pin %d\n", BUZZER_PIN);
    pinMode(BUZZER_PIN, OUTPUT);
    buzzerAttach();
    writeTone(0);
    Serial.println("Audio initialization complete");
}

void playStartupTone() {
    Serial.println("Playing startup tone...");
    int startupNotes[] = {523, 659, 784, 1047, 1319, 1568};
    int duration = 150;
    for (unsigned i = 0; i < sizeof(startupNotes) / sizeof(startupNotes[0]); i++) {
        Serial.printf("Playing note %d Hz\n", startupNotes[i]);
        writeTone(startupNotes[i]);
        vTaskDelay(pdMS_TO_TICKS(duration + 20));
    }
    writeTone(0);
    Serial.println("Startup tone complete");
}

void playModeSwitchTone() {
    int switchNotes[] = {784, 988, 1175};
    int duration = 120;
    for (unsigned i = 0; i < sizeof(switchNotes) / sizeof(switchNotes[0]); i++) {
        writeTone(switchNotes[i]);
        vTaskDelay(pdMS_TO_TICKS(duration + 20));
    }
    writeTone(0);
}

void playAlertTone() {
    Serial.println("Playing alert tone...");
    int duration = 150;
    int tone1 = 880;
    int tone2 = 440;
    for (int i = 0; i < 3; i++) {
        Serial.printf("Alert tone cycle %d: %d Hz\n", i+1, tone1);
        writeTone(tone1);
        vTaskDelay(pdMS_TO_TICKS(duration));
        writeTone(tone2);
        vTaskDelay(pdMS_TO_TICKS(duration));
    }
    writeTone(tone1);
    vTaskDelay(pdMS_TO_TICKS(250));
    writeTone(0);
    Serial.println("Alert tone complete");
}

void playTone(int frequency, int duration) {
    writeTone(frequency);
    if (duration > 0) {
        vTaskDelay(pdMS_TO_TICKS(duration));
        writeTone(0);
    }
}

void stopTone() {
    writeTone(0);
}

// ========== LCD DISPLAY FUNCTIONS ==========
void initDisplay() {
    lcd.init();
    lcd.backlight();
}

void clearDisplay() {
    lcd.clear();
}

void setDisplayCursor(int col, int row) {
    lcd.setCursor(col, row);
}

void printToDisplay(const char* text) {
    lcd.print(text);
}

void printToDisplay(int value) {
    lcd.print(value);
}

void displayTemperatureHumidity(int temperature, int humidity) {
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(temperature);
    lcd.print("C ");

    lcd.setCursor(7, 1);
    lcd.print("H:");
    lcd.print(humidity);
    lcd.print("%");
}

void displayWelcomeMessage() {
    for (int i = 0; i < 3; i++) {
        lcd.setCursor(4, 0);
        lcd.print("Welcome!");
        vTaskDelay(pdMS_TO_TICKS(STARTUP_DISPLAY_DELAY));
        yield();
        lcd.clear();
        vTaskDelay(pdMS_TO_TICKS(200));
        yield();
    }
}

void displayModeStatus() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    lcd.print(isDay ? "Day" : "Night");  // Fixed mode display logic
    vTaskDelay(pdMS_TO_TICKS(MODE_DISPLAY_DELAY));
    yield();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Smart Shop Guard");
    vTaskDelay(pdMS_TO_TICKS(MODE_DISPLAY_DELAY));
    yield();
}

void displayFireAlert() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FIRE ALERT!");
    lcd.setCursor(0, 1);
    lcd.print("EVACUATE NOW!");
    Serial.println("LCD: Fire alert displayed");
}

void displayNormalStatus() {
    static bool created = false;
    
    // Create custom glyphs only once
    if (!created) {
        uint8_t arrowRight[8] = {0x00,0x04,0x06,0x1F,0x06,0x04,0x00,0x00};
        uint8_t arrowLeft[8]  = {0x00,0x04,0x0C,0x1F,0x0C,0x04,0x00,0x00};
        uint8_t spinner0[8]   = {0x00,0x04,0x04,0x1F,0x04,0x04,0x00,0x00};
        uint8_t spinner1[8]   = {0x00,0x00,0x04,0x1F,0x04,0x00,0x00,0x00};
        uint8_t spinner2[8]   = {0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00};
        uint8_t spinner3[8]   = {0x00,0x00,0x10,0x1F,0x10,0x00,0x00,0x00};
        lcd.createChar(0, arrowRight);
        lcd.createChar(1, arrowLeft);
        lcd.createChar(2, spinner0);
        lcd.createChar(3, spinner1);
        lcd.createChar(4, spinner2);
        lcd.createChar(5, spinner3);
        created = true;
    }

    lcd.clear();
    
    // Show one slide per call to avoid blocking
    switch (lcdSlide) {
        case 0:
            lcd.setCursor(0, 0);
            lcd.print("Smart Shop Guard");
            lcd.setCursor(0, 1);
            lcd.print("Dual Display IoT");
            break;
        case 1:
            lcd.setCursor(0, 0);
            lcd.print("Secure Smart");
            lcd.setCursor(0, 1);
            lcd.print("Multi-Sensor");
            break;
        case 2:
            lcd.setCursor(0, 0);
            lcd.print("Monitors:");
            lcd.setCursor(0, 1);
            lcd.print("Temp PIR Flame");
            break;
        case 3:
            lcd.setCursor(0, 0);
            lcd.print("Controls:");
            lcd.setCursor(0, 1);
            lcd.print("Servo Fan Relay");
            break;
        case 4:
            lcd.setCursor(0, 0);
            lcd.print("Connectivity:");
            lcd.setCursor(0, 1);
            lcd.print("WiFi + Blynk IoT");
            break;
        case 5:
            lcd.setCursor(0, 0);
            lcd.print("Door System:");
            lcd.setCursor(0, 1);
            lcd.print("Auto Open/Close");
            break;
        case 6:
            lcd.setCursor(0, 0);
            lcd.print("Status: ");
            lcd.setCursor(8, 0);
            lcd.write((uint8_t)2); // spinner
            lcd.setCursor(0, 1);
            lcd.print("All Systems OK");
            break;
    }
    
    lcdSlide = (lcdSlide + 1) % 7;
}

void displaySafeStatus() {
    displayNormalStatus();
}

void displayThiefAlert() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("THIEF ALERT!");
    lcd.setCursor(0, 1);
    lcd.print("Security Breach!");
    Serial.println("LCD: Thief alert displayed");
}

// ========== OLED DISPLAY FUNCTIONS ==========
void initOLEDDisplay() {
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
    
    // Initialize button pins
    pinMode(BUTTON_NEXT, INPUT_PULLUP);
    pinMode(BUTTON_PREV, INPUT_PULLUP);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    Serial.println("OLED buttons initialized - NEXT: " + String(BUTTON_NEXT) + ", PREV: " + String(BUTTON_PREV));
    
    // Show intro animation
    showIntro();
}

void showIntro() {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    
    String line1 = "Smart Shop";
    for (int i = 0; i <= line1.length(); i++) {
        display.clear();
        display.drawString(15, 15, line1.substring(0, i));
        display.display();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    vTaskDelay(pdMS_TO_TICKS(200));
    
    String line2 = "Guard";
    for (int i = 0; i <= line2.length(); i++) {
        display.clear();
        display.drawString(15, 15, line1);
        display.drawString(35, 30, line2.substring(0, i));
        
        if (i == line2.length()) {
            display.drawXbm(80, 32, 16, 16, shield_icon);
        }
        
        display.display();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    vTaskDelay(pdMS_TO_TICKS(300));
    
    for (int i = 0; i < 2; i++) {
        display.clear();
        display.display();
        vTaskDelay(pdMS_TO_TICKS(150));
        
        display.clear();
        display.drawString(15, 15, "Smart Shop");
        display.drawString(35, 30, "Guard");
        display.drawXbm(80, 32, 16, 16, shield_icon);
        display.display();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    
    display.clear();
    display.drawString(15, 15, "Smart Shop");
    display.drawString(35, 30, "Guard");
    display.drawXbm(80, 32, 16, 16, shield_icon);
    display.display();
    vTaskDelay(pdMS_TO_TICKS(400));
}

void clearOLEDDisplay() {
    display.clear();
}

void displayOLEDWelcomeMessage() {
    showIntro();
}

void displayOLEDModeStatus() {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(15, 15, "Mode: ");
    display.drawString(15, 35, isDay ? "Day" : "Night");  // Fixed mode display logic
    display.display();
    vTaskDelay(pdMS_TO_TICKS(1000));
    updateOLEDDisplay();
}

void displayOLEDFireAlert() {
    fireDetected = true;
    alertFlashing = true;
    currentPage = PAGE_ALERTS;
    lastAutoSwipe = millis();
    updateOLEDDisplay();
    Serial.println("OLED: Fire alert displayed - page cycling paused");
}

void displayOLEDSafeStatus() {
    fireDetected = false;
    alertFlashing = false;
    currentPage = PAGE_STATUS;
    lastAutoSwipe = millis();
    updateOLEDDisplay();
    Serial.println("OLED: Safe status - resuming normal operation and page cycling");
}

void displayOLEDThiefAlert() {
    motionDetected = true;
    alertFlashing = true;
    currentPage = PAGE_ALERTS;
    lastAutoSwipe = millis();
    updateOLEDDisplay();
    Serial.println("OLED: Thief alert displayed - page cycling paused");
}

void displayOLEDMotionCleared() {
    motionDetected = false;
    alertFlashing = false;
    currentPage = PAGE_STATUS;
    lastAutoSwipe = millis();
    updateOLEDDisplay();
    Serial.println("OLED: Motion cleared - resuming normal operation and page cycling");
}

void updateOLEDDisplay() {
    display.clear();
    showCurrentPage();
    display.display();
}

bool isBluetoothConnected() {
    return true; // Placeholder
}

bool isWiFiConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

void showCurrentPage() {
    switch (currentPage) {
        case PAGE_STATUS:
            showStatusPage();
            break;
        case PAGE_SENSORS:
            showSensorPage();
            break;
        case PAGE_SYSTEM:
            showSystemPage();
            break;
        case PAGE_ALERTS:
            showAlertsPage();
            break;
        case PAGE_SETTINGS:
            showSettingsPage();
            break;
    }
}

void showStatusPage() {
    display.drawXbm(0, 5, 16, 16, shield_icon);
    if(isWiFiConnected()){
        display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
    } else {
        display.drawXbm(120, 5, 8, 8, wifi_icon_disconnected);
    }
    if(isBluetoothConnected()){
        display.drawXbm(110, 5, 8, 8, Bluetooth_icon);
    } else {
        display.drawXbm(110, 5, 8, 8, Bluetooth_icon_disconnected);
    }
    
    display.setFont(ArialMT_Plain_10);
    display.drawString(18, 5, "Status");
    
    if (fireDetected && alertFlashing) {
        display.drawString(0, 20, "FIRE DETECTED!");
        display.drawString(0, 30, "EMERGENCY!");
        display.drawString(0, 40, "EVACUATE NOW!");
    } else if (motionDetected && alertFlashing) {
        display.drawString(0, 20, "MOTION DETECTED!");
        display.drawString(0, 30, "Location: Door");
        display.drawString(0, 40, "Alert: ACTIVE");
    } else {
        display.drawString(0, 20, "System: SECURE");
        display.drawString(0, 30, "Temp: " + String(t) + "C");
        display.drawString(0, 40, "Humid: " + String(h) + "%");
        
        display.setFont(ArialMT_Plain_10);
        if (isWiFiConnected()) {
            display.drawString(90, 20, "WiFi:OK");
        } else {
            display.drawString(90, 20, "WiFi:--");
        }
    }
}

void showSensorPage() {
    display.drawXbm(0, 5, 16, 16, temp_icon);
    if(isWiFiConnected()){
        display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
    } else {
        display.drawXbm(120, 5, 8, 8, wifi_icon_disconnected);
    }
    if(isBluetoothConnected()){
        display.drawXbm(110, 5, 8, 8, Bluetooth_icon);
    } else {
        display.drawXbm(110, 5, 8, 8, Bluetooth_icon_disconnected);
    }
    display.setFont(ArialMT_Plain_10);
    display.drawString(18, 5, "Sensors");
    
    display.drawString(0, 20, "Temperature: " + String(t) + "C");
    display.drawString(0, 30, "Humidity: " + String(h) + "%");
    display.drawString(0, 40, "AC: " + String(AC ? "ON" : "OFF"));
    display.drawString(0, 48, fireDetected ? "Fire: YES" : "Fire: NO");
}

void showSystemPage() {
    display.drawXbm(0, 5, 16, 16, system_icon);
    if(isWiFiConnected()){
        display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
    } else {
        display.drawXbm(120, 5, 8, 8, wifi_icon_disconnected);
    }
    if(isBluetoothConnected()){
        display.drawXbm(110, 5, 8, 8, Bluetooth_icon);
    } else {
        display.drawXbm(110, 5, 8, 8, Bluetooth_icon_disconnected);
    }
    display.setFont(ArialMT_Plain_10);
    display.drawString(18, 5, "System");
    
    if (isWiFiConnected()) {
        display.drawString(0, 20, "WiFi: Connected");
    } else {
        display.drawString(0, 20, "WiFi: Disconnected");
    }
    display.drawString(0, 30, "Mode: " + String(isDay ? "Day" : "Night"));
    display.drawString(0, 40, "Uptime: " + String(millis() / 1000) + "s");
    display.drawString(0, 50, "Memory: " + String(ESP.getFreeHeap()/1000) + "KB");
}

void showAlertsPage() {
    display.drawXbm(0, 5, 16, 16, alert_icon);
    if(isWiFiConnected()){
        display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
    } else {
        display.drawXbm(120, 5, 8, 8, wifi_icon_disconnected);
    }
    if(isBluetoothConnected()){
        display.drawXbm(110, 5, 8, 8, Bluetooth_icon);
    } else {
        display.drawXbm(110, 5, 8, 8, Bluetooth_icon_disconnected);
    }
    display.setFont(ArialMT_Plain_10);
    display.drawString(18, 5, "Alerts");
    
    if (fireDetected) {
        display.drawString(0, 20, "FIRE ALERT!");
        display.drawString(0, 30, "EVACUATE NOW!");
        display.drawString(0, 40, "System: EMERGENCY");
        display.drawString(0, 50, "Status: ACTIVE");
    } else if (motionDetected) {
        display.drawString(0, 20, "THIEF ALERT!");
        display.drawString(0, 30, "Security Breach!");
        display.drawString(0, 40, "Location: Door");
        display.drawString(0, 50, "Status: ACTIVE");
    } else {
        display.drawString(0, 20, "All Clear");
        display.drawString(0, 30, "No active alerts");
        display.drawString(0, 40, "System secure");
        display.drawString(0, 50, "Status: NORMAL");
    }
}

void showSettingsPage() {
    display.drawXbm(0, 5, 16, 16, settings_icon);
    if(isWiFiConnected()){
        display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
    } else {
        display.drawXbm(120, 5, 8, 8, wifi_icon_disconnected);
    }
    if(isBluetoothConnected()){
        display.drawXbm(110, 5, 8, 8, Bluetooth_icon);
    } else {
        display.drawXbm(110, 5, 8, 8, Bluetooth_icon_disconnected);
    }
    display.setFont(ArialMT_Plain_10);
    display.drawString(18, 5, "Settings");
    
    String autoSwipeText = "Auto-Swipe: " + String(oledConfig.auto_swipe ? "ON" : "OFF");
    String pagesText = "Pages: " + String(totalPages);
    
    if (currentState == STATE_SETTINGS) {
        if (currentSetting == 0) {
            display.fillRect(0, 18, 128, 12);
            display.setFont(ArialMT_Plain_10);
            display.drawString(0, 20, "> " + autoSwipeText);
            display.drawString(0, 30, pagesText);
        } else if (currentSetting == 1) {
            display.drawString(0, 20, autoSwipeText);
            display.fillRect(0, 28, 128, 12);
            display.setFont(ArialMT_Plain_10);
            display.drawString(0, 30, "> " + pagesText);
        }
        display.setFont(ArialMT_Plain_10);
        display.drawString(0, 50, "NEXT: Select  PREV: Execute");
    } else {
        display.drawString(0, 20, autoSwipeText);
        display.drawString(0, 30, pagesText);
        display.setFont(ArialMT_Plain_10);
        display.drawString(0, 50, "Hold both buttons to toggle auto-swipe");
    }
}

void handleOLEDButtons() {
    static bool nextPressed = false;
    static bool prevPressed = false;
    static unsigned long bothButtonsStart = 0;
    static unsigned long lastButtonPress = 0;
    
    bool nextState = (digitalRead(BUTTON_NEXT) == LOW);
    bool prevState = (digitalRead(BUTTON_PREV) == LOW);
    
    if (millis() - lastButtonPress < 50) {
        return;
    }
    
    // Check if both buttons are pressed (toggle auto-swipe)
    if (nextState && prevState) {
        if (bothButtonsStart == 0) {
            bothButtonsStart = millis();
        } else if (millis() - bothButtonsStart > 1000) {
            oledConfig.auto_swipe = !oledConfig.auto_swipe;
            lastDebounce = millis();
            lastAutoSwipe = millis();
            bothButtonsStart = 0;
            lastButtonPress = millis();
            Serial.println("Auto-swipe toggled: " + String(oledConfig.auto_swipe ? "ON" : "OFF"));
            
            display.invertDisplay();
            vTaskDelay(pdMS_TO_TICKS(100));
            display.normalDisplay();
            vTaskDelay(pdMS_TO_TICKS(100));
            display.invertDisplay();
            vTaskDelay(pdMS_TO_TICKS(100));
            display.normalDisplay();
        }
        return;
    } else {
        bothButtonsStart = 0;
    }
    
    if (millis() - lastDebounce > debounceDelay) {
        if (currentState == STATE_PAGES) {
            if (nextState && !nextPressed) {
                currentPage = (currentPage + 1) % totalPages;
                lastDebounce = millis();
                lastAutoSwipe = millis();
                lastButtonPress = millis();
                Serial.println("Page changed to: " + String(currentPage));
            }
            
            if (prevState && !prevPressed) {
                currentPage = (currentPage - 1 + totalPages) % totalPages;
                lastDebounce = millis();
                lastAutoSwipe = millis();
                lastButtonPress = millis();
                Serial.println("Page changed to: " + String(currentPage));
            }
        } else {
            if (nextState && !nextPressed) {
                currentSetting = (currentSetting + 1) % totalSettings;
                lastDebounce = millis();
                lastButtonPress = millis();
                Serial.println("Setting selected: " + String(currentSetting));
            }
            
            if (prevState && !prevPressed) {
                executeSetting();
                lastDebounce = millis();
                lastButtonPress = millis();
            }
        }
    }
    
    nextPressed = nextState;
    prevPressed = prevState;
    
    // Auto-swipe functionality - PAUSED during hazards
    if (oledConfig.auto_swipe && (millis() - lastAutoSwipe > autoSwipeDelay)) {
        if (!fireDetected && !motionDetected) {
            currentPage = (currentPage + 1) % totalPages;
            lastAutoSwipe = millis();
        } else {
            lastAutoSwipe = millis();
        }
    }
}

void executeSetting() {
    switch (currentSetting) {
        case 0:
            oledConfig.auto_swipe = !oledConfig.auto_swipe;
            Serial.println("Auto-swipe: " + String(oledConfig.auto_swipe ? "ON" : "OFF"));
            break;
        case 1:
            Serial.println("Total pages: " + String(totalPages));
            break;
    }
    updateOLEDDisplay();
}

// ========== SYSTEM FUNCTIONS ==========
void initSystem() {
    Serial.begin(SERIAL_BAUD_RATE);
    startTime = millis();
}

bool initWiFi() {
    if (strlen(ssid) == 0 || strlen(pass) == 0) {
        Serial.println("❌ Error: WiFi credentials not configured");
        return false;
    }
    
    Serial.print("🌐 Attempting to connect to: ");
    Serial.println(ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    vTaskDelay(pdMS_TO_TICKS(100));
    WiFi.begin(ssid, pass);
    
    unsigned long wifiConnectStart = millis();
    Serial.print("🔄 Connecting to WiFi");
    
    while (WiFi.status() != WL_CONNECTED && millis() - wifiConnectStart < 5000) {
        vTaskDelay(pdMS_TO_TICKS(500));
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("✅ WiFi connected successfully!");
        Serial.print("🌐 IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        Serial.println();
        Serial.println("⚠️ WiFi connection failed - starting in offline mode");
        WiFi.disconnect();
        return false;
    }
}

void handleWiFiReconnection() {
    static unsigned long lastWiFiReconnectAttempt = 0;
    const unsigned long wifiReconnectInterval = 30000;
    static bool wifiReconnecting = false;
    
    if (!isWiFiConnected() && 
        (millis() - lastWiFiReconnectAttempt > wifiReconnectInterval) &&
        !wifiReconnecting) {
        
        wifiReconnecting = true;
        lastWiFiReconnectAttempt = millis();
        
        Serial.println("🔄 Background: Attempting WiFi reconnection...");
        WiFi.disconnect();
        vTaskDelay(pdMS_TO_TICKS(50));
        WiFi.begin(ssid, pass);
        wifiReconnecting = false;
        
        Serial.println("🔄 Background: WiFi reconnection initiated (non-blocking)");
    }
}

// ========== BLYNK FUNCTIONS ==========
void initBlynk() {
    Blynk.config(BLYNK_AUTH_TOKEN);
}

bool connectBlynk() {
    return Blynk.connect();
}

bool isBlynkConnected() {
    return Blynk.connected();
}

BLYNK_CONNECTED() {
    Blynk.syncVirtual(VPIN_DAY_NIGHT, VPIN_AC_CONTROL);
    Blynk.virtualWrite(VPIN_TEMPERATURE, t);
    Blynk.virtualWrite(VPIN_HUMIDITY, h);
}

BLYNK_WRITE(VPIN_DAY_NIGHT) {
    isDay = param.asInt();
    
    if (audioQueue) {
        AudioEvent modeAudio{AUDIO_MODE_SWITCH, (uint32_t)millis()};
        xQueueSend(audioQueue, &modeAudio, 0);
    }
    
    if (i2cMutex && xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        displayModeStatus();
        displayOLEDModeStatus();
        xSemaphoreGive(i2cMutex);
    } else {
        displayModeStatus();
        displayOLEDModeStatus();
    }
}

BLYNK_WRITE(VPIN_AC_CONTROL) {
    AC = param.asInt();
}

void sendSensorDataToBlynk(int temperature, int humidity, bool flame, bool motion) {
    if (Blynk.connected()) {
        Blynk.virtualWrite(VPIN_TEMPERATURE, temperature);
        Blynk.virtualWrite(VPIN_HUMIDITY, humidity);
        Blynk.virtualWrite(VPIN_FLAME, flame);
        Blynk.virtualWrite(VPIN_MOTION, motion);
    }
}

// ========== FREERTOS TASKS ==========
void TaskWiFiBlynk(void* pvParameters) {
    Serial.printf("[CORE %d] TaskWiFiBlynk started\n", xPortGetCoreID());
    SensorData latest{};
    unsigned long lastBlynkSend = 0;
    const unsigned long blynkSendInterval = 2000;
    
    for(;;) {
        handleWiFiReconnection();
        
        if (isWiFiConnected()) {
            Blynk.run();
            
            if (millis() - lastBlynkSend >= blynkSendInterval) {
                if (xQueuePeek(sensorDataQueue, &latest, 0) == pdTRUE) {
                    sendSensorDataToBlynk(latest.temperatureC, latest.humidityPct, latest.flame, latest.pirMotion);
                    lastBlynkSend = millis();
                }
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void TaskLCD(void* pvParameters) {
    Serial.printf("[CORE %d] TaskLCD started\n", xPortGetCoreID());
    static unsigned long lastNormalUpdate = 0;
    
    for(;;) {
        if (!fireAlertActive && !motionAlertActive && (millis() - lastNormalUpdate > 5000)) {
            if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
                displayNormalStatus();
                xSemaphoreGive(i2cMutex);
                lastNormalUpdate = millis();
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void TaskOLED(void* pvParameters) {
    Serial.printf("[CORE %d] TaskOLED started\n", xPortGetCoreID());
    for(;;) {
        handleOLEDButtons();
        if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            updateOLEDDisplay();
            xSemaphoreGive(i2cMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void TaskSensorPoll(void* pvParameters) {
    Serial.printf("[CORE %d] TaskSensorPoll started\n", xPortGetCoreID());
    SensorData msg{};
    static bool fireStable = false;
    static int fireChangeCounter = 0;
    
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
        readMotion();
        
        triggerUltrasonicSensor();
        msg.distanceCm = getDistance();
        
        // Log distance for debugging
        Serial.print("📏 Distance: ");
        Serial.print(msg.distanceCm);
        Serial.println(" cm");
        
        msg.pirMotion = isMotionDetected();
        msg.tsMs = millis();
        xQueueOverwrite(sensorDataQueue, &msg);

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void TaskActuators(void* pvParameters) {
    Serial.printf("[CORE %d] TaskActuators started\n", xPortGetCoreID());
    SensorData latest{};
    Event ev{};
    
    for(;;) {
        if (xQueueReceive(eventQueue, &ev, pdMS_TO_TICKS(10)) == pdTRUE) {
            switch (ev.type) {
                case EVENT_FIRE_DETECTED:
                    fireAlertActive = true;
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
        
        if (xQueuePeek(sensorDataQueue, &latest, 0) == pdTRUE) {
            controlFan(latest.temperatureC, latest.humidityPct);
            moveServo();
        }
        
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void TaskSystemMonitor(void* pvParameters) {
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
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// ========== ARDUINO SETUP & LOOP ==========
void setup() {
    // Power stabilization delay
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    // System initialization
    initSystem();
    
    // FreeRTOS resource creation
    sensorDataQueue = xQueueCreate(1, sizeof(SensorData));
    eventQueue = xQueueCreate(10, sizeof(Event));
    audioQueue = xQueueCreate(5, sizeof(AudioEvent));
    i2cMutex = xSemaphoreCreateMutex();
    dataMutex = xSemaphoreCreateMutex();
    
    // Display system initialization
    Serial.println("🖥️ Initializing display systems...");
    initDisplay();
    initOLEDDisplay();
    
    // Welcome messages
    Serial.println("📝 Displaying welcome messages...");
    displayWelcomeMessage();
    displayOLEDWelcomeMessage();
    
    // Sensor system initialization
    Serial.println("🔍 Initializing sensor systems...");
    initSensors();
    
    // Actuator & audio system setup
    initActuators();
    initAudio();
    
    // Startup audio queue
    AudioEvent startupAudio{AUDIO_STARTUP, (uint32_t)millis()};
    if (audioQueue) {
        xQueueSend(audioQueue, &startupAudio, 0);
        Serial.println("🔊 Startup tone queued for Core 0 audio processing");
    }
    
    // Network & cloud services setup
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
    
    // Task creation & core assignment
    Serial.println("=== 🚀 Task Creation with Optimized Core Assignment ===");
    Serial.println("🔥 Core 0: WiFi + Blynk, system monitoring, alerts/buzzer");
    Serial.println("⚡ Core 1: Sensors, LCD, OLED, actuators, motion processing");
    
    // Core 1: Real-time processing tasks (High Priority)
    xTaskCreatePinnedToCore(TaskSensorPoll, "tSensors", 4096, nullptr, 4, &hTaskSensors, 1);
    xTaskCreatePinnedToCore(TaskActuators,  "tAct",     4096, nullptr, 5, &hTaskActuators, 1);
    xTaskCreatePinnedToCore(TaskLCD,        "tLCD",     3072, nullptr, 2, &hTaskLCD,      1);
    xTaskCreatePinnedToCore(TaskOLED,       "tOLED",    3072, nullptr, 2, &hTaskOLED,     1);
    
    // Core 0: Network & system services (Background Priority)
    xTaskCreatePinnedToCore(TaskWiFiBlynk,     "tWiFi",   4096, nullptr, 3, &hTaskWiFi,   0);
    xTaskCreatePinnedToCore(TaskSystemMonitor, "tSysMon", 3072, nullptr, 1, &hTaskSysMon, 0);
    
    // System resource information
    Serial.printf("💾 Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("⚡ CPU0 freq: %d MHz, CPU1 freq: %d MHz\n", ESP.getCpuFreqMHz(), ESP.getCpuFreqMHz());
    Serial.println("🎯 Setup complete! System ready for operation.");
}

void loop() {
    // Minimal loop - most work done by FreeRTOS tasks
    vTaskDelay(pdMS_TO_TICKS(1000));  // 1-second system heartbeat
}
