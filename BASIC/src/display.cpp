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

    lcd.setCursor(11, 1);
    lcd.print("H:");
    lcd.print(humidity);
    lcd.print("%");
}

void displayWelcomeMessage() {
    for (int i = 0; i < 3; i++) {
        lcd.setCursor(4, 0);
        lcd.print("Welcome!");
        delay(STARTUP_DISPLAY_DELAY);
        yield(); // Allow other tasks to run
        lcd.clear();
        delay(200);
        yield(); // Allow other tasks to run
    }
}

void displayModeStatus() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    lcd.print(isDay ? "Night" : "Day");
    delay(MODE_DISPLAY_DELAY);
    yield(); // Allow other tasks to run
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Gahabeez Market");
    delay(MODE_DISPLAY_DELAY);
    yield(); // Allow other tasks to run
}

void displayFireAlert() {
    lcd.setCursor(6, 1);
    lcd.print("Fire!");
}

void displaySafeStatus() {
    lcd.setCursor(6, 1);
    lcd.print("SAFE ");
}

void displayThiefAlert() {
    lcd.setCursor(5, 1);
    lcd.print("Thief!");
}