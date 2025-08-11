#include "audio.h"
#include "esp_task_wdt.h"
#include <Arduino.h>

// Use LEDC for buzzer on ESP32
#ifndef BUZZER_LEDC_CHANNEL
#define BUZZER_LEDC_CHANNEL 7
#endif
#ifndef BUZZER_LEDC_RES
#define BUZZER_LEDC_RES LEDC_TIMER_10_BIT
#endif

static bool buzzerInitialized = false;

static void buzzerAttach() {
    if (buzzerInitialized) return;
    ledcSetup(BUZZER_LEDC_CHANNEL, 2000 /*Hz*/, 10 /*bits*/);
    ledcAttachPin(BUZZER_PIN, BUZZER_LEDC_CHANNEL);
    buzzerInitialized = true;
}

static void writeTone(int frequency) {
    buzzerAttach();
    if (frequency <= 0) {
        ledcWriteTone(BUZZER_LEDC_CHANNEL, 0);
    } else {
        ledcWriteTone(BUZZER_LEDC_CHANNEL, frequency);
    }
}

void initAudio() {
    Serial.printf("Initializing audio on pin %d\n", BUZZER_PIN);
    pinMode(BUZZER_PIN, OUTPUT);
    buzzerAttach();
    // ensure quiet
    writeTone(0);
    Serial.println("Audio initialization complete");
}

void playStartupTone() {
    Serial.println("Playing startup tone...");
    int startupNotes[] = {523, 659, 784, 1047, 1319, 1568};
    int duration = 150;
    for (unsigned i = 0; i < sizeof(startupNotes) / sizeof(startupNotes[0]); i++) {
        Serial.printf("Playing note %d Hz\n", startupNotes[i]);
        writeTone(startupNotes[i]);
        vTaskDelay(pdMS_TO_TICKS(duration + 20));
        esp_task_wdt_reset();
    }
    writeTone(0);
    Serial.println("Startup tone complete");
}

void playModeSwitchTone() {
    int switchNotes[] = {784, 988, 1175};
    int duration = 120;
    for (unsigned i = 0; i < sizeof(switchNotes) / sizeof(switchNotes[0]); i++) {
        writeTone(switchNotes[i]);
        vTaskDelay(pdMS_TO_TICKS(duration + 20));
        esp_task_wdt_reset();
    }
    writeTone(0);
}

void playAlertTone() {
    Serial.println("Playing alert tone...");
    int duration = 150; 
    int tone1 = 880;
    int tone2 = 440;
    for (int i = 0; i < 3; i++) { 
        Serial.printf("Alert tone cycle %d: %d Hz\n", i+1, tone1);
        writeTone(tone1);
        vTaskDelay(pdMS_TO_TICKS(duration));
        esp_task_wdt_reset();
        writeTone(tone2);
        vTaskDelay(pdMS_TO_TICKS(duration));
        esp_task_wdt_reset();
    }
    writeTone(tone1);
    vTaskDelay(pdMS_TO_TICKS(250));
    esp_task_wdt_reset();
    writeTone(0);
    Serial.println("Alert tone complete");
}

void playTone(int frequency, int duration) {
    writeTone(frequency);
    if (duration > 0) {
        vTaskDelay(pdMS_TO_TICKS(duration));
        writeTone(0);
    }
}

void stopTone() {
    writeTone(0);
}