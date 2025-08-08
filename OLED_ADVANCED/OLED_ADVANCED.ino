#include <Wire.h>
#include <SH1106Wire.h>
#include <ESP8266WiFi.h>
#include "icons.h"

// Button definitions
#define BUTTON_NEXT D5
#define BUTTON_PREV D6

// Display setup for 1.3" 128x64 OLED
SH1106Wire display(0x3C, D2, D1); // SDA = D2, SCL = D1

// Hardcoded WiFi credentials
const char* ssid = "Amer_ABF";
const char* password = "Amer_Ham_2020";

// Simple configuration
struct Config {
  int theme;
  bool auto_swipe;
} config;

// Display themes
enum DisplayTheme {
  THEME_NORMAL = 0,
  THEME_INVERT = 1
};

// Page types (5 pages like OLED_1)
enum PageType {
  PAGE_STATUS = 0,
  PAGE_SENSORS = 1,
  PAGE_SYSTEM = 2,
  PAGE_ALERTS = 3,
  PAGE_SETTINGS = 4
};

// Menu state
enum MenuState {
  STATE_PAGES = 0,
  STATE_SETTINGS = 1
};

// Global variables
int currentPage = 0;
int currentSetting = 0;
MenuState currentState = STATE_PAGES;
const int totalPages = 5;
const int totalSettings = 3;

// Timing variables
unsigned long lastDebounce = 0;
unsigned long lastAutoSwipe = 0;
unsigned long lastAnimation = 0;
unsigned long buttonHoldStart = 0;
const unsigned long debounceDelay = 300;
const unsigned long longPressDelay = 1000; // 1 second like OLED_1
const unsigned long autoSwipeDelay = 15000; // 15 seconds like OLED_1

// Animation
bool alertFlashing = false;
bool bothButtonsPressed = false;

// Sensor data
struct SensorData {
  float temperature = 25.6;
  float humidity = 60.2;
  bool fireDetected = false;
  bool motionDetected = false;
  bool doorOpen = false;
  unsigned long uptime = 0;
  int freeHeap = 0;
} sensors;

void showIntroAnimation() {
  // Animation 1: Smart Shop Guard text appears letter by letter
  display.clear();
  display.setFont(ArialMT_Plain_16);
  
  // First show "SMART"
  display.drawString(30, 15, "SMART");
  display.display();
  delay(800);
  
  // Then add "SHOP"
  display.drawString(35, 35, "SHOP");
  display.display();
  delay(800);
  
  // Clear and show "GUARD" with icon
  display.clear();
  display.drawXbm(20, 10, 16, 16, shield_icon);
  display.setFont(ArialMT_Plain_16);
  display.drawString(40, 15, "GUARD");
  display.display();
  delay(1000);
  
  // Animation 2: Loading dots
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(25, 20, "Smart Shop Guard");
  
  for (int i = 0; i < 8; i++) {
    display.fillRect(0, 45, 128, 10); // Clear loading area
    display.setColor(BLACK);
    display.fillRect(0, 45, 128, 10);
    display.setColor(WHITE);
    
    // Draw loading bar
    int barWidth = (i * 128) / 8;
    display.drawRect(10, 45, 108, 8);
    display.fillRect(12, 47, barWidth - 4, 4);
    
    // Loading text
    display.drawString(45, 35, "Loading");
    String dots = "";
    for (int j = 0; j < (i % 4); j++) {
      dots += ".";
    }
    display.drawString(80, 35, dots + "   "); // Extra spaces to clear previous dots
    
    display.display();
    delay(300);
  }
  
  // Animation 3: System Ready
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(25, 15, "SYSTEM");
  display.drawString(30, 35, "READY!");
  display.display();
  delay(1200);
  
  // Animation 4: Final fade effect
  for (int i = 0; i < 3; i++) {
    display.clear();
    display.display();
    delay(200);
    
    display.setFont(ArialMT_Plain_10);
    display.drawString(30, 25, "Initializing...");
    display.display();
    delay(200);
  }
}

