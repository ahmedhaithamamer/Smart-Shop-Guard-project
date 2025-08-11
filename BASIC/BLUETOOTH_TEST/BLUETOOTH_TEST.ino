/*
 * Bluetooth Test Sketch for Smart Shop Guard
 * 
 * This sketch tests the basic Bluetooth functionality
 * before integrating with the main project.
 * 
 * Features:
 * - Bluetooth Classic initialization
 * - Basic command handling
 * - LED status indication
 * - Serial debugging
 */

#include <BluetoothSerial.h>
#include <Arduino.h>

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

BluetoothSerial SerialBT;

// Pin definitions
const int LED_PIN = 2;  // Built-in LED
const int STATUS_LED = 4;  // Status LED

// Bluetooth configuration
const char* DEVICE_NAME = "SmartShopGuard_Test";
const char* PIN = "1234";

// Variables
bool btConnected = false;
unsigned long lastBlink = 0;
const unsigned long BLINK_INTERVAL = 1000;

void setup() {
  Serial.begin(115200);
  Serial.println("=== Bluetooth Test Sketch ===");
  
  // Initialize pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  
  // Turn off LEDs initially
  digitalWrite(LED_PIN, LOW);
  digitalWrite(STATUS_LED, LOW);
  
  // Initialize Bluetooth
  Serial.println("Initializing Bluetooth...");
  SerialBT.begin(DEVICE_NAME);
  SerialBT.setPin(PIN);
  
  Serial.println("Bluetooth initialized successfully!");
  Serial.println("Device name: " + String(DEVICE_NAME));
  Serial.println("PIN: " + String(PIN));
  Serial.println("Waiting for connection...");
  
  // Blink status LED to show ready
  digitalWrite(STATUS_LED, HIGH);
  delay(500);
  digitalWrite(STATUS_LED, LOW);
}

void loop() {
  // Handle Bluetooth input
  if (SerialBT.available()) {
    String input = SerialBT.readStringUntil('\n');
    input.trim();
    
    if (input.length() > 0) {
      Serial.println("BT Received: " + input);
      handleCommand(input);
    }
  }
  
  // Status LED blinking when connected
  if (btConnected) {
    if (millis() - lastBlink > BLINK_INTERVAL) {
      digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
      lastBlink = millis();
    }
  }
  
  // Check connection status
  checkConnection();
  
  delay(100);
}

void handleCommand(const String& command) {
  String cmd = command;
  cmd.toUpperCase();
  String response = "OK";
  
  if (cmd == "HELP") {
    response = "Commands: HELP, STATUS, LED_ON, LED_OFF, BLINK, VERSION";
  }
  else if (cmd == "STATUS") {
    response = "System: OK, LED: " + String(digitalRead(LED_PIN) ? "ON" : "OFF") + 
               ", Connected: " + String(btConnected ? "YES" : "NO");
  }
  else if (cmd == "LED_ON") {
    digitalWrite(LED_PIN, HIGH);
    response = "LED turned ON";
  }
  else if (cmd == "LED_OFF") {
    digitalWrite(LED_PIN, LOW);
    response = "LED turned OFF";
  }
  else if (cmd == "BLINK") {
    response = "Blinking LED 5 times...";
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      delay(200);
    }
  }
  else if (cmd == "VERSION") {
    response = "Smart Shop Guard Bluetooth Test v1.0";
  }
  else {
    response = "Unknown command: " + command;
  }
  
  // Send response
  SerialBT.println("RESPONSE: " + response);
  Serial.println("BT Response: " + response);
}

void checkConnection() {
  static bool wasConnected = false;
  bool isConnected = SerialBT.available() > 0;
  
  if (isConnected != wasConnected) {
    if (isConnected) {
      btConnected = true;
      Serial.println("Bluetooth device connected!");
      SerialBT.println("Welcome to Smart Shop Guard Test!");
      SerialBT.println("Send 'HELP' for available commands");
      
      // Turn on status LED
      digitalWrite(STATUS_LED, HIGH);
    } else {
      btConnected = false;
      Serial.println("Bluetooth device disconnected!");
      
      // Turn off status LED
      digitalWrite(STATUS_LED, LOW);
    }
    wasConnected = isConnected;
  }
}

/*
 * Test Commands:
 * 
 * 1. HELP - Show available commands
 * 2. STATUS - Show system status
 * 3. LED_ON - Turn on built-in LED
 * 4. LED_OFF - Turn off built-in LED
 * 5. BLINK - Blink LED 5 times
 * 6. VERSION - Show version information
 * 
 * Expected Output:
 * - Serial monitor shows all Bluetooth activity
 * - Status LED blinks when connected
 * - Built-in LED responds to commands
 * - All commands return responses
 */ 