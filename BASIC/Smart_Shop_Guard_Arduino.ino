// 🏪 Smart Shop Guard - ESP32 Arduino IDE Compatible Implementation
// 🚀 Arduino IDE compatible version with no FreeRTOS dependencies
// 🛡️ Advanced security monitoring with sensor processing
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

// ========== CONFIGURATION ==========
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
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);  // 16x2 LCD Display
DHT dht(DHTPIN, DHTTYPE);                                    // Temperature & Humidity Sensor
Servo myServo;                                               // Security Door Control
SH1106Wire display(OLED_ADDRESS, OLED_SDA_PIN, OLED_SCL_PIN); // OLED Display object - 1.3" 128x64 display

// Network Configuration
char ssid[] = WIFI_SSID;      // WiFi Network Name
char pass[] = WIFI_PASSWORD;  // WiFi Password

// ========== SYSTEM STATE VARIABLES ==========
// System State Variables
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

// Alert State Management
static bool fireAlertActive = false;    // Fire Detection Active Flag
static bool motionAlertActive = false;  // Motion Detection Active Flag

// Arduino Communication Resources (replacing FreeRTOS)
// Simple flags and timers instead of queues and semaphores
static bool sensorDataReady = false;
static bool eventPending = false;
static bool audioPending = false;
static unsigned long lastSensorUpdate = 0;
static unsigned long lastEventCheck = 0;
static unsigned long lastAudioCheck = 0;

// Performance Optimization Settings
const unsigned long oledUpdateInterval = 1000;  // OLED Refresh Rate (ms)

// Task Management (simplified for Arduino)
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
    return digitalRead(FLAME_SENSOR_PIN) == LOW;  // LOW = flame detected
}

void readMotion() {
    bool currentPirState = digitalRead(PIR_PIN);
    
    if (currentPirState != lastPirState) {
        pirStabilityCounter++;
        if (pirStabilityCounter >= 3) {  // Require 3 consecutive readings
            motionDetected = currentPirState;
            lastPirState = currentPirState;
            pirStabilityCounter = 0;
            
            if (motionDetected) {
                Serial.println("🚨 Motion detected!");
            } else {
                Serial.println("✅ Motion cleared");
            }
        }
    } else {
        pirStabilityCounter = 0;
    }
}

bool isMotionDetected() {
    return motionDetected;
}

void triggerUltrasonicSensor() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    duration = pulseIn(ECHO_PIN, HIGH);
}

int getDistance() {
    return duration * 0.034 / 2;  // Convert to cm
}

// ========== ACTUATOR FUNCTIONS ==========
void initActuators() {
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    
    myServo.attach(SERVO_PIN);
    myServo.write(0);  // Initial position
}

void activateRelay() {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("🔥 Relay activated - Emergency response!");
}

void deactivateRelay() {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("✅ Relay deactivated - System safe");
}

void controlFan(int temperature, int humidity) {
    if (temperature > TEMP_THRESHOLD || humidity > HUMIDITY_THRESHOLD) {
        digitalWrite(FAN_PIN, HIGH);
        AC = true;
    } else {
        digitalWrite(FAN_PIN, LOW);
        AC = false;
    }
}

void moveServo() {
    SensorData latest{};
    if (sensorDataReady) {
        if (getDistance() <= DISTANCE_THRESHOLD) {
            degree = 180;
            lastTime = millis();
        } else if (millis() - lastTime > SERVO_DELAY) {
            degree = 0;
        }
        
        if (Servodegree != degree) {
            myServo.write(degree);
            Servodegree = degree;
            Serial.printf("🔄 Servo moved to %d degrees\n", degree);
        }
    }
}

// ========== AUDIO FUNCTIONS ==========
void initAudio() {
    pinMode(BUZZER_PIN, OUTPUT);
    buzzerInitialized = true;
}

static void writeTone(int frequency) {
    if (frequency <= 0) {
        noTone(BUZZER_PIN);  // Stop tone
    } else {
        tone(BUZZER_PIN, frequency);  // Play tone at frequency
    }
}

void playStartupTone() {
    if (!buzzerInitialized) return;
    
    int duration = 200;
    writeTone(523); // C5
    delay(duration + 20);
    writeTone(659); // E5
    delay(duration + 20);
    writeTone(784); // G5
    delay(duration + 20);
    writeTone(0);   // Stop
}

void playModeSwitchTone() {
    if (!buzzerInitialized) return;
    
    int duration = 150;
    writeTone(880); // A5
    delay(duration + 20);
    writeTone(0);   // Stop
}

void playAlertTone() {
    if (!buzzerInitialized) return;
    
    for (int i = 0; i < 3; i++) {
        int duration = 300;
        writeTone(1000);  // Alert frequency
        delay(duration);
        writeTone(0);
        delay(duration);
    }
}

