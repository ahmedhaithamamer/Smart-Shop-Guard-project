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

void displayOLEDTemperatureHumidity(int temperature, int humidity) {
  // Force immediate OLED update with new temperature/humidity values
  // This ensures synchronization with LCD display
  updateOLEDDisplay();
}

void displayOLEDWelcomeMessage() {
  // Welcome message is now handled by showIntro()
  showIntro();
}

void displayOLEDModeStatus() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(15, 15, "Mode: ");
  display.drawString(15, 35, isDay ? "Night" : "Day");
  display.display();
  delay(1000);  // Reduced delay for better sync
  updateOLEDDisplay();  // Return to normal display after mode status
}

void displayOLEDFireAlert() {
  fireDetected = true;
  alertFlashing = true;
  updateOLEDDisplay();  // Force immediate update for sync
}

void displayOLEDSafeStatus() {
  fireDetected = false;
  alertFlashing = false;
  updateOLEDDisplay();  // Force immediate update for sync
}

void displayOLEDThiefAlert() {
  motionDetected = true;
  alertFlashing = true;
  updateOLEDDisplay();  // Force immediate update for sync
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
  display.drawXbm(0, 5, 16, 16, shield_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 5, "Status");
  
  // Status info - COMPACT LIKE OLED_1 (moved up 8 pixels)
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
    
    // Add WiFi status indicator on status page
    display.setFont(ArialMT_Plain_10);
    if (isWiFiConnected()) {
      display.drawString(90, 20, "WiFi:OK");
    } else {
      display.drawString(90, 20, "WiFi:--");
    }
  }
}

void showSensorPage() {
  // EXACTLY LIKE OLED_1 showSensorData()
  display.drawXbm(0, 5, 16, 16, temp_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 5, "Sensors");
  
  // Sensor readings (moved up 8 pixels)
  display.drawString(0, 20, "Temperature: " + String(t) + "C");
  display.drawString(0, 30, "Humidity: " + String(h) + "%");
  display.drawString(0, 40, "AC: " + String(AC ? "ON" : "OFF"));
  
  // Fire sensor status
  display.drawString(0, 48, fireDetected ? "Fire: YES" : "Fire: NO");
}

void showSystemPage() {
  display.drawXbm(0, 5, 16, 16, system_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 5, "System");
  
  // System information with dynamic WiFi status
  display.drawString(0, 20, getWiFiStatus());
  display.drawString(0, 30, "Mode: " + String(isDay ? "Night" : "Day"));
  display.drawString(0, 40, "Uptime: " + String(millis() / 1000) + "s");
  display.drawString(0, 50, "Memory: " + String(ESP.getFreeHeap()/1000) + "KB");
}

void showAlertsPage() {
  display.drawXbm(0, 5, 16, 16, alert_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 5, "Alerts");
  
  // Alert status (moved up 8 pixels)
  if (fireDetected) {
    display.drawString(0, 20, "FIRE ALERT ACTIVE");
    display.drawString(0, 30, "Check fire sensor");
  } else if (motionDetected) {
    display.drawString(0, 20, "MOTION DETECTED");
    display.drawString(0, 30, "Security breach");
  } else {
    display.drawString(0, 20, "All Clear");
    display.drawString(0, 30, "No active alerts");
    display.drawString(0, 40, "System secure");
  }
}

void showSettingsPage() {
  display.drawXbm(0, 5, 16, 16, settings_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 5, "Settings");
  
  // Settings menu (moved up 8 pixels)
  display.drawString(0, 20, "Theme: " + String(oledConfig.theme == THEME_NORMAL ? "Normal" : "Invert"));
  display.drawString(0, 30, "Auto-Swipe: " + String(oledConfig.auto_swipe ? "ON" : "OFF"));
  display.drawString(0, 40, "Pages: " + String(totalPages));
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