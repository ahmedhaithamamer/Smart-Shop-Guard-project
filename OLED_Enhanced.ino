#include <Wire.h>
#include <SH1106Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <EEPROM.h>
#include "icons.h"

// Button definitions
#define BUTTON_NEXT D5
#define BUTTON_PREV D6
#define BUTTON_SELECT D7  // New select button for menu interaction

// Display setup
SH1106Wire display(0x3C, D2, D1); // SDA = D2, SCL = D1

// Configuration structure
struct Config {
  char wifi_ssid[32];
  char wifi_password[64];
  int theme;
  int brightness;
  bool auto_swipe;
  int temp_threshold;
  int humid_threshold;
  unsigned long auto_swipe_delay;
} config;

// Display themes
enum DisplayTheme {
  THEME_DEFAULT = 0,
  THEME_DARK = 1,
  THEME_HIGH_CONTRAST = 2,
  THEME_MINIMAL = 3
};

// Page types
enum PageType {
  PAGE_MAIN = 0,
  PAGE_WIFI = 1,
  PAGE_SENSORS = 2,
  PAGE_SYSTEM = 3,
  PAGE_FIRE = 4,
  PAGE_SECURITY = 5,
  PAGE_SETTINGS = 6,
  PAGE_DIAGNOSTICS = 7,
  PAGE_WIFI_SETUP = 8,
  PAGE_CHARTS = 9
};

// Menu states
enum MenuState {
  STATE_NORMAL = 0,
  STATE_SETTINGS_MENU = 1,
  STATE_WIFI_SETUP = 2,
  STATE_THEME_SELECT = 3
};

// Global variables
int currentPage = 0;
int currentMenuItem = 0;
MenuState currentState = STATE_NORMAL;
const int totalPages = 10;
const int totalMenuItems = 6;

// Timing variables
unsigned long lastDebounce = 0;
unsigned long lastAutoSwipe = 0;
unsigned long lastAnimation = 0;
unsigned long transitionStart = 0;
const unsigned long debounceDelay = 200;

// Animation variables
int animationFrame = 0;
bool alertFlashing = false;
bool transitionActive = false;
int transitionDirection = 1; // 1 for forward, -1 for backward

// Sensor data (will be replaced with real sensors)
struct SensorData {
  float temperature = 25.6;
  float humidity = 60.2;
  int airQuality = 85;
  bool fireDetected = false;
  bool motionDetected = false;
  bool doorOpen = false;
  int wifiSignal = -45;
  unsigned long uptime = 0;
  int freeHeap = 0;
} sensors;

// Chart data arrays
float tempHistory[64];
float humidHistory[64];
int chartIndex = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize EEPROM
  EEPROM.begin(512);
  loadConfig();
  
  // Initialize buttons
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_PREV, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  
  // Initialize display
  display.init();
  display.flipScreenVertically();
  applyTheme();
  
  // Show startup animation
  showStartupAnimation();
  
  // Initialize WiFi
  initWiFi();
  
  // Initialize chart data
  initChartData();
  
  Serial.println("Smart Shop Guard OLED Enhanced - Ready!");
}

void loop() {
  handleButtons();
  handleAutoSwipe();
  handleAnimations();
  updateSensorData();
  updateDisplay();
  delay(50);
}

void loadConfig() {
  EEPROM.get(0, config);
  
  // Set defaults if EEPROM is empty
  if (config.theme < 0 || config.theme > 3) {
    strcpy(config.wifi_ssid, "");
    strcpy(config.wifi_password, "");
    config.theme = THEME_DEFAULT;
    config.brightness = 255;
    config.auto_swipe = true;
    config.temp_threshold = 30;
    config.humid_threshold = 70;
    config.auto_swipe_delay = 15000;
    saveConfig();
  }
}

void saveConfig() {
  EEPROM.put(0, config);
  EEPROM.commit();
}

