#include "audio.h"

void initAudio() {
    pinMode(BUZZER_PIN, OUTPUT);
    stopTone();
}

void playStartupTone() {
    int startupNotes[] = {523, 659, 784, 1047, 1319, 1568};
    int duration = 150;
    
    for (int i = 0; i < sizeof(startupNotes) / sizeof(startupNotes[0]); i++) {
        tone(BUZZER_PIN, startupNotes[i], duration);
        delay(duration + 20);
        yield(); // Allow other tasks to run
    }
    noTone(BUZZER_PIN);
}

void playModeSwitchTone() {
    int switchNotes[] = {784, 988, 1175};
    int duration = 120;
    
    for (int i = 0; i < sizeof(switchNotes) / sizeof(switchNotes[0]); i++) {
        tone(BUZZER_PIN, switchNotes[i], duration);
        delay(duration + 20);
        yield(); // Allow other tasks to run
    }
    noTone(BUZZER_PIN);
}

void playAlertTone() {
    int duration = 150; // Reduced from 300ms to 150ms
    int tone1 = 880;
    int tone2 = 440;

    for (int i = 0; i < 3; i++) { // Reduced from 5 to 3 iterations
        tone(BUZZER_PIN, tone1, duration);
        delay(duration);
        yield(); // Allow other tasks to run
        tone(BUZZER_PIN, tone2, duration);
        delay(duration);
        yield(); // Allow other tasks to run
    }

    tone(BUZZER_PIN, tone1, 250); // Reduced from 500ms to 250ms
    delay(250);
    yield(); // Allow other tasks to run
    noTone(BUZZER_PIN);
}

void playTone(int frequency, int duration) {
    tone(BUZZER_PIN, frequency, duration);
}

void stopTone() {
    noTone(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, LOW);
}