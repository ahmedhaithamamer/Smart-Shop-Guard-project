#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// WiFi Configuration
#define WIFI_SSID "Amer_ABF"
#define WIFI_PASSWORD "Amer_Ham_2020"

// Pin Definitions
#define DHTPIN 5
#define DHTTYPE DHT11

#define FAN_PIN 6
#define FLAME_SENSOR_PIN 7
#define BUZZER_PIN 8
#define RELAY_PIN 4
#define PIR_PIN 0
#define SERVO_PIN 3
#define TRIG_PIN 1
#define ECHO_PIN 2

// LCD Configuration
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

// Task Priorities
#define TASK_PRIORITY_LOW 1
#define TASK_PRIORITY_MEDIUM 2
#define TASK_PRIORITY_HIGH 3

// Task Stack Sizes
#define TASK_STACK_SIZE_SMALL 2048
#define TASK_STACK_SIZE_MEDIUM 4096
#define TASK_STACK_SIZE_LARGE 8192

// Timing Constants
#define TEMP_HUMIDITY_DELAY 2000
#define FLAME_DELAY 500
#define MOTION_DELAY 1000
#define ULTRASONIC_DELAY 500
#define LCD_DELAY 1500
#define OLED_DELAY 2000
#define BLYNK_DELAY 10

// Threshold Values
#define TEMP_THRESHOLD 23
#define HUMIDITY_THRESHOLD 60
#define DISTANCE_THRESHOLD 12
#define SERVO_TIMEOUT 3000

// Blynk Virtual Pins (using integer constants to avoid Blynk dependency)
#define VIRTUAL_PIN_TEMP 0
#define VIRTUAL_PIN_HUMIDITY 1
#define VIRTUAL_PIN_MOTION 3
#define VIRTUAL_PIN_FLAME 4
#define VIRTUAL_PIN_DAY_MODE 5
#define VIRTUAL_PIN_AC 6

#endif 