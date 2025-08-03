#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <LiquidCrystal_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

class DisplayManager {
private:
    LiquidCrystal_I2C* lcd;
    Adafruit_SSD1306* oled;
    
    // Display data
    float temperature;
    float humidity;
    bool isDayMode;
    String wifiStatus;
    String blynkStatus;
    String systemStatus;
    
    // Display control
    bool lcdEnabled;
    bool oledEnabled;
    unsigned long lastUpdate;

public:
    DisplayManager();
    void begin();
    void updateDisplays(float temp, float hum, bool dayMode, String wifi, String blynk, String system);
    
    // Individual display updates
    void updateLCD();
    void updateOLED();
    
    // Display control
    void enableLCD(bool enabled);
    void enableOLED(bool enabled);
    void clearDisplays();
    
    // Status methods
    bool isLCDEnabled();
    bool isOLEDEnabled();
    
    // Custom display methods
    void showMessage(const char* message, int line = 0);
    void showStatus(const char* status);
    void showAlert(const char* alert);
};

#endif 