// ========== DISPLAY FUNCTIONS ==========
void initDisplay() {
    lcd.init();
    lcd.backlight();
}

void displayWelcomeMessage() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Smart Shop Guard");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
    delay(STARTUP_DISPLAY_DELAY);
    
    lcd.clear();
    delay(200);
}

void displayProjectInfoSequence() {
    const char* messages[][2] = {
        {"Smart Shop Guard", "Security System"},
        {"ESP32 Powered", "IoT Solution"},
        {"Multi-Sensor", "Monitoring"},
        {"Fire Detection", "Motion Alerts"},
        {"WiFi Connected", "Cloud Ready"},
        {"System Ready", "Stay Secure!"}
    };
    
    for (int i = 0; i < 6; i++) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(messages[i][0]);
        lcd.setCursor(0, 1);
        lcd.print(messages[i][1]);
        delay(MODE_DISPLAY_DELAY);
    }
}

void displayLCDAnimations() {
    // Scrolling text animation
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(">>> SECURE <<<");
    lcd.setCursor(0, 1);
    lcd.print(">>> SMART  <<<");
    delay(MODE_DISPLAY_DELAY);
}

void displayNormalStatus() {
    static int currentSlide = 0;
    static unsigned long lastSlideChange = 0;
    static bool customCharsCreated = false;
    const unsigned long slideInterval = 3000; // 3 seconds per slide
    
    // Create custom characters only once
    if (!customCharsCreated) {
        // Custom character for temperature symbol
        byte tempChar[8] = {0x04, 0x0A, 0x0A, 0x0A, 0x0E, 0x1F, 0x1F, 0x0E};
        lcd.createChar(0, tempChar);
        
        // Custom character for humidity symbol
        byte humidChar[8] = {0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E, 0x00};
        lcd.createChar(1, humidChar);
        
        // Custom character for heart symbol
        byte heartChar[8] = {0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00};
        lcd.createChar(2, heartChar);
        
        customCharsCreated = true;
    }
    
    if (millis() - lastSlideChange >= slideInterval) {
        lcd.clear();
        
        switch (currentSlide) {
            case 0:
                lcd.setCursor(0, 0);
                lcd.print("Smart Shop Guard");
                lcd.setCursor(0, 1);
                lcd.print("Status: SECURE");
                break;
                
            case 1:
                lcd.setCursor(0, 0);
                lcd.print("Temp: ");
                lcd.write(0); // Temperature symbol
                lcd.print(t);
                lcd.print("C");
                lcd.setCursor(0, 1);
                lcd.print("Humidity: ");
                lcd.write(1); // Humidity symbol
                lcd.print(h);
                lcd.print("%");
                break;
                
            case 2:
                lcd.setCursor(0, 0);
                lcd.print("Fire: ");
                lcd.print(fireDetected ? "DETECTED!" : "Safe");
                lcd.setCursor(0, 1);
                lcd.print("Motion: ");
                lcd.print(motionDetected ? "DETECTED!" : "Clear");
                break;
                
            case 3:
                lcd.setCursor(0, 0);
                lcd.print("Distance: ");
                lcd.print(getDistance());
                lcd.print("cm");
                lcd.setCursor(0, 1);
                lcd.print("Door: ");
                lcd.print(getDistance() <= DISTANCE_THRESHOLD ? "OPEN" : "CLOSED");
                break;
                
            case 4:
                lcd.setCursor(0, 0);
                lcd.print("WiFi: ");
                lcd.print(WiFi.status() == WL_CONNECTED ? "Connected" : "Offline");
                lcd.setCursor(0, 1);
                lcd.print("AC: ");
                lcd.print(AC ? "ON" : "OFF");
                lcd.print(" Fan: ");
                lcd.print(AC ? "ON" : "OFF");
                break;
                
            case 5:
                lcd.setCursor(0, 0);
                lcd.print("Uptime: ");
                lcd.print((millis() - startTime) / 1000);
                lcd.print("s");
                lcd.setCursor(0, 1);
                lcd.print("Memory: ");
                lcd.print(ESP.getFreeHeap() / 1000);
                lcd.print("KB");
                break;
                
            case 6:
                lcd.setCursor(0, 0);
                lcd.print("System Ready");
                lcd.write(2); // Heart symbol
                lcd.setCursor(0, 1);
                lcd.print("All Systems GO!");
                break;
        }
        
        currentSlide = (currentSlide + 1) % 7; // 7 slides total
        lastSlideChange = millis();
    }
}

void displayFireAlert() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FIRE DETECTED!");
    lcd.setCursor(0, 1);
    lcd.print("EVACUATE NOW!");
}