void initWiFi() {
  if (strlen(config.wifi_ssid) > 0) {
    WiFi.begin(config.wifi_ssid, config.wifi_password);
    
    // Show connection status
    int dots = 0;
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      display.clear();
      drawConnectingAnimation(dots % 4);
      display.display();
      dots++;
      attempts++;
      delay(500);
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    showConnectionSuccess();
  } else {
    // Start WiFi setup mode
    currentState = STATE_WIFI_SETUP;
    currentPage = PAGE_WIFI_SETUP;
  }
}

void handleButtons() {
  static bool nextPressed = false;
  static bool prevPressed = false;
  static bool selectPressed = false;
  static unsigned long selectHoldStart = 0;
  
  bool nextState = (digitalRead(BUTTON_NEXT) == LOW);
  bool prevState = (digitalRead(BUTTON_PREV) == LOW);
  bool selectState = (digitalRead(BUTTON_SELECT) == LOW);
  
  // Handle long press on select button (enter/exit settings)
  if (selectState && !selectPressed) {
    selectHoldStart = millis();
  } else if (selectState && selectPressed && (millis() - selectHoldStart > 1000)) {
    toggleSettingsMenu();
    selectHoldStart = millis() + 2000; // Prevent multiple triggers
  }
  
  if (millis() - lastDebounce > debounceDelay) {
    // Handle navigation based on current state
    switch (currentState) {
      case STATE_NORMAL:
        handleNormalNavigation(nextState, prevState, selectState, nextPressed, prevPressed, selectPressed);
        break;
      case STATE_SETTINGS_MENU:
        handleSettingsNavigation(nextState, prevState, selectState, nextPressed, prevPressed, selectPressed);
        break;
      case STATE_WIFI_SETUP:
        handleWiFiSetupNavigation(nextState, prevState, selectState, nextPressed, prevPressed, selectPressed);
        break;
      case STATE_THEME_SELECT:
        handleThemeNavigation(nextState, prevState, selectState, nextPressed, prevPressed, selectPressed);
        break;
    }
  }
  
  nextPressed = nextState;
  prevPressed = prevState;
  selectPressed = selectState;
}

void handleNormalNavigation(bool next, bool prev, bool select, bool &nextPressed, bool &prevPressed, bool &selectPressed) {
  if (next && !nextPressed) {
    startPageTransition(1);
    currentPage = (currentPage + 1) % totalPages;
    lastDebounce = millis();
    lastAutoSwipe = millis();
  }
  
  if (prev && !prevPressed) {
    startPageTransition(-1);
    currentPage = (currentPage - 1 + totalPages) % totalPages;
    lastDebounce = millis();
    lastAutoSwipe = millis();
  }
  
  if (select && !selectPressed && currentPage == PAGE_SETTINGS) {
    currentState = STATE_SETTINGS_MENU;
    currentMenuItem = 0;
    lastDebounce = millis();
  }
}

void handleSettingsNavigation(bool next, bool prev, bool select, bool &nextPressed, bool &prevPressed, bool &selectPressed) {
  if (next && !nextPressed) {
    currentMenuItem = (currentMenuItem + 1) % totalMenuItems;
    lastDebounce = millis();
  }
  
  if (prev && !prevPressed) {
    currentMenuItem = (currentMenuItem - 1 + totalMenuItems) % totalMenuItems;
    lastDebounce = millis();
  }
  
  if (select && !selectPressed) {
    executeMenuItem();
    lastDebounce = millis();
  }
}

void handleWiFiSetupNavigation(bool next, bool prev, bool select, bool &nextPressed, bool &prevPressed, bool &selectPressed) {
  if (select && !selectPressed) {
    startWiFiSetup();
    lastDebounce = millis();
  }
}

void handleThemeNavigation(bool next, bool prev, bool select, bool &nextPressed, bool &prevPressed, bool &selectPressed) {
  if (next && !nextPressed) {
    config.theme = (config.theme + 1) % 4;
    applyTheme();
    lastDebounce = millis();
  }
  
  if (prev && !prevPressed) {
    config.theme = (config.theme - 1 + 4) % 4;
    applyTheme();
    lastDebounce = millis();
  }
  
  if (select && !selectPressed) {
    saveConfig();
    currentState = STATE_SETTINGS_MENU;
    lastDebounce = millis();
  }
}

