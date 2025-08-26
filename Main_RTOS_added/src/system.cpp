#include "system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 🌐 WiFi Reconnection Management Variables
unsigned long lastWiFiReconnectAttempt = 0;        // Timestamp of last reconnection attempt
const unsigned long wifiReconnectInterval = 30000;  // Reconnection interval (30 seconds)
bool wifiReconnecting = false;                     // Current reconnection status flag
bool wifiReconnectionEnabled = true;               // Enable/disable background reconnection

// External variables
extern char ssid[];
extern char pass[];
extern unsigned long startTime;

// 🚀 System Initialization
// Sets up core system components and timing
void initSystem() {
    initSerial();    // Initialize serial communication
    setStartTime();  // Start system uptime counter
}

// 📡 Serial Communication Setup
// Initializes serial port for debugging and monitoring
void initSerial() {
    Serial.begin(SERIAL_BAUD_RATE);  // Configure serial communication rate
}

// 🌐 WiFi Connection Initialization
// Establishes WiFi connection with fast-fail timeout strategy
bool initWiFi() {
    // 🔐 WiFi Credential Validation
    if (strlen(ssid) == 0 || strlen(pass) == 0) {
        Serial.println("❌ Error: WiFi credentials not configured");
        Serial.println("🔑 Please check your WiFi SSID and password");
        return false;
    }
    
    Serial.print("🌐 Attempting to connect to: ");
    Serial.println(ssid);
    
    WiFi.mode(WIFI_STA);           // Set WiFi to station mode
    WiFi.disconnect();              // Clear any previous connection attempts
    vTaskDelay(pdMS_TO_TICKS(100)); // Brief stabilization delay
    WiFi.begin(ssid, pass);         // Start connection process
    
    unsigned long wifiConnectStart = millis();  // Connection attempt start time
    Serial.print("🔄 Connecting to WiFi");
    
    // ⏱️ Fast-Fail Connection Strategy (5 second timeout)
    while (WiFi.status() != WL_CONNECTED && millis() - wifiConnectStart < 5000) {
        vTaskDelay(pdMS_TO_TICKS(500));        // 500ms connection check interval
        Serial.print(".");                      // Visual connection progress indicator
    }
    
    // ✅ Connection Success Handling
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("✅ WiFi connected successfully!");
        Serial.print("🌐 IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        // ⚠️ Connection Failure Handling
        Serial.println();
        Serial.println("⚠️ WiFi connection failed - starting in offline mode");
        Serial.println("🔄 Background reconnection will attempt every 30 seconds");
        WiFi.disconnect();  // Ensure clean connection state
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

// ⏰ System Timing Functions
void setStartTime() {
    startTime = millis();  // Record system startup timestamp
}

unsigned long getUptime() {
    return millis() - startTime;  // Calculate system uptime in milliseconds
}

// 🔄 Background WiFi Reconnection Handler
// Manages automatic WiFi reconnection without blocking system operation
void handleWiFiReconnection() {
    // 🔍 Reconnection Condition Check
    if (wifiReconnectionEnabled && 
        !isWiFiConnected() && 
        (millis() - lastWiFiReconnectAttempt > wifiReconnectInterval) &&
        !wifiReconnecting) {
        
        wifiReconnecting = true;                    // Set reconnection flag
        lastWiFiReconnectAttempt = millis();        // Update attempt timestamp
        
        Serial.println("🔄 Background: Attempting WiFi reconnection...");
        
        // 🚀 Non-blocking Reconnection Process
        WiFi.disconnect();                          // Clear current connection
        vTaskDelay(pdMS_TO_TICKS(50));             // Minimal stabilization delay
        WiFi.begin(ssid, pass);                     // Initiate new connection
        
        // ⚡ Immediate Reset for Non-blocking Operation
        wifiReconnecting = false;                   // Allow main loop to continue
        
        Serial.println("🔄 Background: WiFi reconnection initiated (non-blocking)");
    }
    
    // ✅ Reconnection Success Verification
    if (wifiReconnecting && isWiFiConnected()) {
        Serial.println("✅ Background: WiFi reconnection successful!");
        Serial.print("🌐 New IP address: ");
        Serial.println(WiFi.localIP());
        wifiReconnecting = false;                   // Clear reconnection flag
    }
}

// 🎛️ WiFi Reconnection Control Functions
void setWiFiReconnectionEnabled(bool enabled) {
    wifiReconnectionEnabled = enabled;  // Update reconnection setting
    if (!enabled) {
        Serial.println("🔄 Background WiFi reconnection disabled");
    } else {
        Serial.println("🔄 Background WiFi reconnection enabled");
    }
}

// 🚀 Manual WiFi Reconnection Trigger
void triggerWiFiReconnection() {
    if (isWiFiConnected()) {
        Serial.println("✅ WiFi already connected - no reconnection needed");
        return;
    }
    
    Serial.println("🔄 Manual WiFi reconnection triggered");
    lastWiFiReconnectAttempt = 0;  // Force immediate reconnection attempt
    wifiReconnecting = false;       // Reset reconnection state
}

// 📊 WiFi Status Information Generator
// Returns human-readable WiFi connection status with signal quality indicators
String getWiFiStatus() {
    if (wifiReconnecting) {
        return "WiFi: 🔄 Connecting...";
    } else if (isWiFiConnected()) {
        // 📶 Signal Strength Analysis
        int rssi = WiFi.RSSI();
        String signalQuality;
        if (rssi >= -50) signalQuality = "Excellent";
        else if (rssi >= -60) signalQuality = "Good";
        else if (rssi >= -70) signalQuality = "Fair";
        else signalQuality = "Poor";
        
        return "WiFi: " + signalQuality + " (" + String(rssi) + "dBm)";
    } else {
        // ⏱️ Reconnection Countdown Display
        unsigned long timeSinceLastAttempt = millis() - lastWiFiReconnectAttempt;
        if (timeSinceLastAttempt < wifiReconnectInterval) {
            unsigned long timeToNext = (wifiReconnectInterval - timeSinceLastAttempt) / 1000;
            return "WiFi: 🔄 Retry in " + String(timeToNext) + "s";
        } else {
            return "WiFi: ❌ Disconnected";
        }
    }
}