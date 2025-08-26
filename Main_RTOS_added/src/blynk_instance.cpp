#include "blynk_instance.h"

// Define the actual Blynk objects here (only once in the entire project)
WiFiClient _blynkWifiClient;
BlynkEsp32Client _blynkTransport(_blynkWifiClient);
BlynkWifi Blynk(_blynkTransport);