# 🖥️ OLED Display Demo - Smart Shop Guard

## 📖 Overview

This folder contains a standalone demonstration of the OLED display system used in the Smart Shop Guard project. It showcases the multi-page navigation interface, button controls, and various display themes that are integrated into the main system.

## 🎯 Purpose

- **Display Testing** - Test OLED functionality before integration
- **UI Development** - Develop and refine user interface elements
- **Feature Demonstration** - Showcase display capabilities
- **Educational Tool** - Learn OLED programming techniques

## 📁 Contents

```
OLED_DEMO/
├── OLED.ino           # Main demonstration code
└── icons.h            # Icon definitions and graphics (referenced)
```

## 🖥️ Hardware Requirements

### 📺 Display
- **1.3" OLED Display** (128x64 pixels, SH1106 controller)
- **I2C Interface** (SDA, SCL connections)

### 🎛️ Controls
- **2 Push Buttons** for navigation
- **Pull-up resistors** (10kΩ recommended)

### 🔌 Microcontroller
- **ESP8266** (as configured) or **ESP32**
- **WiFi capability** for connectivity features

## 📌 Pin Configuration

```cpp
// Button Definitions (ESP8266)
#define BUTTON_NEXT D5    // Next/Forward button
#define BUTTON_PREV D6    // Previous/Back button

// OLED Display (I2C)
SH1106Wire display(0x3C, D2, D1);  // Address, SDA, SCL
```

## 🚀 Quick Start

### 📋 Prerequisites

- **Arduino IDE** with ESP8266 support
- **Required Libraries:**
  - ESP8266WiFi
  - SH1106Wire (or compatible OLED library)
  - Wire (built-in)

### 🔧 Installation

1. **Install Libraries:**
   ```
   Tools → Manage Libraries → Search and install:
   - ESP8266 OLED Driver for SSD1306/SH1106
   - ESP8266WiFi (built-in)
   ```

2. **Configure WiFi:**
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```

3. **Upload Code:**
   - Select Board: ESP8266 (NodeMCU, Wemos D1, etc.)
   - Select correct COM port
   - Upload OLED.ino

## 🎨 Features Demonstrated

### 📄 Multi-Page Interface

The demo showcases 5 different pages:

1. **Status Page** - System overview with animated icons
2. **Sensor Page** - Sensor readings with activity indicators
3. **System Page** - System information and diagnostics
4. **Alerts Page** - Security and fire alert displays
5. **Settings Page** - Configuration options

### 🎛️ Navigation Controls

- **NEXT Button** - Navigate forward through pages
- **PREV Button** - Navigate backward through pages
- **Both Buttons** - Toggle auto-swipe mode (hold 1 second)
- **Long Press NEXT** - Enter settings menu (on settings page)

### 🎭 Display Themes

- **Normal Theme** - Standard black on white display
- **Invert Theme** - White on black display
- **Theme Toggle** - Available in settings menu

### ⚡ Auto-Swipe Mode

- **Automatic Navigation** - Pages change every 15 seconds
- **Visual Indicator** - Dot (●) appears in page counter when active
- **Manual Override** - Any button press resets timer
- **Toggle Control** - Both buttons held for 1 second

## 🎨 Visual Elements

### 🖼️ Icons and Graphics

The demo uses various icons to enhance the visual experience:

- **Shield Icon** - Security status
- **Fire Icon** - Fire detection alerts
- **Temperature Icon** - Climate monitoring
- **System Icon** - System information
- **Settings Icon** - Configuration access
- **Alert Icons** - Various warning states

### ✨ Animations

- **Flashing Alerts** - Alternating icons for emergency states
- **Activity Indicators** - Animated dots showing sensor activity
- **Breathing Effect** - Subtle animations for normal operation
- **Loading Animations** - Boot sequence with progress bars

## 🔧 Code Structure

### 📋 Main Components

```cpp
// Configuration
struct Config {
  int theme;           // Display theme (normal/invert)
  bool auto_swipe;     // Auto-navigation enabled
} config;