void toggleSettingsMenu() {
  if (currentState == STATE_SETTINGS_MENU) {
    currentState = STATE_NORMAL;
    currentPage = PAGE_MAIN;
  } else {
    currentState = STATE_SETTINGS_MENU;
    currentMenuItem = 0;
  }
}

void executeMenuItem() {
  switch (currentMenuItem) {
    case 0: // WiFi Setup
      currentState = STATE_WIFI_SETUP;
      currentPage = PAGE_WIFI_SETUP;
      break;
    case 1: // Theme
      currentState = STATE_THEME_SELECT;
      break;
    case 2: // Auto-swipe toggle
      config.auto_swipe = !config.auto_swipe;
      saveConfig();
      break;
    case 3: // Temperature threshold
      config.temp_threshold = (config.temp_threshold == 30) ? 25 : 30;
      saveConfig();
      break;
    case 4: // Diagnostics
      currentState = STATE_NORMAL;
      currentPage = PAGE_DIAGNOSTICS;
      break;
    case 5: // Back
      currentState = STATE_NORMAL;
      currentPage = PAGE_MAIN;
      break;
  }
}

void handleAutoSwipe() {
  if (config.auto_swipe && currentState == STATE_NORMAL && 
      (millis() - lastAutoSwipe > config.auto_swipe_delay)) {
    startPageTransition(1);
    currentPage = (currentPage + 1) % totalPages;
    lastAutoSwipe = millis();
  }
}

void handleAnimations() {
  if (millis() - lastAnimation > 250) {
    animationFrame = (animationFrame + 1) % 4;
    
    // Handle alert flashing
    if (sensors.fireDetected || sensors.motionDetected) {
      alertFlashing = !alertFlashing;
    }
    
    lastAnimation = millis();
  }
}

void updateSensorData() {
  // Simulate sensor data - replace with real sensor readings
  sensors.uptime = millis() / 1000;
  sensors.freeHeap = ESP.getFreeHeap();
  sensors.wifiSignal = WiFi.RSSI();
  
  // Update chart data
  if (millis() % 5000 == 0) { // Every 5 seconds
    tempHistory[chartIndex] = sensors.temperature;
    humidHistory[chartIndex] = sensors.humidity;
    chartIndex = (chartIndex + 1) % 64;
  }
}

void updateDisplay() {
  if (transitionActive) {
    drawPageTransition();
    return;
  }
  
  display.clear();
  
  switch (currentState) {
    case STATE_NORMAL:
      drawNormalPage();
      break;
    case STATE_SETTINGS_MENU:
      drawSettingsMenu();
      break;
    case STATE_WIFI_SETUP:
      drawWiFiSetup();
      break;
    case STATE_THEME_SELECT:
      drawThemeSelect();
      break;
  }
  
  display.display();
}

void drawNormalPage() {
  drawTitleBar();
  
  switch (currentPage) {
    case PAGE_MAIN:
      drawMainPage();
      break;
    case PAGE_WIFI:
      drawWiFiStatus();
      break;
    case PAGE_SENSORS:
      drawSensorData();
      break;
    case PAGE_SYSTEM:
      drawSystemInfo();
      break;
    case PAGE_FIRE:
      drawFireDetection();
      break;
    case PAGE_SECURITY:
      drawSecurityStatus();
      break;
    case PAGE_SETTINGS:
      drawSettingsPage();
      break;
    case PAGE_DIAGNOSTICS:
      drawDiagnostics();
      break;
    case PAGE_CHARTS:
      drawCharts();
      break;
  }
}

void drawTitleBar() {
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Smart Shop Guard");
  
  // Page indicator
  String pageIndicator = String(currentPage + 1) + "/" + String(totalPages);
  if (config.auto_swipe) {
    pageIndicator += " ●";
  }
  display.drawString(128 - (pageIndicator.length() * 6), 0, pageIndicator);
  
  // WiFi signal indicator
  if (WiFi.status() == WL_CONNECTED) {
    int signal = WiFi.RSSI();
    int bars = map(signal, -100, -30, 0, 4);
    for (int i = 0; i < bars; i++) {
      display.fillRect(90 + i * 3, 8 - i * 2, 2, i * 2 + 2);
    }
  }
  
  display.drawLine(0, 12, 128, 12);
}

