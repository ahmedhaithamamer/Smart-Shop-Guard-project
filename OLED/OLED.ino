#include <Wire.h>
#include <SH1106Wire.h>
#include <ESP8266WiFi.h>

#define BUTTON_NEXT D5
#define BUTTON_PREV D6

SH1106Wire display(0x3C, D2, D1); // SDA = D2, SCL = D1

const char* ssid = "Amer_ABF";
const char* password = "Amer_Ham_2020";

int currentPage = 0;
const int totalPages = 3;
unsigned long lastDebounce = 0;
const unsigned long debounceDelay = 200;

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
currentPage = 1;
updateDisplay(); // Show the first page

}

void loop() {
  handleButtons();
  updateDisplay();
  delay(100);
}

void displayWelcome() {
  display.clear();
  display.drawString(0, 0, "Connecting to WiFi...");
  display.display();
}

void handleButtons() {
  if (millis() - lastDebounce > debounceDelay) {
    if (digitalRead(BUTTON_NEXT) == LOW) {
      currentPage = (currentPage + 1) % totalPages;
      lastDebounce = millis();
    }
    if (digitalRead(BUTTON_PREV) == LOW) {
      currentPage = (currentPage - 1 + totalPages) % totalPages;
      lastDebounce = millis();
    }
  }
}

void updateDisplay() {
  display.clear();

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
  }

  display.display();
}

void showWiFiStatus() {
  display.drawString(0, 0, "WiFi Status");

  if (WiFi.status() == WL_CONNECTED) {
    display.drawString(0, 20, "Connected");
    display.drawString(0, 40, WiFi.localIP().toString());
  } else {
    display.drawString(0, 20, "Not Connected");
  }
}

void showSensorData() {
  // Fake sensor values – replace with real sensors later
  float temperature = 25.6;
  float humidity = 60.2;

  display.drawString(0, 0, "Sensor Data");
  display.drawString(0, 20, "Temp: " + String(temperature) + "C");
  display.drawString(0, 40, "Humidity: " + String(humidity) + "%");
}

void showSystemInfo() {
  display.drawString(0, 0, "System Info");
  display.drawString(0, 20, "Free Heap:");
  display.drawString(0, 40, String(ESP.getFreeHeap()) + " bytes");
}
