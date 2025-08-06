#include "blynk_handlers.h"
#include "audio.h"
#include "display.h"

void initBlynk() {
    Blynk.config(BLYNK_AUTH_TOKEN);
}

bool connectBlynk() {
    return Blynk.connect();
}

bool isBlynkConnected() {
    return Blynk.connected();
}

// Blynk virtual pin handlers
BLYNK_WRITE(VPIN_DAY_NIGHT) {
    isDay = param.asInt();
    playModeSwitchTone();
    displayModeStatus();
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