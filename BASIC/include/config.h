#ifndef CONFIG_H
#define CONFIG_H

// Blynk Configuration
#define BLYNK_TEMPLATE_ID "TMPL2jt8pOqfP"
#define BLYNK_TEMPLATE_NAME "Smart Secure Smart Shop"
#define BLYNK_AUTH_TOKEN "LVODBytZ9qETpIjcYFFx5-4b4UFfRUdg"

// WiFi Configuration
#define WIFI_SSID "Amer_ABF"
#define WIFI_PASSWORD "Amer_Ham_2020"
#define WIFI_TIMEOUT 10000  // WiFi connection timeout in milliseconds

// Pin Definitions
#define DHTPIN 40           // DHT sensor pin
#define FAN_PIN 12          // Fan control pin
#define FLAME_SENSOR_PIN 4  // Flame sensor pin
#define BUZZER_PIN 17       // Buzzer pin
#define RELAY_PIN 1         // Relay pin
#define PIR_PIN 5           // PIR motion sensor pin
#define SERVO_PIN 7         // Servo motor pin
#define TRIG_PIN 42         // Ultrasonic sensor trigger pin
#define ECHO_PIN 41         // Ultrasonic sensor echo pin
// new
#define LED_STRIP 20
#define LOUD_BUZZER 19

// DHT Sensor Configuration
#define DHTTYPE DHT11       // DHT sensor type
// LCD Configuration
#define LCD_ADDRESS 0x27    // I2C address for LCD
#define LCD_COLUMNS 16      // LCD columns

// OLED Display Configuration (1.3" 128x64)
#define OLED_SDA_PIN 8     // SDA pin for OLED (ESP32)
#define OLED_SCL_PIN 9     // SCL pin for OLED (ESP32)
#define OLED_ADDRESS 0x3C   // I2C address for OLED

// Button Definitions
#define BUTTON_NEXT 47
#define BUTTON_PREV 48


// Legacy LCD Configuration (commented out - replaced by OLED)
// #define LCD_ADDRESS 0x27    // I2C address for LCD
// #define LCD_COLUMNS 16      // LCD columns
#define LCD_ROWS 2          // LCD rows

// Temperature and Humidity Thresholds
#define TEMP_THRESHOLD 23   // Temperature threshold in Celsius
#define HUMIDITY_THRESHOLD 60  // Humidity threshold in percentage

// Ultrasonic Sensor Configuration
#define DISTANCE_THRESHOLD 12  // Distance threshold in cm for servo activation
#define SERVO_DELAY 3000      // Servo return delay in milliseconds

// Timing Configuration
#define SERIAL_BAUD_RATE 9600  // Serial communication baud rate
#define STARTUP_DISPLAY_DELAY 400  // Startup message display delay
#define MODE_DISPLAY_DELAY 1000    // Mode status display delay

// Blynk Virtual Pins
#define VPIN_TEMPERATURE V0
#define VPIN_HUMIDITY V1
#define VPIN_MOTION V3
#define VPIN_FLAME V4
#define VPIN_DAY_NIGHT V5
#define VPIN_AC_CONTROL V6

#endif // CONFIG_H