void setup() {
  Serial.begin(115200);
  
  // Default config
  config.theme = THEME_NORMAL;
  config.auto_swipe = true;
  
  // Initialize buttons
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_PREV, INPUT_PULLUP);
  
  // Initialize display - SAME AS OLED_1
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10); // Start with small font like OLED_1
  
  // Show intro animation
  showIntroAnimation();
  
  // Connect WiFi - SAME STYLE AS OLED_1
  WiFi.begin(ssid, password);
  
  int dots = 0;
  while (WiFi.status() != WL_CONNECTED) {
    display.clear();
    String dotsStr = "";
    for (int i = 0; i < dots % 4; i++) {
      dotsStr += ".";
    }
    display.drawString(0, 0, "Connecting" + dotsStr);
    display.display();
    dots++;
    delay(400);
  }
  
  // Show connection success
  display.clear();
  display.drawString(0, 0, "WiFi Connected!");
  display.display();
  delay(1000);
  
  // Start with first page
  currentPage = 0;
  updateDisplay();
  
  Serial.println("1.3\" OLED Ready - Small Font Mode!");
}

void loop() {
  handleButtons();
  handleAutoSwipe();
  handleAnimations();
  updateSensorData();
  updateDisplay();
  delay(50); // Same as OLED_1
}

void handleButtons() {
  static bool nextPressed = false;
  static bool prevPressed = false;
  static unsigned long bothButtonsStart = 0;
  
  bool nextState = (digitalRead(BUTTON_NEXT) == LOW);
  bool prevState = (digitalRead(BUTTON_PREV) == LOW);
  
  // Check if both buttons are pressed (toggle auto-swipe) - SAME AS OLED_1
  if (nextState && prevState) {
    if (bothButtonsStart == 0) {
      bothButtonsStart = millis();
    } else if (millis() - bothButtonsStart > 1000) {
      config.auto_swipe = !config.auto_swipe;
      lastDebounce = millis();
      lastAutoSwipe = millis();
      bothButtonsStart = 0;
      Serial.println("Auto-swipe toggled: " + String(config.auto_swipe ? "ON" : "OFF"));
    }
    return;
  } else {
    bothButtonsStart = 0;
  }
  
  // Check for settings access (long press NEXT)
  if (nextState && !nextPressed) {
    buttonHoldStart = millis();
  } else if (nextState && nextPressed && 
             (millis() - buttonHoldStart > longPressDelay) && currentPage == PAGE_SETTINGS) {
    toggleSettings();
    buttonHoldStart = millis() + 2000; // Prevent multiple triggers
  }
  
  if (millis() - lastDebounce > debounceDelay) {
    if (currentState == STATE_PAGES) {
      // Page navigation - SAME AS OLED_1
      if (nextState && !nextPressed) {
        currentPage = (currentPage + 1) % totalPages;
        lastDebounce = millis();
        lastAutoSwipe = millis();
      }
      
      if (prevState && !prevPressed) {
        currentPage = (currentPage - 1 + totalPages) % totalPages;
        lastDebounce = millis();
        lastAutoSwipe = millis();
      }
    } else {
      // Settings navigation
      if (nextState && !nextPressed) {
        currentSetting = (currentSetting + 1) % totalSettings;
        lastDebounce = millis();
      }
      
      if (prevState && !prevPressed) {
        executeSetting();
        lastDebounce = millis();
      }
    }
  }
  
  nextPressed = nextState;
  prevPressed = prevState;
}

void toggleSettings() {
  if (currentState == STATE_SETTINGS) {
    currentState = STATE_PAGES;
    currentPage = PAGE_STATUS;
  } else {
    currentState = STATE_SETTINGS;
    currentSetting = 0;
  }
}

void executeSetting() {
  switch (currentSetting) {
    case 0: // Theme
      config.theme = (config.theme + 1) % 2;
      applyTheme();
      break;
    case 1: // Auto-swipe (also available via both buttons)
      config.auto_swipe = !config.auto_swipe;
      break;
    case 2: // Back
      currentState = STATE_PAGES;
      currentPage = PAGE_STATUS;
      break;
  }
}