void displayThiefAlert() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MOTION DETECTED!");
    lcd.setCursor(0, 1);
    lcd.print("SECURITY ALERT!");
}

void displaySafeStatus() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Safe");
    lcd.setCursor(0, 1);
    lcd.print("All Clear");
}

// ========== OLED DISPLAY FUNCTIONS ==========
void initOLEDDisplay() {
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
    display.clear();
    display.display();
}

void displayOLEDWelcomeMessage() {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Smart Shop");
    display.drawString(0, 20, "Guard");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 45, "Initializing...");
    display.display();
    delay(2000);
    
    // Animation sequence
    for (int i = 0; i < 3; i++) {
        display.clear();
        display.setFont(ArialMT_Plain_16);
        display.drawString(20 + i*5, 10, "SECURE");
        display.drawString(20 + i*5, 30, "SMART");
        display.display();
        delay(300);
    }
    
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 20, "System Ready!");
    display.display();
    delay(1000);
}

void showIntro() {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Smart Shop");
    display.drawString(0, 20, "Guard v2.0");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 45, "ESP32 Security System");
    display.display();
}

void showStatusPage() {
    display.drawXbm(0, 5, 16, 16, shield_icon);
    if(WiFi.status() == WL_CONNECTED){
        display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
    } else {
        display.drawXbm(120, 5, 8, 8, wifi_icon_disconnected);
    }
    if(false){ // Bluetooth not implemented
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
        if (WiFi.status() == WL_CONNECTED) {
            display.drawString(90, 20, "WiFi:OK");
        } else {
            display.drawString(90, 20, "WiFi:--");
        }
    }
}

void showSensorPage() {
    display.drawXbm(0, 5, 16, 16, temp_icon);
    if(WiFi.status() == WL_CONNECTED){
        display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
    } else {
        display.drawXbm(120, 5, 8, 8, wifi_icon_disconnected);
    }
    if(false){ // Bluetooth not implemented
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
    if(WiFi.status() == WL_CONNECTED){
        display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
    } else {
        display.drawXbm(120, 5, 8, 8, wifi_icon_disconnected);
    }
    if(false){ // Bluetooth not implemented
        display.drawXbm(110, 5, 8, 8, Bluetooth_icon);
    } else {
        display.drawXbm(110, 5, 8, 8, Bluetooth_icon_disconnected);
    }
    display.setFont(ArialMT_Plain_10);
    display.drawString(18, 5, "System");
    
    if (WiFi.status() == WL_CONNECTED) {
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
    if(WiFi.status() == WL_CONNECTED){
        display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
    } else {
        display.drawXbm(120, 5, 8, 8, wifi_icon_disconnected);
    }
    if(false){ // Bluetooth not implemented
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
    if(WiFi.status() == WL_CONNECTED){
        display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
    } else {
        display.drawXbm(120, 5, 8, 8, wifi_icon_disconnected);
    }
    if(false){ // Bluetooth not implemented
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
        } else if (currentState == STATE_SETTINGS) {
            if (nextState && !nextPressed) {
                currentSetting = (currentSetting + 1) % totalSettings;
                lastDebounce = millis();
                lastButtonPress = millis();
                Serial.println("Setting changed to: " + String(currentSetting));
            }
            
            if (prevState && !prevPressed) {
                // Execute setting action
                if (currentSetting == 0) {
                    oledConfig.auto_swipe = !oledConfig.auto_swipe;
                    Serial.println("Auto-swipe toggled: " + String(oledConfig.auto_swipe ? "ON" : "OFF"));
                }
                lastDebounce = millis();
                lastButtonPress = millis();
            }
        }
    }
    
    nextPressed = nextState;
    prevPressed = prevState;
}

void updateOLEDDisplay() {
    display.clear();
    
    // Auto-swipe functionality
    if (oledConfig.auto_swipe && millis() - lastAutoSwipe > autoSwipeDelay) {
        currentPage = (currentPage + 1) % totalPages;
        lastAutoSwipe = millis();
    }
    
    // Update alert flashing
    if ((fireDetected || motionDetected) && millis() - lastAnimation > 500) {
        alertFlashing = !alertFlashing;
        lastAnimation = millis();
    }
    
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
    
    display.display();
}

void displayOLEDFireAlert() {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "FIRE ALERT!");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 25, "EVACUATE NOW!");
    display.drawString(0, 40, "Emergency Response");
    display.drawString(0, 55, "System Activated");
    display.display();
}

void displayOLEDThiefAlert() {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "THIEF ALERT!");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 25, "Motion Detected!");
    display.drawString(0, 40, "Security Breach");
    display.drawString(0, 55, "Location: Door");
    display.display();
}

