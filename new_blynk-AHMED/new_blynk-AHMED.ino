#define BLYNK_TEMPLATE_ID "TMPL2jt8pOqfP"
#define BLYNK_TEMPLATE_NAME "Smart Secure Smart Shop"
#define BLYNK_AUTH_TOKEN "LVODBytZ9qETpIjcYFFx5-4b4UFfRUdg"

#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP32Servo.h>           // ✅ Replace with ESP32-compatible servo lib
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// WiFi credentials
char ssid[] = "Amer_ABF";
char pass[] = "Amer_Ham_2020";

// DHT Sensor setup
#define DHTPIN 40        // ✅ Was D5 —> use GPIO13 (customize as needed)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Pin definitions
#define FAN_PIN 12             // ✅ Was D6 —> use GPIO12 //
#define FLAME_SENSOR_PIN 4    // ✅ Was D7 —> GPIO14
#define BUZZER_PIN 17          // ✅ Was D8 —> GPIO27
#define RELAY_PIN 1            // ✅ Was D4 —> GPIO4
#define PIR_PIN 5            // ✅ Was D0 —> GPIO26
#define SERVO_PIN 7           // ✅ Was D3 —> GPIO25
#define TRIG_PIN 42            // ✅ Was GPIO1 (avoid TX/RX) —> GPIO33
#define ECHO_PIN 41            // ✅ Was GPIO3 —> GPIO32

Servo myServo;

// Variables
int h;  // Humidity
int t;  // Temperature
long duration;
int distance;
int degree = 0;
int Servodegree = 0;
unsigned long lastTime = 0;
unsigned long startTime;
bool isDay = false;
bool AC = false;
unsigned long lastStatusPrint = 0;

// Function declarations
void fanTempLCD();
void readFlameSensor();
void readMotion();
void triggerUltrasonicSensor();
void moveServo();
void StartupTone();
void ModeSwitchTone();
void alertTone();
void displayModeStatus();

BLYNK_WRITE(V5) {
  isDay = param.asInt();
  ModeSwitchTone();
  displayModeStatus();
}

BLYNK_WRITE(V6) {
  AC = param.asInt();
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);

  unsigned long wifiConnectStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiConnectStart < 10000) {
    delay(200);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
    Blynk.config(BLYNK_AUTH_TOKEN);
    Blynk.connect();
  } else {
    Serial.println("WiFi not connected");
  }

  pinMode(FAN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);

  myServo.attach(SERVO_PIN);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  dht.begin();
  lcd.init();
  lcd.backlight();
  StartupTone();

  for (int i = 0; i < 3; i++) {
    lcd.setCursor(4, 0);
    lcd.print("Welcome!");
    delay(400);
    lcd.clear();
    delay(200);
  }

  displayModeStatus();
  startTime = millis();
}

void fanTempLCD() {
  h = dht.readHumidity();
  t = dht.readTemperature();

  if (t > 23 || h > 60 || AC)
    digitalWrite(FAN_PIN, HIGH);
  else
    digitalWrite(FAN_PIN, LOW);

  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(t);
  lcd.print("C ");

  lcd.setCursor(11, 1);
  lcd.print("H:");
  lcd.print(h);
  lcd.print("%");

  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, h);
  }
}

void readFlameSensor() {
  int value1 = digitalRead(FLAME_SENSOR_PIN);
  if (value1 == 0) {
    alertTone();
    digitalWrite(RELAY_PIN, HIGH);
    lcd.setCursor(6, 1);
    lcd.print("Fire!");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
    lcd.setCursor(6, 1);
    lcd.print("SAFE ");
  }

  if (Blynk.connected()) {
    Blynk.virtualWrite(V4, !value1);
  }

  delay(100);
}

void readMotion() {
  int pirState = digitalRead(PIR_PIN);
  if (pirState == HIGH) {
    lcd.setCursor(5, 1);
    lcd.print("Thief!");
    alertTone();
  }

  if (Blynk.connected()) {
    Blynk.virtualWrite(V3, pirState);
  }

  delay(200);
}

void loop() {
  Blynk.run();
  if (!isDay) {
    fanTempLCD();
    readFlameSensor();
    triggerUltrasonicSensor();
    moveServo();
  } else {
    fanTempLCD();
    readFlameSensor();
    readMotion();
  }
}

void moveServo() {
  if (distance <= 12) {
    degree = 180;
    lastTime = millis();
  }

  if (millis() - lastTime >= 3000) {
    degree = 0;
  }

  myServo.write(degree);
  delay(15);
}

void triggerUltrasonicSensor() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void StartupTone() {
  int startupNotes[] = {523, 659, 784, 1047, 1319, 1568 };
  int duration = 150;
  for (int i = 0; i < sizeof(startupNotes) / sizeof(startupNotes[0]); i++) {
    tone(BUZZER_PIN, startupNotes[i], duration);
    delay(duration + 20);
  }
  noTone(BUZZER_PIN);
}

void ModeSwitchTone() {
  int switchNotes[] = {784, 988, 1175};
  int duration = 120;
  for (int i = 0; i < sizeof(switchNotes) / sizeof(switchNotes[0]); i++) {
    tone(BUZZER_PIN, switchNotes[i], duration);
    delay(duration + 20);
  }
  noTone(BUZZER_PIN);
}

void alertTone() {
  int duration = 300;
  int tone1 = 880;
  int tone2 = 440;

  for (int i = 0; i < 5; i++) {
    tone(BUZZER_PIN, tone1, duration);
    delay(duration);
    tone(BUZZER_PIN, tone2, duration);
    delay(duration);
  }

  tone(BUZZER_PIN, tone1, 500);
  delay(500);
  noTone(BUZZER_PIN);
}

void displayModeStatus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode: ");
  lcd.print(isDay ? "Night" : "Day");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gahabeez Market");
  delay(1000);
}
