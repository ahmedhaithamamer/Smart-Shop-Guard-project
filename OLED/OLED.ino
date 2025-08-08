#include <Wire.h>
#include <SH1106Wire.h>
#include <ESP8266WiFi.h>
#include "icons.h"

#define BUTTON_NEXT D5
#define BUTTON_PREV D6

SH1106Wire display(0x3C, D2, D1); // SDA = D2, SCL = D1

const char* ssid = "Amer_ABF";
const char* password = "Amer_Ham_2020";

int currentPage = 0;
const int totalPages = 5; // 5 screens total
unsigned long lastDebounce = 0;
const unsigned long debounceDelay = 300; // Increased for better debouncing
unsigned long lastAutoSwipe = 0;
const unsigned long autoSwipeDelay = 15000; // Auto-swipe every 15 seconds
bool autoSwipeEnabled = true;

// All icons are now defined in icons.h

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_PREV, INPUT_PULLUP);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);

  WiFi.begin(ssid, password);

  // Wait until connected (optional visual feedback)
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

// ✅ After successful connection
display.clear();
display.drawString(0, 0, "WiFi Connected!");
display.display();
delay(1000); // Just show it briefly

// Start with the first page after connection
currentPage = 0;
updateDisplay(); // Show WiFi status page

}

void loop() {
  handleButtons();
  handleAutoSwipe();
  updateDisplay();
  delay(50); // Reduced delay for better responsiveness
}

void displayWelcome() {
  display.clear();
  display.drawString(0, 0, "Connecting to WiFi...");
  display.display();
}

void handleButtons() {
  static bool nextPressed = false;
  static bool prevPressed = false;
  static unsigned long bothButtonsStart = 0;
  
  bool nextState = (digitalRead(BUTTON_NEXT) == LOW);
  bool prevState = (digitalRead(BUTTON_PREV) == LOW);
  
  // Check if both buttons are pressed (toggle auto-swipe)
  if (nextState && prevState) {
    if (bothButtonsStart == 0) {
      bothButtonsStart = millis(); // Start timing
    } else if (millis() - bothButtonsStart > 1000) { // Hold for 1 second
      autoSwipeEnabled = !autoSwipeEnabled;
      lastDebounce = millis();
      lastAutoSwipe = millis();
      bothButtonsStart = 0;
      Serial.println("Auto-swipe toggled: " + String(autoSwipeEnabled ? "ON" : "OFF"));
    }
    return; // Don't process individual buttons when both are pressed
  } else {
    bothButtonsStart = 0; // Reset if both buttons not pressed
  }
  
  // Check if enough time has passed since last button press
  if (millis() - lastDebounce > debounceDelay) {
    
    // Handle NEXT button (rising edge detection)
    if (nextState && !nextPressed) {
      currentPage = (currentPage + 1) % totalPages;
      lastDebounce = millis();
      lastAutoSwipe = millis(); // Reset auto-swipe timer
      Serial.println("Next button pressed - Page: " + String(currentPage));
    }
    
    // Handle PREV button (rising edge detection)  
    if (prevState && !prevPressed) {
      currentPage = (currentPage - 1 + totalPages) % totalPages;
      lastDebounce = millis();
      lastAutoSwipe = millis(); // Reset auto-swipe timer
      Serial.println("Prev button pressed - Page: " + String(currentPage));
    }
  }
  
  // Update button states
  nextPressed = nextState;
  prevPressed = prevState;
}

void handleAutoSwipe() {
  if (autoSwipeEnabled && (millis() - lastAutoSwipe > autoSwipeDelay)) {
    currentPage = (currentPage + 1) % totalPages;
    lastAutoSwipe = millis();
    Serial.println("Auto-swipe to page: " + String(currentPage));
  }
}

void updateDisplay() {
  display.clear();
  
  // Draw compact title bar
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Shop Guard");
  
  // Draw page indicator (right aligned)
  String pageIndicator = String(currentPage + 1) + "/" + String(totalPages);
  if (autoSwipeEnabled) {
    pageIndicator += " ●"; // Add dot to indicate auto-swipe is active
  }
  display.drawString(128 - (pageIndicator.length() * 6), 0, pageIndicator);
  
  // Thin separator line
  display.drawLine(0, 10, 128, 10);

  switch (currentPage) {
    case 0:
      showWiFiStatus();
      break;
    case 1:
      showSensorData();
      break;
    case 2:
      showSystemInfo();
      break;
    case 3:
      showFireDetection();
      break;
    case 4:
      showSecurityStatus();
      break;
  }

  display.display();
}

void showWiFiStatus() {
  // Draw WiFi icon (smaller, positioned better)
  display.drawXbm(0, 13, 16, 16, wifi_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "WiFi");
  
  if (WiFi.status() == WL_CONNECTED) {
    display.drawString(0, 26, "Connected");
    display.drawString(0, 36, WiFi.localIP().toString());
    display.drawString(0, 46, "RSSI:" + String(WiFi.RSSI()));
  } else {
    display.drawString(0, 26, "Disconnected");
    display.drawString(0, 36, "Connecting...");
  }
}

void showSensorData() {
  // Fake sensor values – replace with real sensors later
  float temperature = 25.6;
  float humidity = 60.2;
  
  // Draw temperature icon
  display.drawXbm(0, 13, 16, 16, temp_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "Sensors");
  
  display.drawString(0, 26, "Temp: " + String(temperature) + "C");
  display.drawString(0, 36, "Humid: " + String(humidity) + "%");
  display.drawString(0, 46, "Air: Good");
}

void showSystemInfo() {
  // Draw system icon
  display.drawXbm(0, 13, 16, 16, system_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "System");
  
  display.drawString(0, 26, "Heap: " + String(ESP.getFreeHeap() / 1024) + "KB");
  display.drawString(0, 36, "Up: " + String(millis() / 1000) + "s");
  display.drawString(0, 46, "Flash: " + String(ESP.getFlashChipSize() / 1024) + "KB");
}

void showFireDetection() {
  // Draw fire icon
  display.drawXbm(0, 13, 16, 16, fire_icon);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "Fire");
  
  // Simulate fire detection status
  bool fireDetected = false; // Replace with real sensor reading
  
  if (fireDetected) {
    display.drawString(0, 26, "FIRE DETECTED!");
    display.drawString(0, 36, "Temp: HIGH");
    display.drawString(0, 46, "ALARM: ON");
  } else {
    display.drawString(0, 26, "Safe");
    display.drawString(0, 36, "Temp: Normal");
    display.drawString(0, 46, "Smoke: Low");
  }
}

void showSecurityStatus() {
  // Draw security icon
  display.drawXbm(0, 13, 16, 16, lock_icon);
  display.setFont(ArialMT_Plain_10);
  display.drawString(18, 13, "Security");
  
  // Simulate security status
  bool intruderDetected = false; // Replace with real sensor reading
  bool doorOpen = false; // Replace with real sensor reading
  
  if (intruderDetected) {
    display.drawString(0, 26, "INTRUDER!");
    display.drawString(0, 36, "Location: Door");
    display.drawString(0, 46, "Alert: ACTIVE");
  } else {
    display.drawString(0, 26, "Secure");
    display.drawString(0, 36, "Door: " + String(doorOpen ? "Open" : "Closed"));
    display.drawString(0, 46, "Motion: None");
  }
}

