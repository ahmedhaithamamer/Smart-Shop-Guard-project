#include "oled_display.h"
#include "sensors.h"
#include "system.h"
#include "esp_task_wdt.h"

// OLED Display object - 1.3" 128x64 display
SH1106Wire display(0x3C, OLED_SDA_PIN, OLED_SCL_PIN);

// Global OLED variables
int currentPage = 0;
int currentSetting = 0;
MenuState currentState = STATE_PAGES;
OLEDConfig oledConfig = {true}; // auto_swipe only
const int totalPages = 5;
const int totalSettings = 2; // Auto-swipe and Pages info

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
  
  // Initialize button pins
  initOLEDButtons();
  
  // Show intro animation
  showIntro();
  
  // Display is always in normal mode
}

void initOLEDButtons() {
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_PREV, INPUT_PULLUP);
  
  // Small delay to stabilize button states
  delay(100);
  
  Serial.println("OLED buttons initialized - NEXT: " + String(BUTTON_NEXT) + ", PREV: " + String(BUTTON_PREV));
}

void showIntro() {
  // Animated Smart Shop Guard intro - optimized for watchdog safety
  
  // Animation 1: Text appears letter by letter
  display.clear();
  display.setFont(ArialMT_Plain_16);
  
  // Show "Smart Shop" letter by letter
  String line1 = "Smart Shop";
  for (int i = 0; i <= line1.length(); i++) {
    display.clear();
    display.drawString(15, 15, line1.substring(0, i));
    display.display();
    delay(100); // Reduced from 150ms
    esp_task_wdt_reset(); // Reset watchdog during animation
  }
  
  delay(200); // Reduced from 300ms
  esp_task_wdt_reset();
  
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
    delay(100); // Reduced from 150ms
    esp_task_wdt_reset(); // Reset watchdog during animation
  }
  
  delay(300); // Reduced from 500ms
  esp_task_wdt_reset();
  
  // Animation 2: Blinking effect with shield icon (reduced iterations)
  for (int i = 0; i < 2; i++) { // Reduced from 3 to 2 iterations
    display.clear();
    display.display();
    delay(150); // Reduced from 200ms
    esp_task_wdt_reset();
    
    display.clear();
    display.drawString(15, 15, "Smart Shop");
    display.drawString(35, 30, "Guard");
    display.drawXbm(80, 32, 16, 16, shield_icon);
    display.display();
    delay(200); // Reduced from 300ms
    esp_task_wdt_reset();
  }
  
  // Animation 3: Border animation with shield icon (simplified)
  for (int i = 0; i < 32; i += 16) { // Reduced iterations and increased step
    display.clear();
    display.drawString(15, 15, "Smart Shop");
    display.drawString(35, 30, "Guard");
    display.drawXbm(80, 32, 16, 16, shield_icon);
    
    // Draw animated border (simplified)
    display.drawRect(5, 5, 118, 50);
    display.drawRect(4, 4, 120, 52);
    
    // Moving corner dots (simplified)
    int x = (i * 4) % 128;
    display.fillCircle(x, 5, 1);
    display.fillCircle(128 - x, 58, 1);
    
    display.display();
    delay(80); // Reduced from 100ms
    esp_task_wdt_reset(); // Reset watchdog during animation
  }
  
  // Final display with shield icon
  display.clear();
  display.drawString(15, 15, "Smart Shop");
  display.drawString(35, 30, "Guard");
  display.drawXbm(80, 32, 16, 16, shield_icon);
  display.display();
  delay(400); // Reduced from 800ms
  esp_task_wdt_reset();
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
  esp_task_wdt_reset(); // Reset watchdog after delay
  updateOLEDDisplay();  // Return to normal display after mode status
}

void displayOLEDFireAlert() {
  fireDetected = true;
  alertFlashing = true;
  
  // Force OLED to show hazard page and stop cycling
  currentPage = PAGE_ALERTS;  // Switch to alerts page
  lastAutoSwipe = millis();   // Reset auto-swipe timer
  
  // Update OLED immediately to show fire alert
  updateOLEDDisplay();
  
  Serial.println("OLED: Fire alert displayed - page cycling paused");
}

void displayOLEDSafeStatus() {
  fireDetected = false;
  alertFlashing = false;
  
  // Resume normal operation
  updateOLEDDisplay();
  
  Serial.println("OLED: Safe status - resuming normal operation");
}

void displayOLEDThiefAlert() {
  motionDetected = true;
  alertFlashing = true;
  
  // Force OLED to show hazard page and stop cycling
  currentPage = PAGE_ALERTS;  // Switch to alerts page
  lastAutoSwipe = millis();   // Reset auto-swipe timer
  
  // Update OLED immediately to show thief alert
  updateOLEDDisplay();
  
  Serial.println("OLED: Thief alert displayed - page cycling paused");
}