void displayOLEDSafeStatus() {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 20, "System Safe");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 45, "All Clear");
    display.display();
}

void displayOLEDMotionCleared() {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 20, "Motion Cleared");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 45, "System Normal");
    display.display();
}

// ========== SYSTEM FUNCTIONS ==========
void initSystem() {
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println("🚀 Smart Shop Guard - Arduino Version Starting...");
    
    Wire.begin();
    startTime = millis();
}

bool initWiFi() {
    WiFi.begin(ssid, pass);
    Serial.print("🌐 Connecting to WiFi");
    
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < WIFI_TIMEOUT) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.printf("✅ WiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    } else {
        Serial.println();
        Serial.println("❌ WiFi connection failed!");
        return false;
    }
}

bool isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void handleWiFiReconnection() {
    static unsigned long lastReconnectAttempt = 0;
    const unsigned long reconnectInterval = 30000; // 30 seconds
    
    if (!isWiFiConnected() && millis() - lastReconnectAttempt > reconnectInterval) {
        Serial.println("🔄 Attempting WiFi reconnection...");
        WiFi.disconnect();
        WiFi.begin(ssid, pass);
        lastReconnectAttempt = millis();
    }
}

void initBlynk() {
    Blynk.config(BLYNK_AUTH_TOKEN);
}

void connectBlynk() {
    if (isWiFiConnected()) {
        Blynk.connect();
    }
}

void sendSensorDataToBlynk(int temperature, int humidity, bool flame, bool motion) {
    if (isWiFiConnected() && Blynk.connected()) {
        Blynk.virtualWrite(VPIN_TEMPERATURE, temperature);
        Blynk.virtualWrite(VPIN_HUMIDITY, humidity);
        Blynk.virtualWrite(VPIN_FLAME, flame ? 1 : 0);
        Blynk.virtualWrite(VPIN_MOTION, motion ? 1 : 0);
        Blynk.virtualWrite(VPIN_DAY_NIGHT, isDay ? 1 : 0);
        Blynk.virtualWrite(VPIN_AC_CONTROL, AC ? 1 : 0);
    }
}

// Blynk handlers
BLYNK_WRITE(VPIN_AC_CONTROL) {
    int value = param.asInt();
    AC = (value == 1);
    digitalWrite(FAN_PIN, AC ? HIGH : LOW);
    Serial.printf("🌀 AC control via Blynk: %s\n", AC ? "ON" : "OFF");
}

BLYNK_WRITE(VPIN_DAY_NIGHT) {
    int value = param.asInt();
    isDay = (value == 1);
    Serial.printf("🌅 Day/Night mode via Blynk: %s\n", isDay ? "Day" : "Night");
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
        }
    } else {
        fireChangeCounter = 0;
    }

    readMotion();
    
    triggerUltrasonicSensor();
    
    // Log distance for debugging
    Serial.print("📏 Distance: ");
    Serial.print(getDistance());
    Serial.println(" cm");
    
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
        // Play fire alert tone
        playAlertTone();
    } else if (!fireDetected && fireAlertActive) {
        fireAlertActive = false;
        Serial.println("✅ Fire cleared - Deactivating relay and displays");
        deactivateRelay();
        displaySafeStatus();
        displayOLEDSafeStatus();
    }
    
    // Handle motion alerts
    if (motionDetected && !motionAlertActive) {
        motionAlertActive = true;
        Serial.println("🚨 MOTION ALERT ACTIVATED - Displaying thief alert");
        displayThiefAlert();
        displayOLEDThiefAlert();
        // Play motion alert tone
        playAlertTone();
    } else if (!motionDetected && motionAlertActive) {
        motionAlertActive = false;
        Serial.println("✅ Motion cleared - Returning to normal status");
        displayNormalStatus();
        displayOLEDMotionCleared();
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
    
    // Process audio events (simplified - no complex queuing)
    if (audioPending) {
        Serial.println("🔊 Processing audio event");
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
    
    // Play startup tone
    Serial.println("🔊 Playing startup tone...");
    playStartupTone();
    
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
    
    // Arduino-style task management
    Serial.println("=== 🚀 Arduino Task Management ===");
    Serial.println("🔄 All tasks will run in the main loop with timing control");
    
    // System resource information
    Serial.printf("💾 Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("⚡ CPU freq: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.println("🎯 Setup complete! System ready for operation.");
}

void loop() {
    // Call all handler functions with appropriate timing
    handleWiFiBlynk();
    handleLCD();
    handleOLED();
    handleSensors();
    handleActuators();
    handleSystemMonitor();
    
    // Small delay to prevent overwhelming the system
    delay(10);
}
