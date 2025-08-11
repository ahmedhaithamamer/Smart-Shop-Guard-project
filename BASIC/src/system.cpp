#include "system.h"
#include "esp_task_wdt.h"

// Global variables for WiFi reconnection
unsigned long lastWiFiReconnectAttempt = 0;
const unsigned long wifiReconnectInterval = 30000; // 30 seconds
bool wifiReconnecting = false;
bool wifiReconnectionEnabled = true;

// External variables
extern char ssid[];
extern char pass[];
extern unsigned long startTime;

void initSystem() {
    initSerial();
    setStartTime();
}

void initSerial() {
    Serial.begin(SERIAL_BAUD_RATE);
}

bool initWiFi() {
    // Validate WiFi credentials
    if (strlen(ssid) == 0 || strlen(pass) == 0) {
        Serial.println("Error: WiFi credentials not configured");
        Serial.println("Please check your WiFi SSID and password");
        return false;
    }
    
    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(); // Clear any previous connection attempts
    delay(100);
    WiFi.begin(ssid, pass);
    
    unsigned long wifiConnectStart = millis();
    Serial.print("Connecting to WiFi");
    
    // Try for 5 seconds maximum - no retries, just fail fast
    while (WiFi.status() != WL_CONNECTED && millis() - wifiConnectStart < 5000) {
        delay(500);
        Serial.print(".");
        yield(); // Allow other tasks to run
        esp_task_wdt_reset(); // Reset watchdog during WiFi connection
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("WiFi connected successfully!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        Serial.println();
        Serial.println("WiFi connection failed - starting in offline mode");
        Serial.println("Background reconnection will attempt every 30 seconds");
        WiFi.disconnect(); // Ensure clean state
        return false;
    }
}

bool isWiFiConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

void printWiFiStatus() {
    if (isWiFiConnected()) {
        Serial.print("WiFi connected to: ");
        Serial.println(WiFi.SSID());
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Signal strength: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
        Serial.print("Channel: ");
        Serial.println(WiFi.channel());
    } else {
        Serial.println("WiFi not connected");
        if (wifiReconnectionEnabled) {
            unsigned long timeSinceLastAttempt = millis() - lastWiFiReconnectAttempt;
            if (timeSinceLastAttempt < wifiReconnectInterval) {
                unsigned long timeToNext = (wifiReconnectInterval - timeSinceLastAttempt) / 1000;
                Serial.print("Next reconnection attempt in: ");
                Serial.print(timeToNext);
                Serial.println(" seconds");
            } else {
                Serial.println("Ready for reconnection attempt");
            }
        } else {
            Serial.println("Background reconnection is disabled");
        }
    }
}

void setStartTime() {
    startTime = millis();
}

unsigned long getUptime() {
    return millis() - startTime;
}

void handleWiFiReconnection() {
    // Only try reconnection if enabled, not currently connected, and enough time has passed
    if (wifiReconnectionEnabled && 
        !isWiFiConnected() && 
        (millis() - lastWiFiReconnectAttempt > wifiReconnectInterval) &&
        !wifiReconnecting) {
        
        wifiReconnecting = true;
        lastWiFiReconnectAttempt = millis();
        
        Serial.println("Background: Attempting WiFi reconnection...");
        
        // Non-blocking reconnection attempt
        WiFi.disconnect();
        delay(50); // Minimal delay
        WiFi.begin(ssid, pass);
        
        // Set a flag to check connection status in next iterations
        wifiReconnecting = false; // Reset immediately to allow main loop to continue
        
        Serial.println("Background: WiFi reconnection initiated (non-blocking)");
    }
    
    // Check if reconnection was successful (non-blocking check)
    if (wifiReconnecting && isWiFiConnected()) {
        Serial.println("Background: WiFi reconnection successful!");
        Serial.print("New IP address: ");
        Serial.println(WiFi.localIP());
        wifiReconnecting = false;
    }
}

// Function to enable/disable background reconnection
void setWiFiReconnectionEnabled(bool enabled) {
    wifiReconnectionEnabled = enabled;
    if (!enabled) {
        Serial.println("Background WiFi reconnection disabled");
    } else {
        Serial.println("Background WiFi reconnection enabled");
    }
}

// Function to manually trigger WiFi reconnection
void triggerWiFiReconnection() {
    if (isWiFiConnected()) {
        Serial.println("WiFi already connected - no reconnection needed");
        return;
    }
    
    Serial.println("Manual WiFi reconnection triggered");
    lastWiFiReconnectAttempt = 0; // Force immediate reconnection
    wifiReconnecting = false; // Reset state
}

String getWiFiStatus() {
    if (wifiReconnecting) {
        return "WiFi: Connecting...";
    } else if (isWiFiConnected()) {
        // Get signal strength
        int rssi = WiFi.RSSI();
        String signalQuality;
        if (rssi >= -50) signalQuality = "Excellent";
        else if (rssi >= -60) signalQuality = "Good";
        else if (rssi >= -70) signalQuality = "Fair";
        else signalQuality = "Poor";
        
        return "WiFi: " + signalQuality + " (" + String(rssi) + "dBm)";
    } else {
        // Show countdown to next attempt
        unsigned long timeSinceLastAttempt = millis() - lastWiFiReconnectAttempt;
        if (timeSinceLastAttempt < wifiReconnectInterval) {
            unsigned long timeToNext = (wifiReconnectInterval - timeSinceLastAttempt) / 1000;
            return "WiFi: Retry in " + String(timeToNext) + "s";
        } else {
            return "WiFi: Disconnected";
        }
    }
}