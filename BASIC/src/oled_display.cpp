#include "oled_display.h"
#include "sensors.h"
#include "system.h"

// OLED Display object - 1.3" 128x64 display
SH1106Wire display(0x3C, OLED_SDA_PIN, OLED_SCL_PIN);

// Global OLED variables
int currentPage = 0;
int currentSetting = 0;
MenuState currentState = STATE_PAGES;
OLEDConfig oledConfig = {THEME_NORMAL, true};
const int totalPages = 5;
const int totalSettings = 3;

// Animation and status variables
bool alertFlashing = false;
bool fireDetected = false;
bool motionDetected = false;
bool doorOpen = false;
unsigned long lastAnimation = 0;
unsigned long lastAutoSwipe = 0;
unsigned long lastDebounce = 0;
unsigned long buttonHoldStart = 0;

// Timing constants
const unsigned long debounceDelay = 300;
const unsigned long longPressDelay = 1000;
const unsigned long autoSwipeDelay = 15000;

void initOLEDDisplay() {
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  
  // Show intro animation
  showIntro();
  
  // Apply initial theme
  applyTheme();
}

void showIntro() {
  // Animated Smart Shop Guard intro
  
  // Animation 1: Text appears letter by letter
  display.clear();
  display.setFont(ArialMT_Plain_16);
  
  // Show "Smart Shop" letter by letter
  String line1 = "Smart Shop";
  for (int i = 0; i <= line1.length(); i++) {
    display.clear();
    display.drawString(15, 15, line1.substring(0, i));
    display.display();
    delay(150);
  }
  
  delay(300);
  
  // Show "Guard" letter by letter with shield icon
  String line2 = "Guard";
  for (int i = 0; i <= line2.length(); i++) {
    display.clear();
    display.drawString(15, 15, line1); // Keep first line
    display.drawString(35, 30, line2.substring(0, i));
    
    // Add shield icon next to Guard when text is complete
    if (i == line2.length()) {
      display.drawXbm(80, 32, 16, 16, shield_icon);
    }
    
    display.display();
    delay(150);
  }
  
  delay(500);
  
  // Animation 2: Blinking effect with shield icon
  for (int i = 0; i < 3; i++) {
    display.clear();
    display.display();
    delay(200);
    
    display.clear();
    display.drawString(15, 15, "Smart Shop");
    display.drawString(35, 30, "Guard");
    display.drawXbm(80, 32, 16, 16, shield_icon); // Shield next to Guard
    display.display();
    delay(300);
  }
  
  // Animation 3: Border animation with shield icon
  for (int i = 0; i < 64; i += 8) {
    display.clear();
    display.drawString(15, 15, "Smart Shop");
    display.drawString(35, 30, "Guard");
    display.drawXbm(80, 32, 16, 16, shield_icon); // Shield next to Guard
    
    // Draw animated border (adjusted for centered content)
    display.drawRect(5, 5, 118, 50);
    display.drawRect(4, 4, 120, 52);
    
    // Moving corner dots
    int x = (i * 2) % 128;
    display.fillCircle(x, 5, 1);
    display.fillCircle(128 - x, 58, 1);
    
    display.display();
    delay(100);
  }
  
  // Final display with shield icon
  display.clear();
  display.drawString(15, 15, "Smart Shop");
  display.drawString(35, 30, "Guard");
  display.drawXbm(80, 32, 16, 16, shield_icon); // Shield next to Guard (centered)
  display.display();
  delay(800);
}

void clearOLEDDisplay() {
  display.clear();
}

void displayTemperatureHumidity(int temperature, int humidity) {
  // This function is called from main.cpp, but we'll handle display in updateOLEDDisplay()
  // Just update the global variables
  // t and h are already global variables in main.cpp
}

void displayWelcomeMessage() {
  // Welcome message is now handled by showIntro()
  showIntro();
}

void displayModeStatus() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(15, 15, "Mode: ");
  display.drawString(15, 35, isDay ? "Day" : "Night");
  display.display();
  delay(2000);
}

void displayFireAlert() {
  fireDetected = true;
  alertFlashing = true;
}

void displaySafeStatus() {
  fireDetected = false;
  alertFlashing = false;
}