void drawMainPage() {
  display.setFont(ArialMT_Plain_16);
  display.drawString(32, 20, "SHOP GUARD");
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(32, 40, "Status: Active");
  
  // Status indicators
  if (sensors.fireDetected && alertFlashing) {
    display.drawXbm(0, 16, 16, 16, fire_icon);
    display.drawString(20, 20, "FIRE!");
  } else if (sensors.motionDetected && alertFlashing) {
    display.drawXbm(0, 16, 16, 16, alert_icon);
    display.drawString(20, 20, "MOTION!");
  } else {
    display.drawXbm(0, 16, 16, 16, shield_icon);
  }
  
  display.drawString(0, 50, "Temp: " + String(sensors.temperature, 1) + "°C");
  display.drawString(64, 50, "Humid: " + String(sensors.humidity, 0) + "%");
}

void drawWiFiStatus() {
  display.drawXbm(0, 15, 16, 16, wifi_icon);
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 15, "WiFi Status");
  
  if (WiFi.status() == WL_CONNECTED) {
    display.drawString(0, 28, "Connected");
    display.drawString(0, 38, WiFi.localIP().toString());
    display.drawString(0, 48, "RSSI: " + String(WiFi.RSSI()) + " dBm");
  } else {
    display.drawString(0, 28, "Disconnected");
    display.drawString(0, 38, "Press SELECT for setup");
  }
}

void drawSensorData() {
  display.drawXbm(0, 15, 16, 16, temp_icon);
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 15, "Sensors");
  
  // Temperature with progress bar
  display.drawString(0, 28, "Temp: " + String(sensors.temperature, 1) + "°C");
  drawProgressBar(0, 38, 80, 6, sensors.temperature, 0, 50);
  
  // Humidity with progress bar
  display.drawString(0, 46, "Humid: " + String(sensors.humidity, 0) + "%");
  drawProgressBar(0, 56, 80, 6, sensors.humidity, 0, 100);
  
  // Air quality indicator
  display.drawString(85, 28, "Air:");
  display.drawString(85, 38, String(sensors.airQuality) + "%");
  
  if (sensors.airQuality > 80) {
    display.drawString(85, 48, "Good");
  } else if (sensors.airQuality > 60) {
    display.drawString(85, 48, "Fair");
  } else {
    display.drawString(85, 48, "Poor");
  }
}

void drawSystemInfo() {
  display.drawXbm(0, 15, 16, 16, system_icon);
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 15, "System Info");
  
  display.drawString(0, 28, "Uptime: " + formatTime(sensors.uptime));
  display.drawString(0, 38, "Heap: " + String(sensors.freeHeap / 1024) + "KB");
  display.drawString(0, 48, "Flash: " + String(ESP.getFlashChipSize() / 1024) + "KB");
  display.drawString(0, 58, "Theme: " + getThemeName());
}

void drawFireDetection() {
  if (sensors.fireDetected && alertFlashing) {
    display.drawXbm(0, 15, 16, 16, fire_alt1);
  } else {
    display.drawXbm(0, 15, 16, 16, fire_icon);
  }
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 15, "Fire Detection");
  
  if (sensors.fireDetected) {
    if (alertFlashing) {
      display.setFont(ArialMT_Plain_16);
      display.drawString(0, 30, "FIRE DETECTED!");
      display.setFont(ArialMT_Plain_10);
    }
    display.drawString(0, 48, "Temp: HIGH");
    display.drawString(0, 58, "Alarm: ACTIVE");
  } else {
    display.drawString(0, 28, "Status: Safe");
    display.drawString(0, 38, "Temp: Normal");
    display.drawString(0, 48, "Smoke: Low");
    display.drawString(0, 58, "Sprinkler: Ready");
  }
}

