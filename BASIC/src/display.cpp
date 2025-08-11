#include "display.h"
#include "esp_task_wdt.h"

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
        delay(STARTUP_DISPLAY_DELAY);
        esp_task_wdt_reset(); // Reset watchdog during delay
        yield(); // Allow other tasks to run
        lcd.clear();
        delay(200);
        esp_task_wdt_reset(); // Reset watchdog during delay
        yield(); // Allow other tasks to run
    }
}

void displayModeStatus() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    lcd.print(isDay ? "Night" : "Day");
    delay(MODE_DISPLAY_DELAY);
    esp_task_wdt_reset(); // Reset watchdog during delay
    yield(); // Allow other tasks to run
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Smart Shop Guard");
    delay(MODE_DISPLAY_DELAY);
    esp_task_wdt_reset(); // Reset watchdog during delay
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
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Smart Shop Guard");
    displayTemperatureHumidity(t, h);
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