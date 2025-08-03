#include "display_manager.h"
#include "config.h"

DisplayManager::DisplayManager() {
    lcd = new LiquidCrystal_I2C(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
    oled = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    
    temperature = 0.0;
    humidity = 0.0;
    isDayMode = false;
    wifiStatus = "Unknown";
    blynkStatus = "Unknown";
    systemStatus = "Idle";
    
    lcdEnabled = true;
    oledEnabled = true;
    lastUpdate = 0;
}

void DisplayManager::begin() {
    // Initialize LCD
    lcd->init();
    lcd->backlight();
    lcd->setCursor(0, 0);
    lcd->print("Starting...");
    
    // Initialize OLED
    if (!oled->begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("OLED failed to start");
        oledEnabled = false;
    } else {
        oled->clearDisplay();
        oled->setTextColor(WHITE);
        oled->setTextSize(1);
        oled->setCursor(0, 0);
        oled->println("OLED Init...");
        oled->display();
    }
    
    Serial.println("Displays initialized");
}

void DisplayManager::updateDisplays(float temp, float hum, bool dayMode, String wifi, String blynk, String system) {
    temperature = temp;
    humidity = hum;
    isDayMode = dayMode;
    wifiStatus = wifi;
    blynkStatus = blynk;
    systemStatus = system;
    
    updateLCD();
    updateOLED();
}

void DisplayManager::updateLCD() {
    if (!lcdEnabled) return;
    
    lcd->setCursor(0, 0);
    lcd->print("T:"); lcd->print(temperature, 1); lcd->print("C ");
    lcd->setCursor(8, 0);
    lcd->print("H:"); lcd->print(humidity, 1); lcd->print("%");
    
    lcd->setCursor(0, 1);
    lcd->print("Mode: ");
    lcd->print(isDayMode ? "Night" : "Day");
}

void DisplayManager::updateOLED() {
    if (!oledEnabled) return;
    
    oled->clearDisplay();
    oled->setCursor(0, 0);
    oled->setTextSize(1);
    oled->println("Smart Secure Shop");
    
    oled->setCursor(0, 15);
    oled->print("WiFi: ");
    oled->println(wifiStatus);
    
    oled->setCursor(0, 30);
    oled->print("Blynk: ");
    oled->println(blynkStatus);
    
    oled->setCursor(0, 45);
    oled->print("Mode: ");
    oled->println(systemStatus);
    
    oled->display();
}

void DisplayManager::enableLCD(bool enabled) {
    lcdEnabled = enabled;
    if (!enabled) {
        lcd->clear();
        lcd->noBacklight();
    } else {
        lcd->backlight();
    }
}

void DisplayManager::enableOLED(bool enabled) {
    oledEnabled = enabled;
    if (!enabled) {
        oled->clearDisplay();
        oled->display();
    }
}

void DisplayManager::clearDisplays() {
    if (lcdEnabled) {
        lcd->clear();
    }
    if (oledEnabled) {
        oled->clearDisplay();
        oled->display();
    }
}

bool DisplayManager::isLCDEnabled() {
    return lcdEnabled;
}

bool DisplayManager::isOLEDEnabled() {
    return oledEnabled;
}

void DisplayManager::showMessage(const char* message, int line) {
    if (lcdEnabled) {
        lcd->setCursor(0, line);
        lcd->print(message);
    }
}

void DisplayManager::showStatus(const char* status) {
    systemStatus = status;
    updateOLED();
}

void DisplayManager::showAlert(const char* alert) {
    if (lcdEnabled) {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("ALERT:");
        lcd->setCursor(0, 1);
        lcd->print(alert);
    }
    
    if (oledEnabled) {
        oled->clearDisplay();
        oled->setCursor(0, 0);
        oled->setTextSize(1);
        oled->println("ALERT!");
        oled->setCursor(0, 20);
        oled->println(alert);
        oled->display();
    }
} 