void drawSecurityStatus() {
  display.drawXbm(0, 15, 16, 16, lock_icon);
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 15, "Security");
  
  if (sensors.motionDetected && alertFlashing) {
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 30, "MOTION DETECTED!");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 45, "Location: Main Door");
    display.drawString(0, 55, "Alert: ACTIVE");
  } else {
    display.drawString(0, 28, "Status: Secure");
    display.drawString(0, 38, "Door: " + String(sensors.doorOpen ? "Open" : "Closed"));
    display.drawString(0, 48, "Motion: None");
    display.drawString(0, 58, "Cameras: Active");
  }
}

void drawSettingsPage() {
  display.drawXbm(0, 15, 16, 16, settings_icon);
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 15, "Settings");
  
  display.drawString(0, 30, "Press SELECT to enter");
  display.drawString(0, 40, "Hold SELECT to exit");
  display.drawString(0, 55, "Configure system here");
}

void drawDiagnostics() {
  display.drawXbm(0, 15, 16, 16, diagnostic_icon);
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 15, "Diagnostics");
  
  display.drawString(0, 28, "WiFi: " + String(WiFi.status() == WL_CONNECTED ? "OK" : "FAIL"));
  display.drawString(0, 38, "Sensors: OK");
  display.drawString(0, 48, "Display: OK");
  display.drawString(0, 58, "Memory: " + String(sensors.freeHeap > 10000 ? "OK" : "LOW"));
}

void drawCharts() {
  display.drawXbm(0, 15, 16, 16, chart_icon);
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 15, "Data Charts");
  
  // Temperature chart
  display.drawString(0, 28, "Temperature Trend:");
  drawMiniChart(0, 35, 60, 15, tempHistory, 64, 20, 35);
  
  // Humidity chart  
  display.drawString(0, 52, "Humidity Trend:");
  drawMiniChart(68, 35, 60, 15, humidHistory, 64, 30, 80);
}

void drawSettingsMenu() {
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Settings Menu");
  display.drawLine(0, 12, 128, 12);
  
  String menuItems[] = {
    "WiFi Setup",
    "Theme",
    "Auto-swipe: " + String(config.auto_swipe ? "ON" : "OFF"),
    "Temp Alert: " + String(config.temp_threshold) + "°C",
    "Diagnostics",
    "Back"
  };
  
  for (int i = 0; i < totalMenuItems; i++) {
    int y = 16 + i * 8;
    if (i == currentMenuItem) {
      display.fillRect(0, y - 1, 128, 9);
      display.setColor(BLACK);
      display.drawString(2, y, "> " + menuItems[i]);
      display.setColor(WHITE);
    } else {
      display.drawString(2, y, "  " + menuItems[i]);
    }
  }
}

void drawWiFiSetup() {
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "WiFi Setup");
  display.drawLine(0, 12, 128, 12);
  
  display.drawString(0, 20, "Connect to:");
  display.drawString(0, 30, "ShopGuard_Setup");
  display.drawString(0, 45, "Then visit:");
  display.drawString(0, 55, "192.168.4.1");
  
  display.drawString(0, 16, "Press SELECT to start");
}

void drawThemeSelect() {
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Select Theme");
  display.drawLine(0, 12, 128, 12);
  
  String themes[] = {"Default", "Dark", "High Contrast", "Minimal"};
  
  for (int i = 0; i < 4; i++) {
    int y = 20 + i * 10;
    if (i == config.theme) {
      display.drawString(0, y, "> " + themes[i] + " *");
    } else {
      display.drawString(0, y, "  " + themes[i]);
    }
  }
  
  display.drawString(0, 58, "SELECT to save");
}

void drawProgressBar(int x, int y, int width, int height, float value, float min, float max) {
  // Draw border
  display.drawRect(x, y, width, height);
  
  // Calculate fill width
  int fillWidth = map(value, min, max, 0, width - 2);
  fillWidth = constrain(fillWidth, 0, width - 2);
  
  // Draw fill
  if (fillWidth > 0) {
    display.fillRect(x + 1, y + 1, fillWidth, height - 2);
  }
}