// Page Management
enum PageType {
  PAGE_STATUS = 0,     // System status overview
  PAGE_SENSORS = 1,    // Sensor readings
  PAGE_SYSTEM = 2,     // System information
  PAGE_ALERTS = 3,     // Alert status
  PAGE_SETTINGS = 4    // Settings menu
};

// Sensor Data (Simulated)
struct SensorData {
  float temperature;   // Temperature reading
  float humidity;      // Humidity reading
  bool fireDetected;   // Fire alert state
  bool motionDetected; // Motion alert state
  bool doorOpen;       // Door status
  unsigned long uptime; // System uptime
  int freeHeap;        // Available memory
} sensors;
```

### 🔄 Main Functions

- **`setup()`** - Initialize display, WiFi, and show intro animation
- **`loop()`** - Handle buttons, auto-swipe, animations, and display updates
- **`handleButtons()`** - Process button inputs and navigation
- **`updateDisplay()`** - Refresh display content
- **`showXXXPage()`** - Render specific page content

## 🎬 Startup Animation

The demo includes an impressive startup sequence:

1. **Logo Animation** - "SMART SHOP GUARD" appears letter by letter
2. **Loading Bar** - Progress bar with animated dots
3. **System Ready** - Confirmation message
4. **WiFi Connection** - Connection status with animated dots
5. **Fade Transition** - Smooth transition to main interface

## 🔧 Customization Options

### 🎨 Visual Customization

```cpp
// Timing Configuration
const unsigned long autoSwipeDelay = 15000;    // Auto-swipe interval
const unsigned long debounceDelay = 300;       // Button debounce time
const unsigned long longPressDelay = 1000;     // Long press detection

// Display Configuration
#define OLED_ADDRESS 0x3C                       // I2C address
```

### 📊 Data Simulation

The demo includes simulated sensor data that can be replaced with real sensor readings:

```cpp
void updateSensorData() {
  sensors.uptime = millis() / 1000;
  sensors.freeHeap = ESP.getFreeHeap();
  // Add real sensor reading code here
}
```

## 🔍 Integration with Main System

This demo code is designed to be easily integrated into the main Smart Shop Guard system:

1. **Copy Functions** - Display functions can be directly copied
2. **Adapt Data Sources** - Replace simulated data with real sensor readings
3. **Modify Navigation** - Adjust button handling for system requirements
4. **Extend Pages** - Add additional pages for new features

## 🛠️ Troubleshooting

### 🔌 Display Issues

**No Display Output:**
- Check I2C connections (SDA, SCL)
- Verify OLED address (0x3C is standard)
- Ensure proper power supply (3.3V)

**Garbled Display:**
- Check for loose connections
- Verify library compatibility
- Try different I2C pins

### 🎛️ Button Issues

**Buttons Not Responding:**
- Check pull-up resistors (10kΩ)
- Verify pin assignments
- Monitor serial output for debug info

**Erratic Button Behavior:**
- Increase debounce delay
- Check for electrical interference
- Verify button wiring

### 📶 WiFi Issues

**Connection Failures:**
- Verify SSID and password
- Check 2.4GHz network availability
- Monitor serial output for connection status

## 📈 Performance Notes

- **Refresh Rate** - Display updates every 50ms for smooth animation
- **Memory Usage** - Optimized for ESP8266 limited memory
- **Power Consumption** - OLED displays are power-efficient
- **Response Time** - Button inputs processed with minimal latency

## 🔄 Future Enhancements

Potential improvements for the demo:

- **Touch Interface** - Add capacitive touch support
- **Color Display** - Adapt for color OLED displays
- **Web Interface** - Add web-based configuration
- **Data Logging** - Store display metrics
- **Custom Fonts** - Add additional font options

## 📞 Support

For OLED demo specific issues:
- Check serial monitor output at 115200 baud
- Verify all library dependencies
- Test with basic OLED examples first
- Compare pin assignments with your hardware

---

This OLED demo provides a comprehensive example of advanced display programming techniques and serves as an excellent foundation for developing sophisticated user interfaces in embedded systems.
