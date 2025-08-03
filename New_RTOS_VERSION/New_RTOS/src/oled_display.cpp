#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "oled_display.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR     0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String wifiStatusStr = "Connecting...";
String blynkStatusStr = "Connecting...";
String modeStatusStr = "Idle";

void initOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED failed to start");
    return;
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("OLED Init...");
  display.display();
}

void updateOLEDStatus(const char* wifi, const char* blynk, const char* mode) {
  wifiStatusStr = wifi;
  blynkStatusStr = blynk;
  modeStatusStr = mode;
}

void oledTask(void* pvParams) {
  while (true) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Smart Secure Shop");

    display.setCursor(0, 15);
    display.print("WiFi: ");
    display.println(wifiStatusStr);

    display.setCursor(0, 30);
    display.print("Blynk: ");
    display.println(blynkStatusStr);

    display.setCursor(0, 45);
    display.print("Mode: ");
    display.println(modeStatusStr);

    display.display();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void startOLEDTask() {
  xTaskCreatePinnedToCore(oledTask, "OLEDTask", 2048, NULL, 1, NULL, 1);
}