void drawMiniChart(int x, int y, int width, int height, float* data, int dataSize, float minVal, float maxVal) {
  // Draw chart border
  display.drawRect(x, y, width, height);
  
  // Draw data points
  for (int i = 1; i < width - 2 && i < dataSize; i++) {
    int dataIndex = (chartIndex - width + 2 + i + dataSize) % dataSize;
    int prevIndex = (chartIndex - width + 1 + i + dataSize) % dataSize;
    
    int y1 = y + height - 2 - map(data[prevIndex], minVal, maxVal, 0, height - 3);
    int y2 = y + height - 2 - map(data[dataIndex], minVal, maxVal, 0, height - 3);
    
    display.drawLine(x + i - 1, y1, x + i, y2);
  }
}

void startPageTransition(int direction) {
  transitionActive = true;
  transitionDirection = direction;
  transitionStart = millis();
}

void drawPageTransition() {
  unsigned long elapsed = millis() - transitionStart;
  const unsigned long transitionDuration = 300;
  
  if (elapsed >= transitionDuration) {
    transitionActive = false;
    return;
  }
  
  // Calculate transition offset
  int offset = map(elapsed, 0, transitionDuration, 0, 128);
  if (transitionDirection < 0) offset = -offset;
  
  // Draw current page shifted
  display.clear();
  // This would require more complex implementation to properly shift content
  // For now, just show a simple slide effect
  
  display.fillRect(offset, 0, 128, 64);
  display.display();
}

void showStartupAnimation() {
  for (int i = 0; i < 3; i++) {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(16, 20, "SHOP GUARD");
    
    // Animated dots
    String dots = "";
    for (int j = 0; j <= i; j++) {
      dots += ".";
    }
    display.drawString(48, 40, dots);
    
    display.display();
    delay(500);
  }
}

void showConnectionSuccess() {
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "WiFi Connected!");
  display.drawString(0, 35, WiFi.localIP().toString());
  display.display();
  delay(2000);
}

void drawConnectingAnimation(int frame) {
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "Connecting to WiFi");
  
  String dots = "";
  for (int i = 0; i < frame; i++) {
    dots += ".";
  }
  display.drawString(0, 35, dots);
}

void startWiFiSetup() {
  WiFiManager wifiManager;
  wifiManager.startConfigPortal("ShopGuard_Setup");
  
  if (WiFi.status() == WL_CONNECTED) {
    strcpy(config.wifi_ssid, WiFi.SSID().c_str());
    strcpy(config.wifi_password, WiFi.psk().c_str());
    saveConfig();
    showConnectionSuccess();
    currentState = STATE_NORMAL;
    currentPage = PAGE_WIFI;
  }
}

void applyTheme() {
  switch (config.theme) {
    case THEME_DEFAULT:
      // Standard theme - no changes needed
      break;
    case THEME_DARK:
      // Invert display for dark theme
      display.invertDisplay();
      break;
    case THEME_HIGH_CONTRAST:
      // High contrast - could adjust font sizes
      break;
    case THEME_MINIMAL:
      // Minimal theme - reduce decorative elements
      break;
  }
}

String getThemeName() {
  switch (config.theme) {
    case THEME_DEFAULT: return "Default";
    case THEME_DARK: return "Dark";
    case THEME_HIGH_CONTRAST: return "High Contrast";
    case THEME_MINIMAL: return "Minimal";
    default: return "Unknown";
  }
}

String formatTime(unsigned long seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;
  
  return String(hours) + ":" + 
         (minutes < 10 ? "0" : "") + String(minutes) + ":" +
         (secs < 10 ? "0" : "") + String(secs);
}

void initChartData() {
  // Initialize chart arrays with some sample data
  for (int i = 0; i < 64; i++) {
    tempHistory[i] = 25.0 + random(-50, 50) / 10.0;
    humidHistory[i] = 60.0 + random(-200, 200) / 10.0;
  }
}