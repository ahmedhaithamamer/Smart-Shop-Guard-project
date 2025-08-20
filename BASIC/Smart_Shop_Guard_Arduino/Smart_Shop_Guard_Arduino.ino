// 🏪 Smart Shop Guard - ESP32 Arduino IDE Compatible Implementation
// 🚀 Dual-core architecture for optimal performance and reliability
// 🛡️ Advanced security monitoring with real-time sensor processing
// Arduino IDE Compatible Version - Single File Structure

// ========== BLYNK CONFIGURATION (MUST BE FIRST) ==========
#define BLYNK_TEMPLATE_ID "TMPL2jt8pOqfP"
#define BLYNK_TEMPLATE_NAME "Smart Secure Smart Shop"
#define BLYNK_AUTH_TOKEN "LVODBytZ9qETpIjcYFFx5-4b4UFfRUdg"

// ========== INCLUDES ==========
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SH1106Wire.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <Adafruit_NeoPixel.h>

// ========== ARDUINO COMPATIBILITY ==========
// Using native Arduino functions - no compatibility macros needed

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

// LED Strip Configuration
#define LED_PIN 20          // GPIO pin for NeoPixel strip (ESP32-S3 compatible)
#define NUM_PIXELS 31       // Number of NeoPixels in the strip
#define LED_BRIGHTNESS 100  // Global brightness (0-255)

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

// LED Effect Timing
#define LED_BOOT_DELAY 100      // Boot animation speed
#define LED_EFFECT_DELAY 50     // General effect speed
#define LED_STABLE_DELAY 2000   // Stable mode duration

// LED Color Definitions (RGB values)
#define LED_COLOR_OFF      0, 0, 0
#define LED_COLOR_BLUE     0, 100, 255    // Security/Normal
#define LED_COLOR_GREEN    0, 255, 100    // Safe/OK status
#define LED_COLOR_ORANGE   255, 165, 0    // Warning/Caution
#define LED_COLOR_RED      255, 0, 0      // Alert/Danger
#define LED_COLOR_PURPLE   128, 0, 128    // System/Boot
#define LED_COLOR_WHITE    255, 255, 255  // Bright/Active
#define LED_COLOR_CYAN     0, 255, 255    // Info/Data

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
Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800); // NeoPixel LED Strip

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
bool isNight = true ;                // Day/Night Mode Flag
bool AC = false;                   // Air Conditioning Status
unsigned long lastStatusPrint = 0; // Last Status Display Update
unsigned long lastOLEDUpdate = 0;  // Last OLED Screen Update

// 🚨 Alert State Management
static bool fireAlertActive = false;    // Fire Detection Active Flag
static bool motionAlertActive = false;  // Motion Detection Active Flag

// 🔄 Arduino Communication Resources (replacing FreeRTOS)
// Simple flags and timers instead of queues and semaphores
static bool sensorDataReady = false;
static bool eventPending = false;
static bool audioPending = false;
static unsigned long lastSensorUpdate = 0;
static unsigned long lastEventCheck = 0;
static unsigned long lastAudioCheck = 0;

// ⏱️ Performance Optimization Settings
const unsigned long oledUpdateInterval = 1000;  // OLED Refresh Rate (ms)

// 📋 Task Management (simplified for Arduino)
static unsigned long lastSensorTask = 0;
static unsigned long lastActuatorTask = 0;
static unsigned long lastLCDTask = 0;
static unsigned long lastOLEDTask = 0;
static unsigned long lastWiFiTask = 0;
static unsigned long lastSysMonTask = 0;

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

// LED System Variables
enum LEDMode {
  LED_BOOT_MODE,
  LED_SECURE_MODE,
  LED_ALERT_MODE,
  LED_SCAN_MODE,
  LED_FIRE_MODE,
  LED_MOTION_MODE,
  LED_OFF_MODE
};

