#ifndef AUDIO_H
#define AUDIO_H

#include "config.h"

// Audio initialization
void initAudio();

// Tone functions
void playStartupTone();
void playModeSwitchTone();
void playAlertTone();

// Basic tone control
void playTone(int frequency, int duration);
void stopTone();

#endif // AUDIO_H