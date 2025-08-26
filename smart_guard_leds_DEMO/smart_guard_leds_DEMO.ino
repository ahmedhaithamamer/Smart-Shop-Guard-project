// 🛡️ Smart Shop Guard - LED Effects System
// Enhanced NeoPixel effects for security monitoring visualization
// Compatible with ESP32-S3 and Arduino boards

#include <Adafruit_NeoPixel.h>

// 🔧 Hardware Configuration
#define LED_PIN        D4        // GPIO pin for NeoPixel strip (ESP32-S3 compatible)
#define NUM_PIXELS     31       // Number of NeoPixels in the strip
#define BRIGHTNESS     100       // Global brightness (0-255)

// ⏱️ Timing Configuration
#define BOOT_DELAY     100      // Boot animation speed
#define EFFECT_DELAY   50       // General effect speed
#define STABLE_DELAY   2000     // Stable mode duration

// 🎨 Color Definitions (RGB values)
#define COLOR_OFF      0, 0, 0
#define COLOR_BLUE     0, 100, 255    // Security/Normal
#define COLOR_GREEN    0, 255, 100    // Safe/OK status
#define COLOR_ORANGE   255, 165, 0    // Warning/Caution
#define COLOR_RED      255, 0, 0    // Alert/Danger
#define COLOR_PURPLE   128, 0, 128    // System/Boot
#define COLOR_WHITE    255, 255, 255  // Bright/Active
#define COLOR_CYAN     0, 255, 255    // Info/Data

// Initialize NeoPixel strip
Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// 🔄 System State Variables
enum SystemMode {
  BOOT_MODE,
  SECURE_MODE,
  ALERT_MODE,
  SCAN_MODE,
  DEMO_MODE
};

SystemMode currentMode = BOOT_MODE;
unsigned long lastModeChange = 0;
unsigned long modeInterval = 10000; // Change mode every 10 seconds for demo

void setup() {
  Serial.begin(115200);
  Serial.println("🛡️ Smart Shop Guard - LED Effects System Starting...");
  
  // Initialize NeoPixel strip
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show(); // Initialize all pixels to 'off'
  
  // Boot sequence
  bootSequence();
  
  Serial.println("✅ LED System Ready!");
}

void loop() {
  // Demo mode - cycle through different effects
  if (millis() - lastModeChange > modeInterval) {
    currentMode = (SystemMode)((currentMode + 1) % 5);
    lastModeChange = millis();
    
    // Print current mode
    switch(currentMode) {
      case BOOT_MODE: Serial.println("🚀 Mode: BOOT"); break;
      case SECURE_MODE: Serial.println("🛡️ Mode: SECURE"); break;
      case ALERT_MODE: Serial.println("🚨 Mode: ALERT"); break;
      case SCAN_MODE: Serial.println("🔍 Mode: SCAN"); break;
      case DEMO_MODE: Serial.println("✨ Mode: DEMO"); break;
    }
  }
  
  // Execute current mode effects
  switch(currentMode) {
    case BOOT_MODE:
      bootSequence();
      break;
    case SECURE_MODE:
      secureMode();
      break;
    case ALERT_MODE:
      alertMode();
      break;
    case SCAN_MODE:
      scanMode();
      break;
    case DEMO_MODE:
      demoMode();
      break;
  }
}

// 🚀 Boot Sequence - Smart Shop Guard startup animation
void bootSequence() {
  Serial.println("🚀 Boot Sequence Starting...");
  
  // Phase 1: Purple wave - System initialization
  for(int wave = 0; wave < 3; wave++) {
    for(int i = 0; i < NUM_PIXELS; i++) {
      strip.clear();
      // Create a moving wave of purple
      for(int j = 0; j < 5; j++) {
        int pos = (i + j) % NUM_PIXELS;
        int brightness = 255 - (j * 50);
        strip.setPixelColor(pos, brightness/2, 0, brightness);
      }
      strip.show();
      delay(BOOT_DELAY);
    }
  }
  
  // Phase 2: Blue pulse - Security systems online
  for(int pulse = 0; pulse < 4; pulse++) {
    for(int brightness = 0; brightness <= 255; brightness += 15) {
      for(int i = 0; i < NUM_PIXELS; i++) {
        strip.setPixelColor(i, 0, brightness/3, brightness);
      }
      strip.show();
      delay(20);
    }
    for(int brightness = 255; brightness >= 0; brightness -= 15) {
      for(int i = 0; i < NUM_PIXELS; i++) {
        strip.setPixelColor(i, 0, brightness/3, brightness);
      }
      strip.show();
      delay(20);
    }
  }
  
  // Phase 3: Green confirmation - All systems ready
  for(int i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, COLOR_GREEN);
    strip.show();
    delay(BOOT_DELAY/2);
  }
  delay(1000);
  
  // Fade to stable blue
  fadeToColor(COLOR_BLUE, 50);
}

