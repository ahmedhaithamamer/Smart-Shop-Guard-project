#include "display.h"

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
        vTaskDelay(pdMS_TO_TICKS(STARTUP_DISPLAY_DELAY));   
        yield(); // Allow other tasks to run
        lcd.clear();
        vTaskDelay(pdMS_TO_TICKS(200));   
        yield(); // Allow other tasks to run
    }
}



void displayModeStatus() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    lcd.print(isDay ? "Night" : "Day");
    vTaskDelay(pdMS_TO_TICKS(MODE_DISPLAY_DELAY));   
    yield(); // Allow other tasks to run
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Smart Shop Guard");
        vTaskDelay(pdMS_TO_TICKS(MODE_DISPLAY_DELAY));   
    yield(); // Allow other tasks to run
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
    static int slide = 0;
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
    switch (slide) {
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
    
    slide = (slide + 1) % 7;
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