void displayThiefAlert() {
  motionDetected = true;
  alertFlashing = true;
}

void updateOLEDDisplay() {
  display.clear();
  showCurrentPage();
  display.display();
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
  // Draw shield icon for status
  display.drawXbm(0, 13, 16, 16, shield_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "Status");
  
  // Status info - COMPACT LIKE OLED_1
  if (fireDetected && alertFlashing) {
    display.drawString(0, 26, "FIRE DETECTED!");
    display.drawString(0, 36, "EMERGENCY!");
    display.drawString(0, 46, "EVACUATE NOW!");
  } else if (motionDetected && alertFlashing) {
    display.drawString(0, 26, "MOTION DETECTED!");
    display.drawString(0, 36, "Location: Door");
    display.drawString(0, 46, "Alert: ACTIVE");
  } else {
    display.drawString(0, 26, "System: SECURE");
    display.drawString(0, 36, "Temp: " + String(t, 1) + "C");
    display.drawString(0, 46, "Humid: " + String(h, 0) + "%");
  }
}

void showSensorPage() {
  // EXACTLY LIKE OLED_1 showSensorData()
  display.drawXbm(0, 13, 16, 16, temp_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "Sensors");
  
  // Sensor readings
  display.drawString(0, 26, "Temperature: " + String(t) + "C");
  display.drawString(0, 36, "Humidity: " + String(h) + "%");
  display.drawString(0, 46, "AC: " + String(AC ? "ON" : "OFF"));
  
  // Fire sensor status
  display.drawString(0, 56, fireDetected ? "Fire: YES" : "Fire: NO");
}

void showSystemPage() {
  display.drawXbm(0, 13, 16, 16, system_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "System");
  
  // System information
  display.drawString(0, 26, "WiFi: Connected");
  display.drawString(0, 36, "Mode: " + String(isDay ? "Day" : "Night"));
  display.drawString(0, 46, "Uptime: " + String(millis() / 1000) + "s");
  display.drawString(0, 56, "Memory: " + String(ESP.getFreeHeap()) + "B");
}

void showAlertsPage() {
  display.drawXbm(0, 13, 16, 16, alert_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "Alerts");
  
  // Alert status
  if (fireDetected) {
    display.drawString(0, 26, "FIRE ALERT ACTIVE");
    display.drawString(0, 36, "Check fire sensor");
  } else if (motionDetected) {
    display.drawString(0, 26, "MOTION DETECTED");
    display.drawString(0, 36, "Security breach");
  } else {
    display.drawString(0, 26, "All Clear");
    display.drawString(0, 36, "No active alerts");
    display.drawString(0, 46, "System secure");
  }
}

void showSettingsPage() {
  display.drawXbm(0, 13, 16, 16, settings_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "Settings");
  
  // Settings menu
  display.drawString(0, 26, "Theme: " + String(oledConfig.theme == THEME_NORMAL ? "Normal" : "Invert"));
  display.drawString(0, 36, "Auto-Swipe: " + String(oledConfig.auto_swipe ? "ON" : "OFF"));
  display.drawString(0, 46, "Pages: " + String(totalPages));
}

void handleOLEDButtons() {
  // Button handling logic would go here
  // This would be called from main.cpp loop
  // For now, we'll use auto-swipe functionality
  
  if (oledConfig.auto_swipe && (millis() - lastAutoSwipe > autoSwipeDelay)) {
    currentPage = (currentPage + 1) % totalPages;
    lastAutoSwipe = millis();
  }
}

void applyTheme() {
  if (oledConfig.theme == THEME_INVERT) {
    display.invertDisplay();
  } else {
    display.normalDisplay();
  }
}

void drawProgressBar(int x, int y, int width, int height, int percentage) {
  // Draw progress bar border
  display.drawRect(x, y, width, height);
  
  // Fill progress
  int fillWidth = (width - 2) * percentage / 100;
  if (fillWidth > 0) {
    display.fillRect(x + 1, y + 1, fillWidth, height - 2);
  }
}

void animateAlert() {
  if (alertFlashing) {
    unsigned long currentTime = millis();
    if (currentTime - lastAnimation > 500) { // Flash every 500ms
      alertFlashing = !alertFlashing;
      lastAnimation = currentTime;
    }
  }
}