#include "blynk_handlers.h"
#include "audio.h"
#include "display.h"
#include "oled_display.h"
#include "system.h"

void initBlynk() {
    Blynk.config(BLYNK_AUTH_TOKEN);
}

bool connectBlynk() {
    return Blynk.connect();
}

bool isBlynkConnected() {
    return Blynk.connected();
}

BLYNK_CONNECTED() {
    // Sync incoming states from app
    Blynk.syncVirtual(VPIN_DAY_NIGHT, VPIN_AC_CONTROL);
    // Push some current readings/states
    Blynk.virtualWrite(VPIN_TEMPERATURE, t);
    Blynk.virtualWrite(VPIN_HUMIDITY, h);
}

// Blynk virtual pin handlers
BLYNK_WRITE(VPIN_DAY_NIGHT) {
    isDay = param.asInt();
    
    // Queue mode switch tone for Core 0
    if (audioQueue) {
        AudioEvent modeAudio{AUDIO_MODE_SWITCH, (uint32_t)millis()};
        xQueueSend(audioQueue, &modeAudio, 0);
    }
    
    // Update both displays under I2C mutex
    if (i2cMutex && xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        displayModeStatus();  // LCD
        displayOLEDModeStatus();  // OLED
        xSemaphoreGive(i2cMutex);
    } else {
        // fallback without mutex if not yet created
        displayModeStatus();
        displayOLEDModeStatus();
    }
}

BLYNK_WRITE(VPIN_AC_CONTROL) {
    AC = param.asInt();
}

void sendSensorDataToBlynk(int temperature, int humidity, bool flame, bool motion) {
    if (Blynk.connected()) {
        Blynk.virtualWrite(VPIN_TEMPERATURE, temperature);
        Blynk.virtualWrite(VPIN_HUMIDITY, humidity);
        Blynk.virtualWrite(VPIN_FLAME, flame);
        Blynk.virtualWrite(VPIN_MOTION, motion);
    }
}