// 🚪 Simple Servo Test for Smart Shop Guard
// Test servo movement with ultrasonic sensor on ESP32-S3

#include <ESP32Servo.h>

// Pin definitions (matching your project)
#define SERVO_PIN 7
#define TRIG_PIN 42
#define ECHO_PIN 41
#define DISTANCE_THRESHOLD 12

Servo myServo;
long duration;
int distance;

void setup() {
  Serial.begin(115200);
  Serial.println("🚪 Smart Shop Guard - Servo Test Starting...");
  
  // Initialize pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Initialize servo
  myServo.attach(SERVO_PIN);
  myServo.write(0);  // Start closed
  Serial.println("🚪 Servo initialized to CLOSED (0°)");
  delay(1000);
  
  // Test servo movement
  Serial.println("🚪 Testing servo movement...");
  myServo.write(180);  // Open
  Serial.println("🚪 Servo OPEN (180°)");
  delay(2000);
  
  myServo.write(0);    // Close
  Serial.println("🚪 Servo CLOSED (0°)");
  delay(2000);
  
  Serial.println("🚪 Starting distance-based control...");
  Serial.println("🚪 Move object within 12cm to open door");
}

void loop() {
  // Read ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) {
    distance = 999;  // Out of range
  } else {
    distance = duration * 0.034 / 2;
  }
  
  // Print distance every second
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    Serial.printf("📏 Distance: %dcm ", distance);
    if (distance <= DISTANCE_THRESHOLD) {
      Serial.println("- DOOR SHOULD BE OPEN");
      myServo.write(180);
    } else {
      Serial.println("- DOOR SHOULD BE CLOSED");
      myServo.write(0);
    }
    lastPrint = millis();
  }
  
  delay(100);
}