LEDMode currentLEDMode = LED_BOOT_MODE;
unsigned long lastLEDUpdate = 0;
unsigned long lastLEDModeChange = 0;

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
            if (currentPirState && isNight) {  // Motion detected during NIGHT - trigger thief alert
                motionDetected = true;
                Serial.println("🚨 Motion detected at NIGHT - Thief alert triggered");
            } else if (currentPirState && !isNight) {
                Serial.println("👥 Motion detected during DAY - No thief alert (normal operation)");
                motionDetected = false; // No alert during day
            } else if (!currentPirState) {
                motionDetected = false;
                Serial.println("✅ Motion cleared");
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
    if (AC) {
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
    if (sensorDataReady) { // Changed from xQueuePeek to sensorDataReady
        if ((getDistance() <= DISTANCE_THRESHOLD && !isNight) || (getDistance() == 999 && !isNight)){
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

// ========== AUDIO/BUZZER FUNCTIONS ==========

static void buzzerAttach() {
    if (buzzerInitialized) return;
    // No hardware setup needed - Tone library handles everything
    buzzerInitialized = true;
}

static void writeTone(int frequency) {
    buzzerAttach();
    if (frequency <= 0) {
        noTone(BUZZER_PIN);  // Stop tone
    } else {
        tone(BUZZER_PIN, frequency);  // Play tone at frequency
    }
}

void initAudio() {
    Serial.printf("Initializing audio on pin %d\n", BUZZER_PIN);
    pinMode(BUZZER_PIN, OUTPUT);
    buzzerAttach();
    noTone(BUZZER_PIN);  // Ensure silent start
    Serial.println("Audio initialization complete");
}

void playStartupTone() {
    Serial.println("Playing startup tone...");
    int startupNotes[] = {523, 659, 784, 1047, 1319, 1568};
    int duration = 150;
    for (unsigned i = 0; i < sizeof(startupNotes) / sizeof(startupNotes[0]); i++) {
        Serial.printf("Playing note %d Hz\n", startupNotes[i]);
        writeTone(startupNotes[i]);
        delay(duration + 20);
    }
    noTone(BUZZER_PIN); // Ensure silent end
    Serial.println("Startup tone complete");
}

void playWelcomingSound() {
    if (!buzzerInitialized) return;
    
    Serial.println("🎵 Playing welcoming sound...");
    
    // Pleasant welcoming melody - "Welcome to Smart Shop Guard" style
    // Musical phrase that sounds friendly and professional
    int welcomeMelody[] = {
        523,  // C5 - "Wel-"
        659,  // E5 - "come"
        784,  // G5 - "to"
        523,  // C5 - "Smart"
        659,  // E5 - "Shop"
        1047, // C6 - "Guard!"
        784,  // G5 - ending flourish
        1047  // C6 - final high note
    };
    
    int welcomeDurations[] = {
        250,  // Wel-
        250,  // come
        200,  // to
        200,  // Smart
        200,  // Shop
        400,  // Guard!
        200,  // flourish
        500   // final note
    };
    
    // Play the welcoming melody
    for (int i = 0; i < 8; i++) {
        writeTone(welcomeMelody[i]);
        delay(welcomeDurations[i]);
        writeTone(0); // Brief pause between notes
        delay(50);
    }
    
    // Final silence
    noTone(BUZZER_PIN);
    Serial.println("🎵 Welcome sound complete - System ready!");
}

void playModeSwitchTone() {
    int switchNotes[] = {784, 988, 1175};
    int duration = 120;
    for (unsigned i = 0; i < sizeof(switchNotes) / sizeof(switchNotes[0]); i++) {
        writeTone(switchNotes[i]);
        delay(duration + 20);
    }
    noTone(BUZZER_PIN); // Ensure silent end
}

void playEnhancedModeSwitchTone() {
    Serial.println("🎵 Playing enhanced mode switch sound...");
    
    if (isNight) {
        // Night mode activation: Descending tones (getting serious)
        int nightNotes[] = {1047, 880, 659, 523, 440}; // C6 -> A4
        int nightDurations[] = {150, 150, 150, 150, 300};
        
        for (int i = 0; i < 5; i++) {
            writeTone(nightNotes[i]);
            delay(nightDurations[i]);
            writeTone(0); // Brief pause
            delay(50);
        }
        Serial.println("🌙 Night mode sound complete - Security enhanced!");
    } else {
        // Day mode activation: Ascending tones (getting brighter)
        int dayNotes[] = {440, 523, 659, 784, 1047}; // A4 -> C6
        int dayDurations[] = {150, 150, 150, 150, 300};
        
        for (int i = 0; i < 5; i++) {
            writeTone(dayNotes[i]);
            delay(dayDurations[i]);
            writeTone(0); // Brief pause
            delay(50);
        }
        Serial.println("☀️ Day mode sound complete - Normal operations!");
    }
    
    noTone(BUZZER_PIN); // Ensure silent end
    Serial.println("🎵 Enhanced mode switch sound complete!");
}

void playAlertTone() {
    Serial.println("Playing alert tone...");
    int duration = 150;
    int tone1 = 880;
    int tone2 = 440;
    for (int i = 0; i < 3; i++) {
        Serial.printf("Alert tone cycle %d: %d Hz\n", i+1, tone1);
        writeTone(tone1);
        delay(duration);
        writeTone(tone2);
        delay(duration);
    }
    noTone(BUZZER_PIN); // Ensure silent end
    Serial.println("Alert tone complete");
}

void playContinuousAlertTone() {
    static unsigned long lastAlertTone = 0;
    const unsigned long alertToneInterval = 3000; // Play every 3 seconds during alert
    
    if (millis() - lastAlertTone >= alertToneInterval) {
        playAlertTone();
        lastAlertTone = millis();
    }
}

void playTone(int frequency, int duration) {
    writeTone(frequency);
    if (duration > 0) {
        delay(duration);
        noTone(BUZZER_PIN); // Ensure silent end
    }
}

void stopTone() {
    noTone(BUZZER_PIN);
}

// ========== LED EFFECTS FUNCTIONS ==========
void initLEDStrip() {
    Serial.println("🌈 Initializing NeoPixel LED strip...");
    strip.begin();
    strip.setBrightness(LED_BRIGHTNESS);
    strip.show(); // Initialize all pixels to 'off'
    Serial.printf("✅ LED strip initialized: %d pixels on pin %d\n", NUM_PIXELS, LED_PIN);
}

// 🚀 Boot Sequence - Smart Shop Guard startup animation
void ledBootSequence() {
    Serial.println("🚀 LED Boot Sequence Starting...");
    
    // Phase 1: Purple wave - System initialization
    for(int wave = 0; wave < 2; wave++) {
        for(int i = 0; i < NUM_PIXELS; i++) {
            strip.clear();
            // Create a moving wave of purple
            for(int j = 0; j < 5; j++) {
                int pos = (i + j) % NUM_PIXELS;
                int brightness = 255 - (j * 50);
                strip.setPixelColor(pos, brightness/2, 0, brightness);
            }
            strip.show();
            delay(LED_BOOT_DELAY);
        }
    }
    
    // Phase 2: Blue pulse - Security systems online
    for(int pulse = 0; pulse < 3; pulse++) {
        for(int brightness = 0; brightness <= 200; brightness += 20) {
            for(int i = 0; i < NUM_PIXELS; i++) {
                strip.setPixelColor(i, 0, brightness/3, brightness);
            }
            strip.show();
            delay(30);
        }
        for(int brightness = 200; brightness >= 0; brightness -= 20) {
            for(int i = 0; i < NUM_PIXELS; i++) {
                strip.setPixelColor(i, 0, brightness/3, brightness);
            }
            strip.show();
            delay(30);
        }
    }
    
    // Phase 3: Green confirmation - All systems ready
    for(int i = 0; i < NUM_PIXELS; i++) {
        strip.setPixelColor(i, LED_COLOR_GREEN);
        strip.show();
        delay(LED_BOOT_DELAY/2);
    }
    delay(1000);
    
    // Fade to stable blue
    ledFadeToColor(LED_COLOR_BLUE, 30);
    currentLEDMode = LED_SECURE_MODE;
}

// 🛡️ Secure Mode - Stable operation with gentle breathing
void ledSecureMode() {
    static unsigned long lastBreath = 0;
    static int breathDirection = 1;
    static int breathLevel = 100;
    
    if (millis() - lastBreath > 40) {
        breathLevel += breathDirection * 2;
        
        if (breathLevel >= 180) {
            breathDirection = -1;
        } else if (breathLevel <= 60) {
            breathDirection = 1;
        }
        
        // Apply breathing effect to all pixels - different colors for day/night
        for(int i = 0; i < NUM_PIXELS; i++) {
            if (isNight) {
                // Night mode: Deep blue breathing
                strip.setPixelColor(i, 0, breathLevel/6, breathLevel);
            } else {
                // Day mode: Gentle green breathing
                strip.setPixelColor(i, 0, breathLevel, breathLevel/3);
            }
        }
        strip.show();
        lastBreath = millis();
    }
}

// 🚨 Fire Alert Mode - Rapid red flashing with siren pattern
void ledFireAlertMode() {
    static unsigned long lastAlert = 0;
    static bool alertState = false;
    static int alertPhase = 0;
    
    if (millis() - lastAlert > 150) {
        alertState = !alertState;
        alertPhase = (alertPhase + 1) % 4;
        
        if (alertState) {
            // Create siren-like sweeping effect
            strip.clear();
            for(int i = 0; i < NUM_PIXELS/2; i++) {
                int pos1 = (alertPhase * NUM_PIXELS/4 + i) % NUM_PIXELS;
                int pos2 = (alertPhase * NUM_PIXELS/4 - i + NUM_PIXELS) % NUM_PIXELS;
                strip.setPixelColor(pos1, LED_COLOR_RED);
                strip.setPixelColor(pos2, LED_COLOR_RED);
            }
        } else {
            // Dim red glow
            for(int i = 0; i < NUM_PIXELS; i++) {
                strip.setPixelColor(i, 50, 0, 0);
            }
        }
        
        strip.show();
        lastAlert = millis();
    }
}

// 🚨 Motion Alert Mode - Orange pulsing warning
void ledMotionAlertMode() {
    static unsigned long lastMotion = 0;
    static bool motionState = false;
    
    if (millis() - lastMotion > 300) {
        motionState = !motionState;
        
        if (motionState) {
            // Bright orange flash
            for(int i = 0; i < NUM_PIXELS; i++) {
                strip.setPixelColor(i, LED_COLOR_ORANGE);
            }
        } else {
            // Dim orange glow
            for(int i = 0; i < NUM_PIXELS; i++) {
                strip.setPixelColor(i, 100, 60, 0);
            }
        }
        
        strip.show();
        lastMotion = millis();
    }
}

// 🔍 Scan Mode - Rotating scanner like security camera
void ledScanMode() {
    static unsigned long lastScan = 0;
    static int scanPos = 0;
    static int scanDirection = 1;
    
    if (millis() - lastScan > LED_EFFECT_DELAY) {
        strip.clear();
        
        // Create scanning beam
        for(int i = 0; i < 7; i++) {
            int pos = (scanPos + i) % NUM_PIXELS;
            int intensity = 255 - (i * 35);
            strip.setPixelColor(pos, 0, intensity, intensity);
        }
        
        // Add trailing dots
        for(int i = 8; i < 12; i++) {
            int pos = (scanPos + i) % NUM_PIXELS;
            strip.setPixelColor(pos, 0, 30, 100);
        }
        
        scanPos += scanDirection;
        if (scanPos >= NUM_PIXELS || scanPos < 0) {
            scanDirection *= -1;
            scanPos += scanDirection;
        }
        
        strip.show();
        lastScan = millis();
    }
}

// 🎨 Utility function to fade all pixels to a specific color
void ledFadeToColor(uint8_t r, uint8_t g, uint8_t b, int steps) {
    for(int step = 0; step < steps; step++) {
        for(int i = 0; i < NUM_PIXELS; i++) {
            uint32_t currentColor = strip.getPixelColor(i);
            uint8_t currentR = (currentColor >> 16) & 0xFF;
            uint8_t currentG = (currentColor >> 8) & 0xFF;
            uint8_t currentB = currentColor & 0xFF;
            
            uint8_t newR = currentR + ((r - currentR) * step / steps);
            uint8_t newG = currentG + ((g - currentG) * step / steps);
            uint8_t newB = currentB + ((b - currentB) * step / steps);
            
            strip.setPixelColor(i, newR, newG, newB);
        }
        strip.show();
        delay(20);
    }
}

// 🔄 Clear all pixels
void ledClearAll() {
    strip.clear();
    strip.show();
}

// 💡 Set all pixels to the same color
void ledSetAllColor(uint8_t r, uint8_t g, uint8_t b) {
    for(int i = 0; i < NUM_PIXELS; i++) {
        strip.setPixelColor(i, r, g, b);
    }
    strip.show();
}

// 🎯 Update LED effects based on system state
void updateLEDEffects() {
    // Determine LED mode based on system state
    LEDMode targetMode = LED_SECURE_MODE; // Default
    
    if (fireDetected && fireAlertActive) {
        targetMode = LED_FIRE_MODE;
    } else if (motionDetected && motionAlertActive) {
        targetMode = LED_MOTION_MODE;
    } else if (sensorDataReady && getDistance() <= DISTANCE_THRESHOLD) {
        targetMode = LED_SCAN_MODE; // Door activity
    } else {
        targetMode = LED_SECURE_MODE; // Normal operation
    }
    
    // Change mode if needed
    if (currentLEDMode != targetMode) {
        currentLEDMode = targetMode;
        lastLEDModeChange = millis();
        
        // Log mode change
        switch(currentLEDMode) {
            case LED_SECURE_MODE: Serial.println("🌈 LED Mode: SECURE"); break;
            case LED_FIRE_MODE: Serial.println("🌈 LED Mode: FIRE ALERT"); break;
            case LED_MOTION_MODE: Serial.println("🌈 LED Mode: MOTION ALERT"); break;
            case LED_SCAN_MODE: Serial.println("🌈 LED Mode: SCANNING"); break;
            default: break;
        }
    }
    
    // Execute current mode effects
    switch(currentLEDMode) {
        case LED_BOOT_MODE:
            // Boot sequence is handled separately
            break;
        case LED_SECURE_MODE:
            ledSecureMode();
            break;
        case LED_FIRE_MODE:
            ledFireAlertMode();
            break;
        case LED_MOTION_MODE:
            ledMotionAlertMode();
            break;
        case LED_SCAN_MODE:
            ledScanMode();
            break;
        case LED_OFF_MODE:
            ledClearAll();
            break;
    }
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
        delay(STARTUP_DISPLAY_DELAY);
        yield();
        lcd.clear();
        delay(200);
        yield();
    }
}

void displayModeStatus() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    lcd.print(isNight ? "Night" : "Day");  // Fixed mode display logic
    delay(MODE_DISPLAY_DELAY);
    yield();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Smart Shop Guard");
    delay(MODE_DISPLAY_DELAY);
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
    delay(100);
    
    Serial.println("OLED buttons initialized - NEXT: " + String(BUTTON_NEXT) + ", PREV: " + String(BUTTON_NEXT) + ", PREV: " + String(BUTTON_PREV));
    
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
        delay(100);
    }
    
    delay(200);
    
    String line2 = "Guard";
    for (int i = 0; i <= line2.length(); i++) {
        display.clear();
        display.drawString(15, 15, line1);
        display.drawString(35, 30, line2.substring(0, i));
        display.display();
        delay(100);
    }
    
    delay(300);
    
    for (int i = 0; i < 2; i++) {
        display.clear();
        display.display();
        delay(150);
        
        display.clear();
        display.drawString(15, 15, "Smart Shop");
        display.drawString(35, 30, "Guard");
        display.display();
        delay(200);
    }
    
    // Show connection status after intro
    showConnectionStatus();
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
    display.drawString(15, 35, isNight ? "Night" : "Day");
    display.display();
    delay(1000);
    
    // Show connection status after mode change
    showConnectionStatus();
    delay(1000);
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
    display.drawString(0, 30, "Mode: " + String(isNight ? "Night" : "Day"));
    display.drawString(0, 40, "Uptime: " + String(millis() / 1000) + "s");
    display.drawString(0, 50, "Memory: " + String(ESP.getFreeHeap()/1000) + "KB");
}

void showConnectionStatus() {
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Connection Status:");
    
    // WiFi Status
    if (isWiFiConnected()) {
        display.drawXbm(0, 15, 8, 8, wifi_icon_connected);
        display.drawString(12, 15, "WiFi: Connected");
        display.drawString(12, 25, "IP: " + WiFi.localIP().toString());
    } else {
        display.drawXbm(0, 15, 8, 8, wifi_icon_disconnected);
        display.drawString(12, 15, "WiFi: Disconnected");
        display.drawString(12, 25, "Attempting reconnect...");
    }
    
    // Blynk Status
    if (isBlynkConnected()) {
        display.drawString(0, 35, "Blynk: Connected");
    } else {
        display.drawString(0, 35, "Blynk: Offline");
    }
    
    // System Status
    display.drawString(0, 45, "Mode: " + String(isNight ? "Night" : "Day"));
    display.drawString(0, 55, "System: Ready");
    
    display.display();
    delay(2000);
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
            delay(100);
            display.normalDisplay();
            delay(100);
            display.invertDisplay();
            delay(100);
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
    delay(100);
    WiFi.begin(ssid, pass);
    
    unsigned long wifiConnectStart = millis();
    Serial.print("🔄 Connecting to WiFi");
    
    while (WiFi.status() != WL_CONNECTED && millis() - wifiConnectStart < 5000) {
        delay(500);
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
        delay(50);
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
    bool previousMode = isNight;
    isNight = param.asInt();
    
    // Only process if mode actually changed
    if (previousMode != isNight) {
        Serial.println("🌙 Mode switch: " + String(isNight ? "DAY -> NIGHT" : "NIGHT -> DAY"));
        
        // Play enhanced mode switch tone
        Serial.println("[AUDIO] Playing enhanced mode switch tone");
        playEnhancedModeSwitchTone();
        
        // Clear any active motion alerts when switching to day mode
        if (!isNight && motionAlertActive) {
            motionAlertActive = false;
            motionDetected = false;
            Serial.println("✅ Day mode activated - Clearing motion alerts");
            displayNormalStatus();
            displayOLEDMotionCleared();
        }
        
        // Display mode status
        displayModeStatus();
        displayOLEDModeStatus();
        
        // Update LED effects for new mode
        if (isNight) {
            Serial.println("🌙 Night mode: Enhanced security monitoring active");
        } else {
            Serial.println("☀️ Day mode: Normal operations");
        }
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

// ========== ARDUINO TASK FUNCTIONS ==========
void handleWiFiBlynk() {
    static unsigned long lastBlynkSend = 0;
    const unsigned long blynkSendInterval = 2000;
    
    handleWiFiReconnection();
    
    if (isWiFiConnected()) {
        Blynk.run();
        
        if (millis() - lastBlynkSend >= blynkSendInterval) {
            if (sensorDataReady) {
                sendSensorDataToBlynk(t, h, fireDetected, motionDetected);
                lastBlynkSend = millis();
            }
        }
    }
}

void handleLCD() {
    static unsigned long lastNormalUpdate = 0;
    
    if (!fireAlertActive && !motionAlertActive && (millis() - lastNormalUpdate > 5000)) {
        displayNormalStatus();
        lastNormalUpdate = millis();
    }
}

void handleOLED() {
    handleOLEDButtons();
    updateOLEDDisplay();
}

void handleSensors() {
    static bool fireStable = false;
    static int fireChangeCounter = 0;
    static unsigned long lastSensorRead = 0;
    const unsigned long sensorReadInterval = 500; // Read sensors every 500ms
    
    // Only read sensors at specified intervals to prevent overwhelming
    if (millis() - lastSensorRead < sensorReadInterval) {
        return;
    }
    lastSensorRead = millis();
    
    readTemperatureHumidity();
    
    readFlameSensor();
    bool fireRaw = isFlameDetected();

    // Debounce fire: require 3 consecutive samples to change state
    if (fireRaw != fireStable) {
        fireChangeCounter++;
        if (fireChangeCounter >= 3) {
            fireStable = fireRaw;
            fireDetected = fireStable;
            fireChangeCounter = 0;
            Serial.println("🔥 Fire detection state changed: " + String(fireDetected ? "DETECTED" : "CLEARED"));
        }
    } else {
        fireChangeCounter = 0;
    }

    readMotion();
    
    triggerUltrasonicSensor();
    
    // Log distance for debugging (less frequently)
    static unsigned long lastDistanceLog = 0;
    if (millis() - lastDistanceLog > 2000) {
        Serial.print("📏 Distance: ");
        Serial.print(getDistance());
        Serial.println(" cm");
        lastDistanceLog = millis();
    }
    
    sensorDataReady = true;
    lastSensorUpdate = millis();
}

void handleActuators() {
    static unsigned long lastActuatorUpdate = 0;
    const unsigned long actuatorInterval = 100; // 100ms interval
    
    // Only run every actuatorInterval milliseconds
    if (millis() - lastActuatorUpdate < actuatorInterval) {
        return;
    }
    lastActuatorUpdate = millis();
    
    // Handle fire alerts
    if (fireDetected && !fireAlertActive) {
        fireAlertActive = true;
        Serial.println("🔥 FIRE ALERT ACTIVATED - Activating relay and displays");
        activateRelay();
        displayFireAlert();
        displayOLEDFireAlert();
        // Play initial fire alert tone
        playAlertTone();
    } else if (!fireDetected && fireAlertActive) {
        fireAlertActive = false;
        Serial.println("✅ Fire cleared - Deactivating relay and displays");
        deactivateRelay();
        displaySafeStatus();
        displayOLEDSafeStatus();
    }
    
    // Continue playing alert tones while fire is detected
    if (fireDetected && fireAlertActive) {
        playContinuousAlertTone();
    }
    
    // Handle motion alerts (only during night mode)
    if (motionDetected && isNight && !motionAlertActive) {
        motionAlertActive = true;
        Serial.println("🚨 MOTION ALERT ACTIVATED - Displaying thief alert");
        displayThiefAlert();
        displayOLEDThiefAlert();
        // Play initial motion alert tone
        playAlertTone();
    } else if ((!motionDetected || !isNight) && motionAlertActive) {
        motionAlertActive = false;
        Serial.println("✅ Motion cleared or day mode - Returning to normal status");
        displayNormalStatus();
        displayOLEDMotionCleared();
    }
    
    // Continue playing alert tones while motion is detected during night
    if (motionDetected && isNight && motionAlertActive) {
        playContinuousAlertTone();
    }
    
    // Control fan and servo based on sensor data
    if (sensorDataReady) {
        controlFan(t, h);  // Use global t and h variables
        moveServo();
    }
}

void handleSystemMonitor() {
    static unsigned long lastHeapReport = 0;
    static unsigned long lastSystemUpdate = 0;
    const unsigned long systemInterval = 1000; // 1 second interval
    
    // Only run every systemInterval milliseconds
    if (millis() - lastSystemUpdate < systemInterval) {
        return;
    }
    lastSystemUpdate = millis();
    
    // Process audio events
    if (audioPending) {
        Serial.println("🔊 Processing audio event");
        // For now, just reset the flag since we're not using complex audio queuing
        audioPending = false;
    }
    
    // System monitoring (every 30 seconds)
    if (millis() - lastHeapReport > 30000) {
        Serial.printf("💾 Free heap: %d bytes\n", ESP.getFreeHeap());
        lastHeapReport = millis();
    }
}

// ========== ARDUINO SETUP & LOOP ==========
void setup() {
    // Initialize serial communication
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println("🚀 Smart Shop Guard - Arduino IDE Version Starting...");
    
    // Power stabilization delay
    delay(1000);
    
    // System initialization
    initSystem();
    
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
    
    // LED system initialization
    Serial.println("🌈 Initializing LED effects system...");
    initLEDStrip();
    
    // Play welcoming sound after audio initialization
    Serial.println("🔊 Playing welcoming sound...");
    playWelcomingSound();
    
    // LED boot sequence
    Serial.println("🌈 Starting LED boot sequence...");
    ledBootSequence();
    
    // Network & cloud services setup
    Serial.println("🌐 Attempting WiFi connection (5 second timeout)...");
    if (initWiFi()) {
        Serial.println("✅ WiFi connected, initializing Blynk cloud service...");
        initBlynk();
        if (connectBlynk()) {
            Serial.println("☁️ Cloud services initialized successfully!");
        } else {
            Serial.println("⚠️ Blynk connection failed - WiFi available but cloud offline");
        }
    } else {
        Serial.println("⚠️ Starting in offline mode...");
        Serial.println("🔄 System will attempt WiFi reconnection every 30 seconds");
    }
    
    // Arduino-style task management
    Serial.println("=== 🚀 Arduino Task Management ===");
    Serial.println("🔄 All tasks will run in the main loop with timing control");
    
    // System resource information
    Serial.printf("💾 Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("⚡ CPU freq: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.println("🎯 Setup complete! System ready for operation.");
    
    // Final system ready notification sound
    Serial.println("🎵 System fully initialized - Playing ready notification...");
    delay(500); // Brief pause before final sound
    playStartupTone(); // Play the startup tone sequence to indicate system ready
}

void loop() {
    // Call all handler functions with appropriate timing
    handleWiFiBlynk();
    handleLCD();
    handleOLED();
    handleSensors();
    handleActuators();
    handleSystemMonitor();
    
    // Update LED effects based on current system state
    updateLEDEffects();
    
    // Yield to allow ESP32 to handle background tasks
    yield();
    
    // Small delay to prevent overwhelming the system
    delay(10);
}