void handleAutoSwipe() {
  if (config.auto_swipe && currentState == STATE_PAGES && 
      (millis() - lastAutoSwipe > autoSwipeDelay)) {
    currentPage = (currentPage + 1) % totalPages;
    lastAutoSwipe = millis();
  }
}

void handleAnimations() {
  if (millis() - lastAnimation > 500) {
    if (sensors.fireDetected || sensors.motionDetected) {
      alertFlashing = !alertFlashing;
    }
    lastAnimation = millis();
  }
}

void updateSensorData() {
  sensors.uptime = millis() / 1000;
  sensors.freeHeap = ESP.getFreeHeap();
}

void updateDisplay() {
  display.clear();
  
  if (currentState == STATE_PAGES) {
    drawPageDisplay();
  } else {
    drawSettingsDisplay();
  }
  
  display.display();
}

void drawPageDisplay() {
  // Title bar - EXACTLY LIKE OLED_1
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Shop Guard");
  
  // Page indicator (right aligned) - EXACTLY LIKE OLED_1
  String pageIndicator = String(currentPage + 1) + "/" + String(totalPages);
  if (config.auto_swipe) {
    pageIndicator += " ●";
  }
  display.drawString(128 - (pageIndicator.length() * 6), 0, pageIndicator);
  
  // Separator line - EXACTLY LIKE OLED_1
  display.drawLine(0, 10, 128, 10);
  
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
  // Animated status icon
  if (sensors.fireDetected) {
    if (alertFlashing) {
      display.drawXbm(0, 13, 16, 16, fire_alt1); // Alternate fire icon
    } else {
      display.drawXbm(0, 13, 16, 16, fire_icon);
    }
  } else if (sensors.motionDetected) {
    if (alertFlashing) {
      display.drawXbm(0, 13, 16, 16, warning_icon); // Warning icon when flashing
    } else {
      display.drawXbm(0, 13, 16, 16, alert_icon);
    }
  } else {
    display.drawXbm(0, 13, 16, 16, shield_icon);
  }
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "Status");
  
  // Animated status info - COMPACT LIKE OLED_1
  if (sensors.fireDetected) {
    if (alertFlashing) {
      display.drawString(0, 26, ">>> FIRE! <<<");
      display.drawString(0, 36, ">>> EMERGENCY! <<<");
      display.drawString(0, 46, ">>> EVACUATE! <<<");
    } else {
      display.drawString(0, 26, "FIRE DETECTED!");
      display.drawString(0, 36, "EMERGENCY!");
      display.drawString(0, 46, "EVACUATE NOW!");
    }
  } else if (sensors.motionDetected) {
    if (alertFlashing) {
      display.drawString(0, 26, ">>> MOTION! <<<");
      display.drawString(0, 36, ">>> INTRUDER! <<<");
      display.drawString(0, 46, ">>> ALERT! <<<");
    } else {
      display.drawString(0, 26, "MOTION DETECTED!");
      display.drawString(0, 36, "Location: Door");
      display.drawString(0, 46, "Alert: ACTIVE");
    }
  } else {
    // Normal status with animated heartbeat effect
    display.drawString(0, 26, "System: SECURE");
    display.drawString(0, 36, "Temp: " + String(sensors.temperature, 1) + "C");
    display.drawString(0, 46, "Humid: " + String(sensors.humidity, 0) + "%");
    
    // Add small animated indicator for normal operation
    if (alertFlashing) { // Reuse the flashing variable for heartbeat
      display.fillCircle(120, 55, 2);
    } else {
      display.drawCircle(120, 55, 2);
    }
  }
}

