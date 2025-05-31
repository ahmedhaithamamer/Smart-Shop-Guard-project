#define BLYNK_TEMPLATE_ID "TMPL2jt8pOqfP"
#define BLYNK_TEMPLATE_NAME "Smart Secure Smart Shop"
#define BLYNK_AUTH_TOKEN "LVODBytZ9qETpIjcYFFx5-4b4UFfRUdg"

#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

//blynk
char ssid[]="Amer_ABF";
char pass[]="Amer_Ham_2020";

// Constants for sensors
#define DHTPIN D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define FAN_PIN D6
#define FLAME_SENSOR_PIN D7
#define BUZZER_PIN D8
#define RELAY_PIN D4
#define PIR_PIN D0     // PIR sensor pin
#define SERVO_PIN D3   // Servo motor pin
#define TRIG_PIN 1  //TX   // Ultrasonic Trigger pin
#define ECHO_PIN 3  //RX   // Ultrasonic Echo pin

// Create servo object
Servo myServo;

// Variables for temperature, humidity, and flame sensor
int h;  // Humidity
int t;  // Temperature

long duration;
int distance;
int degree = 0;  // Servo angle
int Servodegree = 0;
unsigned long lastTime = 0;  // Store time of last servo movement
unsigned long startTime;

bool isDay = false;   // Default to Day Mode
bool AC = false;     // Default AC OFF

unsigned long lastStatusPrint = 0;  // Declare at the top of your file

// Blynk bottons ///

// Blynk write for Day/Night switch


BLYNK_WRITE(V5) {
  isDay = param.asInt();
  //if (isDay) --> Day on   if (!isDay) --> Night on 
  ModeSwitchTone();
  displayModeStatus();
}

BLYNK_WRITE(V6){
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
    Blynk.connect(); // Non-blocking connect
  }   else {
    Serial.println("WiFi not connected");
  }
 
  // Initialize pins
  pinMode(FAN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT); // PIR sensor input pin



  // Initialize servo
  myServo.attach(SERVO_PIN);

  // Initialize ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize DHT sensor and LCD
  dht.begin();
  lcd.init();
  lcd.backlight();
  StartupTone();
  // Display Welcome message for 2 seconds
  for (int i = 0; i < 3; i++)
  {
    lcd.setCursor(4, 0);
    lcd.print("Welcome!");
    delay(400);  // Show for 500ms
    lcd.clear();
    delay(200);
  }
  displayModeStatus();

  startTime = millis();
}

void fanTempLCD() {
  // Read humidity and temperature
  h = dht.readHumidity();
  t = dht.readTemperature();

  // Fan control based on temperature and humidity
  if (t > 23 || h > 60 || AC)
    digitalWrite(FAN_PIN, HIGH);
  else
    digitalWrite(FAN_PIN, LOW);

  // Display information on LCD
  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(t);
  lcd.print("C ");

  lcd.setCursor(11, 1);
  lcd.print("H:");
  lcd.print(h);
  lcd.print("%");

  if (Blynk.connected()) 
  {
    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, h);
  }

}

void readFlameSensor() {
  int value1 = digitalRead(FLAME_SENSOR_PIN);
  if (value1 == 0) {
    //digitalWrite(BUZZER_PIN, HIGH);
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

  if (Blynk.connected()) 
  {
    Blynk.virtualWrite(V4, !value1);
  }

  delay(100);
}

void readMotion() {
  int pirState = digitalRead(PIR_PIN);  // Read PIR sensor
  if (pirState == HIGH ) {
    lcd.setCursor(5, 1);
    lcd.print("Thief!");
    alertTone();
  } 
  if (Blynk.connected()) 
  {
    Blynk.virtualWrite(V3, pirState);
  }
  
  delay(200);  // Small delay to avoid continuous triggering
}

void loop() {
  Blynk.run();
  // Call all functions
  if (!isDay)
  {
    fanTempLCD();
    readFlameSensor();
    triggerUltrasonicSensor();
    moveServo();    
  }
  else
  {
    fanTempLCD();
    readFlameSensor();
    readMotion();
  }



}

void moveServo()
{
    // If the distance is less than or equal to 15 cm, set servo to 180 degrees
  if (distance <= 12) 
  {
    degree = 180;
    lastTime = millis();  // Record the time when the servo moves to 180
  }

  // If the servo has been at 180 degrees for 3 seconds, move it back to 0 degrees
  if (millis() - lastTime >= 3000) 
  {
    degree = 0;
  }

  // Rotate the servo to the desired position
  myServo.write(degree);

  // Wait a little bit to allow the servo to move
  delay(15);
}

void triggerUltrasonicSensor()
{
  // Send a pulse to trigger the ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the time it takes for the pulse to return (echo)
  duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance based on the duration of the echo
  distance = duration * 0.034 / 2; // Speed of sound is 0.034 cm/us

  // Print the distance to the serial monitor for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void StartupTone() {
  // Clean 6-tone startup melody
  int startupNotes[] = {523, 659, 784, 1047, 1319, 1568 };

  int duration = 150;  // milliseconds per note

  for (int i = 0; i < sizeof(startupNotes) / sizeof(startupNotes[0]); i++) {
    tone(BUZZER_PIN, startupNotes[i], duration);
    delay(duration + 20);  // short pause between notes
  }

  noTone(BUZZER_PIN);
}

void ModeSwitchTone() {
  int switchNotes[] = {784, 988, 1175}; // G5, B5, D6
  int duration = 120;

  for (int i = 0; i < sizeof(switchNotes) / sizeof(switchNotes[0]); i++) {
    tone(BUZZER_PIN, switchNotes[i], duration);
    delay(duration + 20);
  }

  noTone(BUZZER_PIN);
}

void alertTone()
{
    int duration = 300;  // Duration of each tone

    // Define two alternating tones for the siren effect
    int tone1 = 880;  // A5 (Higher tone)
    int tone2 = 440;  // A4 (Lower tone)

    // Play the alternating tones to mimic a siren effect
    for (int i = 0; i < 5; i++) {
        tone(BUZZER_PIN, tone1, duration);  // Play the first tone
        delay(duration);  // Short pause between tones
        tone(BUZZER_PIN, tone2, duration);  // Play the second tone
        delay(duration);  // Short pause between tones
    }
    tone(BUZZER_PIN, tone1, 500);  // Play the last tone
    delay(500);
    
    noTone(BUZZER_PIN);  // Stop the tone 
}

void displayModeStatus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode: ");
  lcd.print(isDay ? "Night" : "Day");
  delay(1000);
  // Clear the screen and display stats
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gahabeez Market");
  delay(1000);  // Give time to display the market name
}
