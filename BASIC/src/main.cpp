#include "config.h"
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP32Servo.h>         
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// LCD setup
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// WiFi credentials
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

// DHT Sensor setup
DHT dht(DHTPIN, DHTTYPE);

// Pin definitions are now in config.h            

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

BLYNK_WRITE(VPIN_DAY_NIGHT) {
  isDay = param.asInt();
  ModeSwitchTone();
  displayModeStatus();
}

BLYNK_WRITE(VPIN_AC_CONTROL) {
  AC = param.asInt();
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  WiFi.begin(ssid, pass);

  unsigned long wifiConnectStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiConnectStart < WIFI_TIMEOUT) {
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
    delay(STARTUP_DISPLAY_DELAY);
    lcd.clear();
    delay(200);
  }

  displayModeStatus();
  startTime = millis();
}

void fanTempLCD() {
  h = dht.readHumidity();
  t = dht.readTemperature();

  if (t > TEMP_THRESHOLD || h > HUMIDITY_THRESHOLD || AC)
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
    Blynk.virtualWrite(VPIN_TEMPERATURE, t);
    Blynk.virtualWrite(VPIN_HUMIDITY, h);
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
    Blynk.virtualWrite(VPIN_FLAME, !value1);
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
    Blynk.virtualWrite(VPIN_MOTION, pirState);
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
  if (distance <= DISTANCE_THRESHOLD) {
    degree = 180;
    lastTime = millis();
  }

  if (millis() - lastTime >= SERVO_DELAY) {
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
  delay(MODE_DISPLAY_DELAY);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gahabeez Market");
  delay(MODE_DISPLAY_DELAY);
}