void showSensorPage() {
  // Animated temperature icon
  if (sensors.temperature > 30) {
    display.drawXbm(0, 13, 16, 16, temp_alt1); // Hot temperature icon
  } else {
    display.drawXbm(0, 13, 16, 16, temp_icon);
  }
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "Sensors");
  
  display.drawString(0, 26, "Temp: " + String(sensors.temperature) + "C");
  display.drawString(0, 36, "Humid: " + String(sensors.humidity) + "%");
  display.drawString(0, 46, "Air: Good");
  
  // Add animated sensor activity indicators
  if (alertFlashing) {
    display.drawString(100, 26, "*");
    display.drawString(100, 36, "*");
  } else {
    display.drawString(100, 26, "o");
    display.drawString(100, 36, "o");
  }
}

void showSystemPage() {
  // Animated system icon based on system health
  if (sensors.freeHeap < 10000) {
    display.drawXbm(0, 13, 16, 16, warning_icon); // Low memory warning
  } else {
    display.drawXbm(0, 13, 16, 16, system_icon);
  }
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "System");
  
  display.drawString(0, 26, "Heap: " + String(sensors.freeHeap / 1024) + "KB");
  display.drawString(0, 36, "Up: " + String(sensors.uptime) + "s");
  display.drawString(0, 46, "WiFi: " + String(WiFi.status() == WL_CONNECTED ? "OK" : "NO"));
  
  // Add system activity indicator
  if (alertFlashing) {
    display.fillRect(120, 26, 3, 3);
    display.fillRect(120, 36, 3, 3);
    display.fillRect(120, 46, 3, 3);
  } else {
    display.drawRect(120, 26, 3, 3);
    display.drawRect(120, 36, 3, 3);
    display.drawRect(120, 46, 3, 3);
  }
}

void showAlertsPage() {
  // EXACTLY LIKE OLED_1 showFireDetection() and showSecurityStatus() combined
  if (sensors.fireDetected) {
    display.drawXbm(0, 13, 16, 16, fire_icon);
    display.setFont(ArialMT_Plain_10);
    display.drawString(18, 13, "Fire Alert");
    display.drawString(0, 26, "FIRE DETECTED!");
    display.drawString(0, 36, "Temp: HIGH");
    display.drawString(0, 46, "ALARM: ON");
  } else if (sensors.motionDetected) {
    display.drawXbm(0, 13, 16, 16, alert_icon);
    display.setFont(ArialMT_Plain_10);
    display.drawString(18, 13, "Motion Alert");
    display.drawString(0, 26, "INTRUDER!");
    display.drawString(0, 36, "Location: Door");
    display.drawString(0, 46, "Alert: ACTIVE");
  } else {
    display.drawXbm(0, 13, 16, 16, lock_icon);
    display.setFont(ArialMT_Plain_10);
    display.drawString(18, 13, "Security");
    display.drawString(0, 26, "Secure");
    display.drawString(0, 36, "Door: " + String(sensors.doorOpen ? "Open" : "Closed"));
    display.drawString(0, 46, "Motion: None");
  }
}

void showSettingsPage() {
  display.drawXbm(0, 13, 16, 16, settings_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "Settings");
  
  display.drawString(0, 26, "Hold NEXT to enter");
  display.drawString(0, 36, "Both buttons: Auto");
  display.drawString(0, 46, "Theme: " + getThemeName());
}

void drawSettingsDisplay() {
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Settings Menu");
  display.drawLine(0, 10, 128, 10);
  
  String settings[] = {
    "Theme: " + getThemeName(),
    "Auto: " + String(config.auto_swipe ? "ON" : "OFF"),
    "Back"
  };
  
  for (int i = 0; i < totalSettings; i++) {
    int y = 15 + i * 10;
    if (i == currentSetting) {
      display.drawString(0, y, "> " + settings[i]);
    } else {
      display.drawString(5, y, settings[i]);
    }
  }
  
  display.drawString(0, 55, "PREV=Select NEXT=Move");
}

void applyTheme() {
  switch (config.theme) {
    case THEME_NORMAL:
      display.normalDisplay();
      break;
    case THEME_INVERT:
      display.invertDisplay();
      break;
  }
}

String getThemeName() {
  switch (config.theme) {
    case THEME_NORMAL: return "Normal";
    case THEME_INVERT: return "Invert";
    default: return "?";
  }
}