// 🛡️ Secure Mode - Stable operation with gentle breathing
void secureMode() {
  static unsigned long lastBreath = 0;
  static int breathDirection = 1;
  static int breathLevel = 100;
  
  if (millis() - lastBreath > 30) {
    breathLevel += breathDirection * 3;
    
    if (breathLevel >= 200) {
      breathDirection = -1;
    } else if (breathLevel <= 50) {
      breathDirection = 1;
    }
    
    // Apply breathing effect to all pixels
    for(int i = 0; i < NUM_PIXELS; i++) {
      strip.setPixelColor(i, 0, breathLevel/4, breathLevel);
    }
    strip.show();
    lastBreath = millis();
  }
}

// 🚨 Alert Mode - Rapid red flashing with siren pattern
void alertMode() {
  static unsigned long lastAlert = 0;
  static bool alertState = false;
  static int alertPhase = 0;
  
  if (millis() - lastAlert > 200) {
    alertState = !alertState;
    alertPhase = (alertPhase + 1) % 4;
    
    if (alertState) {
      // Create siren-like sweeping effect
      strip.clear();
      for(int i = 0; i < NUM_PIXELS/2; i++) {
        int pos1 = (alertPhase * NUM_PIXELS/4 + i) % NUM_PIXELS;
        int pos2 = (alertPhase * NUM_PIXELS/4 - i + NUM_PIXELS) % NUM_PIXELS;
        strip.setPixelColor(pos1, COLOR_RED);
        strip.setPixelColor(pos2, COLOR_RED);
      }
    } else {
      strip.clear();
    }
    
    strip.show();
    lastAlert = millis();
  }
}

// 🔍 Scan Mode - Rotating scanner like security camera
void scanMode() {
  static unsigned long lastScan = 0;
  static int scanPos = 0;
  static int scanDirection = 1;
  
  if (millis() - lastScan > EFFECT_DELAY) {
    strip.clear();
    
    // Create scanning beam
    for(int i = 0; i < 7; i++) {
      int pos = (scanPos + i) % NUM_PIXELS;
      int intensity = 255 - (i * 35);
      strip.setPixelColor(pos, 0, intensity, intensity);
    }
    
    // Add trailing dots
    for(int i = 8; i < 12; i++) {
      int pos = (scanPos + i) % NUM_PIXELS;
      strip.setPixelColor(pos, 0, 30, 100);
    }
    
    scanPos += scanDirection;
    if (scanPos >= NUM_PIXELS || scanPos < 0) {
      scanDirection *= -1;
      scanPos += scanDirection;
    }
    
    strip.show();
    lastScan = millis();
  }
}

// ✨ Demo Mode - Rainbow security pattern
void demoMode() {
  static unsigned long lastDemo = 0;
  static int rainbowOffset = 0;
  
  if (millis() - lastDemo > 50) {
    for(int i = 0; i < NUM_PIXELS; i++) {
      int hue = (i * 256 / NUM_PIXELS + rainbowOffset) % 256;
      uint32_t color = strip.gamma32(strip.ColorHSV(hue * 256));
      strip.setPixelColor(i, color);
    }
    
    rainbowOffset += 2;
    if (rainbowOffset >= 256) rainbowOffset = 0;
    
    strip.show();
    lastDemo = millis();
  }
}

// 🎨 Utility function to fade all pixels to a specific color
void fadeToColor(uint8_t r, uint8_t g, uint8_t b, int steps) {
  for(int step = 0; step < steps; step++) {
    for(int i = 0; i < NUM_PIXELS; i++) {
      uint32_t currentColor = strip.getPixelColor(i);
      uint8_t currentR = (currentColor >> 16) & 0xFF;
      uint8_t currentG = (currentColor >> 8) & 0xFF;
      uint8_t currentB = currentColor & 0xFF;
      
      uint8_t newR = currentR + ((r - currentR) * step / steps);
      uint8_t newG = currentG + ((g - currentG) * step / steps);
      uint8_t newB = currentB + ((b - currentB) * step / steps);
      
      strip.setPixelColor(i, newR, newG, newB);
    }
    strip.show();
    delay(20);
  }
}

// 🔄 Clear all pixels
void clearAll() {
  strip.clear();
  strip.show();
}

// 💡 Set all pixels to the same color
void setAllColor(uint8_t r, uint8_t g, uint8_t b) {
  for(int i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
}