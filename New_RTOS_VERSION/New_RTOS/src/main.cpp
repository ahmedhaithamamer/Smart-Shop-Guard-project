#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char ssid[] = "Amer_ABF";
char pass[] = "Amer_Ham_2020";

// --- LCD ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Sensors/Actuators ---
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define FAN_PIN 6
#define FLAME_SENSOR_PIN 7
#define BUZZER_PIN 8
#define RELAY_PIN 4
#define PIR_PIN 0
#define SERVO_PIN 3
#define TRIG_PIN 1
#define ECHO_PIN 2

Servo myServo;

// --- Variables ---
bool isDay = false;
bool AC = false;
int distance;
int t, h;
int degree = 0;
unsigned long lastServoTime = 0;

// --- Tasks ---
void TaskBlynk(void *pvParameters);
void TaskTempHumidity(void *pvParameters);
void TaskFlame(void *pvParameters);
void TaskMotion(void *pvParameters);
void TaskUltrasonicServo(void *pvParameters);
void TaskLCD(void *pvParameters);

// --- BLYNK Buttons ---
BLYNK_WRITE(V5) {
  isDay = param.asInt();
  //displayModeStatus();
}

BLYNK_WRITE(V6) {
  AC = param.asInt();
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();
  myServo.attach(SERVO_PIN);

  pinMode(FAN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  lcd.setCursor(0, 0); lcd.print("Starting...");
  delay(1000);

  // --- Create Tasks ---
  xTaskCreatePinnedToCore(TaskBlynk, "Blynk", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskTempHumidity, "TempHumidity", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskFlame, "Flame", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskMotion, "Motion", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskUltrasonicServo, "Ultrasonic", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskLCD, "LCD", 2048, NULL, 1, NULL, 1);
}

void loop() {
  // Empty. All logic handled in RTOS tasks.
}

// --- Tasks Implementation ---
void TaskBlynk(void *pvParameters) {
  while (1) {
    Blynk.run();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void TaskTempHumidity(void *pvParameters) {
  while (1) {
    h = dht.readHumidity();
    t = dht.readTemperature();

    if (t > 23 || h > 60 || AC)
      digitalWrite(FAN_PIN, HIGH);
    else
      digitalWrite(FAN_PIN, LOW);

    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, h);

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void TaskFlame(void *pvParameters) {
  while (1) {
    int flame = digitalRead(FLAME_SENSOR_PIN);
    if (flame == 0) {
      tone(BUZZER_PIN, 1000);
      digitalWrite(RELAY_PIN, HIGH);
    } else {
      noTone(BUZZER_PIN);
      digitalWrite(RELAY_PIN, LOW);
    }
    Blynk.virtualWrite(V4, !flame);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void TaskMotion(void *pvParameters) {
  while (1) {
    if (isDay) {
      int motion = digitalRead(PIR_PIN);
      if (motion) tone(BUZZER_PIN, 1500, 200);
      Blynk.virtualWrite(V3, motion);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void TaskUltrasonicServo(void *pvParameters) {
  while (1) {
    if (!isDay) {
      digitalWrite(TRIG_PIN, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIG_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIG_PIN, LOW);
      long duration = pulseIn(ECHO_PIN, HIGH);
      distance = duration * 0.034 / 2;

      if (distance <= 12) {
        degree = 180;
        lastServoTime = millis();
      }

      if (millis() - lastServoTime >= 3000) {
        degree = 0;
      }

      myServo.write(degree);
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void TaskLCD(void *pvParameters) {
  while (1) {
    lcd.setCursor(0, 0);
    lcd.print("T:"); lcd.print(t); lcd.print("C ");
    lcd.setCursor(8, 0);
    lcd.print("H:"); lcd.print(h); lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("Mode: ");
    lcd.print(isDay ? "Night" : "Day");
    vTaskDelay(pdMS_TO_TICKS(1500));
  }
}

// --- Helper ---
void displayModeStatus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode:");
  lcd.print(isDay ? " Night" : " Day");
  delay(1000);
}