void updateOLEDDisplay() {
  display.clear();
  showCurrentPage();
  display.display();
}
void handleButtons() {
  static bool nextPressed = false;
  static bool prevPressed = false;
  static unsigned long bothButtonsStart = 0;
  static unsigned long lastButtonPress = 0;
  
  // Read button states with safety checks
  bool nextState = false;
  bool prevState = false;
  
  // Safe button reading with error checking
  int nextRead = digitalRead(BUTTON_NEXT);
  int prevRead = digitalRead(BUTTON_PREV);
  
  // Check for valid button readings
  if (nextRead == LOW || nextRead == HIGH) {
    nextState = (nextRead == LOW);
  } else {
    Serial.println("Error reading NEXT button");
    return;
  }
  
  if (prevRead == LOW || prevRead == HIGH) {
    prevState = (prevRead == LOW);
  } else {
    Serial.println("Error reading PREV button");
    return;
  }
  
  // Prevent rapid button presses
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
      
      // Visual feedback - flash the display
      display.invertDisplay();
      delay(100);
      esp_task_wdt_reset();
      display.normalDisplay();
      delay(100);
      esp_task_wdt_reset();
      display.invertDisplay();
      delay(100);
      esp_task_wdt_reset();
      display.normalDisplay();
      // Display restored to normal mode
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
    lastButtonPress = millis();
  }
  
  if (millis() - lastDebounce > debounceDelay) {
    if (currentState == STATE_PAGES) {
      // Page navigation
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
      // Settings navigation
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
  if(isWiFiConnected()){
    display.drawXbm(120, 5, 8 , 8, wifi_icon_connected);
  }
  else{
    display.setFont(ArialMT_Plain_10);
    display.drawString(100, 5, "Not Connected"); 
  }
  
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
  if(isWiFiConnected()){
    display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
  }
  else{
    display.setFont(ArialMT_Plain_10);
    display.drawString(100, 5, "Not Connected"); 
  }  
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
  if(isWiFiConnected()){
    display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
  }
  else{
    display.setFont(ArialMT_Plain_10);
    display.drawString(100, 5, "Not Connected"); 
  }  
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
  if(isWiFiConnected()){
    display.drawXbm(120, 5, 8, 8, wifi_icon_connected);
  }
  else{
    display.setFont(ArialMT_Plain_10);
    display.drawString(100, 5, "Not Connected"); 
  }  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 5, "Alerts");
  
  // Enhanced alert status with detailed information
  if (fireDetected) {
    // Fire alert - match LCD display
    display.drawString(0, 20, "FIRE ALERT!");
    display.drawString(0, 30, "EVACUATE NOW!");
    display.drawString(0, 40, "System: EMERGENCY");
    display.drawString(0, 50, "Status: ACTIVE");
  } else if (motionDetected) {
    // Thief alert - match LCD display
    display.drawString(0, 20, "THIEF ALERT!");
    display.drawString(0, 30, "Security Breach!");
    display.drawString(0, 40, "Location: Door");
    display.drawString(0, 50, "Status: ACTIVE");
  } else {
    // All clear - show normal status
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
  }
  else{
    display.setFont(ArialMT_Plain_10);
    display.drawString(100, 5, "Not Connected"); 
  }  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 5, "Settings");
  
  // Settings menu with better formatting
  String autoSwipeText = "Auto-Swipe: " + String(oledConfig.auto_swipe ? "ON" : "OFF");
  String pagesText = "Pages: " + String(totalPages);
  
  // Show current setting selection indicator
  if (currentState == STATE_SETTINGS) {
    // Highlight current selection with inverted rectangle
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
    
    // Show navigation hint
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 50, "NEXT: Select  PREV: Execute");
  } else {
    // Normal display when not in settings mode
    display.drawString(0, 20, autoSwipeText);
    display.drawString(0, 30, pagesText);
    
    // Show access hint
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 50, "Hold both buttons to toggle auto-swipe");
  }
}

void handleOLEDButtons() {
  // Handle button input
  handleButtons();
  
  // Auto-swipe functionality - PAUSED during hazards
  if (oledConfig.auto_swipe && (millis() - lastAutoSwipe > autoSwipeDelay)) {
    // Only auto-swipe if there are no active hazards
    if (!fireDetected && !motionDetected) {
      currentPage = (currentPage + 1) % totalPages;
      lastAutoSwipe = millis();
    } else {
      // Reset auto-swipe timer during hazards to prevent cycling
      lastAutoSwipe = millis();
    }
  }
}

// Add missing functions
void toggleSettings() {
  if (currentState == STATE_PAGES) {
    currentState = STATE_SETTINGS;
    currentSetting = 0;
    Serial.println("Entered Settings Mode");
  } else {
    currentState = STATE_PAGES;
    Serial.println("Exited Settings Mode");
  }
  updateOLEDDisplay();
}

void executeSetting() {
  switch (currentSetting) {
    case 0: // Auto-swipe
      oledConfig.auto_swipe = !oledConfig.auto_swipe;
      Serial.println("Auto-swipe: " + String(oledConfig.auto_swipe ? "ON" : "OFF"));
      break;
      
    case 1: // Pages (just show info for now)
      Serial.println("Total pages: " + String(totalPages));
      break;
  }
  updateOLEDDisplay();
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