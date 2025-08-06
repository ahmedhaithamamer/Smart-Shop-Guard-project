#ifndef BLYNK_INSTANCE_H
#define BLYNK_INSTANCE_H

// Prevent BlynkSimpleEsp32.h from creating its own global instances
#define NO_GLOBAL_INSTANCES
#define NO_GLOBAL_BLYNK

#include <BlynkSimpleEsp32.h>

// Declare the global Blynk object as extern
extern WiFiClient _blynkWifiClient;
extern BlynkEsp32Client _blynkTransport;
extern BlynkWifi Blynk;

#endif // BLYNK_INSTANCE_H