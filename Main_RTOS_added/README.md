# Smart Shop Guard - Dual Display Security System

A comprehensive IoT security system for shops using ESP32-S3 with dual display support (LCD + OLED), multiple sensors, and cloud connectivity via Blynk.

## 🏗️ Hardware Architecture

### **Microcontroller**
- **ESP32-S3-WROOM-1 N16R8**
  - Dual-core Xtensa LX7 @ 240MHz
  - 16MB Flash Memory
  - 8MB PSRAM (if detected)
  - 512KB SRAM
  - Built-in WiFi & Bluetooth

### **Display Systems**
- **16x2 LCD Display (I2C)** - Primary status display
- **1.3" OLED Display (128x64, SH1106)** - Advanced multi-page interface with graphics

### **Sensors**
- **DHT11** - Temperature and humidity monitoring
- **Flame Sensor** - Fire detection
- **PIR Motion Sensor** - Intrusion detection  
- **Ultrasonic Sensor (HC-SR04)** - Distance measurement for automatic door

### **Actuators**
- **Servo Motor** - Automatic door mechanism
- **Relay Module** - Fire suppression system control
- **Piezo Buzzer** - Audio alerts and notifications
- **Fan** - Temperature control

### **Connectivity**
- **WiFi** - Internet connectivity
- **Blynk IoT Platform** - Remote monitoring and control

## 📋 Pin Configuration

```
ESP32-S3 Pin Assignments:
├── DHT11 Sensor      → GPIO 40
├── Fan Control      → GPIO 12  
├── Flame Sensor     → GPIO 4
├── Buzzer           → GPIO 17
├── Relay            → GPIO 1
├── PIR Motion       → GPIO 5
├── Servo Motor      → GPIO 7
├── Ultrasonic TRIG  → GPIO 42
├── Ultrasonic ECHO  → GPIO 41
├── LCD (I2C)        → SDA: GPIO 21, SCL: GPIO 22
└── OLED (I2C)       → SDA: GPIO 8,  SCL: GPIO 9
```

## 🏛️ Software Architecture

### **Modular Design**
The project follows a clean modular architecture with separated concerns:

```
BASIC/
├── src/
│   ├── main.cpp           # Main application logic
│   ├── sensors.cpp        # Sensor reading and management
│   ├── actuators.cpp      # Motor, relay, and fan control
│   ├── display.cpp        # LCD display functions
│   ├── oled_display.cpp   # OLED display with multi-page interface
│   ├── audio.cpp          # Buzzer and sound management
│   ├── system.cpp         # WiFi and system utilities
│   └── blynk_handlers.cpp # IoT cloud communication
├── include/
│   ├── config.h           # Hardware configuration and pin definitions
│   ├── system.h           # System function declarations
│   ├── sensors.h          # Sensor function declarations
│   ├── actuators.h        # Actuator function declarations
│   ├── display.h          # LCD display function declarations
│   ├── oled_display.h     # OLED display function declarations
│   ├── audio.h            # Audio function declarations
│   ├── icons.h            # OLED graphics and icons
│   └── blynk_handlers.h   # Blynk function declarations
└── lib/                   # External libraries
    ├── ESP8266_and_ESP32_OLED_driver_for_SSD1306_displays/
    ├── Blynk/
    ├── LiquidCrystal_I2C/
    ├── ESP32Servo/
    └── DHT11/
```

### **Key Features**

#### **1. Dual Display System**
- **LCD (16x2)**: Real-time status, temperature, alerts
- **OLED (128x64)**: Multi-page interface with graphics
  - Status Page: System overview with icons
  - Sensor Page: Detailed sensor readings
  - System Page: WiFi, uptime, memory info
  - Alerts Page: Active alerts and warnings
  - Settings Page: Configuration options

#### **2. Multi-Sensor Monitoring**
- **Environmental**: Temperature, humidity with automatic fan control
- **Security**: Motion detection, fire detection
- **Access Control**: Ultrasonic distance-based automatic door

#### **3. Smart Alert System**
- **Visual**: LCD messages, OLED animations, LED indicators
- **Audio**: Different tones for different alert types
- **Remote**: Blynk notifications to mobile devices

#### **4. IoT Connectivity**
- **Real-time monitoring** via Blynk mobile app
- **Remote control** of AC and system modes
- **Cloud data logging** for analysis

## 🔧 Implementation Details

### **Power Management**
- **Watchdog Timer**: 10-second timeout with regular resets
- **Progressive Initialization**: Staged startup to prevent power issues
- **Power Supply Compatibility**: Works with MB102 breadboard power supply
- **Voltage Monitoring**: System diagnostics and power status

### **Display Synchronization**
Both displays are kept perfectly synchronized:
- **Immediate Updates**: Critical alerts update both displays instantly
- **Background Refresh**: Regular updates for non-critical information
- **State Management**: Shared state variables ensure consistency

### **Sensor Integration**
- **Non-blocking Reading**: Sensors read without blocking main loop
- **Debouncing**: Motion and button inputs properly debounced
- **Calibration**: Temperature and humidity with configurable thresholds
- **Error Handling**: Graceful degradation if sensors fail

### **Communication Protocols**
- **I2C**: LCD and OLED displays on separate I2C buses
- **Digital I/O**: Sensors and actuators on dedicated GPIO pins
- **PWM**: Servo motor control with precise positioning
- **WiFi**: 2.4GHz connectivity with automatic reconnection

## 🚀 Getting Started

### **Prerequisites**
- PlatformIO IDE or Arduino IDE with ESP32 support
- ESP32-S3 board with USB-C cable
- All hardware components as listed above
- WiFi network credentials
- Blynk account and auth token

### **Installation**

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd Smart-Shop-Guard-project/BASIC
   ```

2. **Configure WiFi and Blynk**
   Edit `include/config.h`:
   ```cpp
   #define WIFI_SSID "Your_WiFi_Name"
   #define WIFI_PASSWORD "Your_WiFi_Password"
   #define BLYNK_AUTH_TOKEN "Your_Blynk_Token"
   ```

3. **Build and Upload**
   ```bash
   pio run -t upload
   ```

4. **Monitor Serial Output**
   ```bash
   pio device monitor
   ```

### **Expected Startup Sequence**
```
Initializing displays...
Initializing sensors...
Connecting to WiFi...
WiFi connected, initializing Blynk...
=== System Information ===
Free heap: 294896 bytes
CPU frequency: 240 MHz
Flash size: 16777216 bytes
PSRAM size: 8388608 bytes (or 0 if not detected)
PSRAM found: YES/NO
PSRAM support compiled in
Setup complete!
```

## 🎛️ Operation Modes

### **Day Mode**
- Motion detection active
- Full sensor monitoring
- Automatic door operation
- Theft alert system enabled

### **Night Mode**
- Enhanced security monitoring
- Fire detection priority
- Reduced automatic operations
- Silent mode options

### **Manual Override**
- Blynk app control
- Emergency stop functions
- System diagnostics mode

## 📱 Mobile App Integration

The system integrates with Blynk IoT platform:

### **Virtual Pins**
- **V0**: Temperature reading
- **V1**: Humidity reading  
- **V3**: Motion detection status
- **V4**: Fire detection status
- **V5**: Day/Night mode toggle
- **V6**: AC control

### **Features**
- Real-time sensor monitoring
- Remote system control
- Push notifications for alerts
- Historical data logging
- System status dashboard

## 🔧 Troubleshooting

### **Common Issues**

#### **PSRAM Not Detected**
- PSRAM showing 0 bytes is common and doesn't affect functionality
- The system works perfectly with built-in 512KB RAM
- PSRAM is optional for this application

#### **WiFi Connection Issues**
- Check credentials in `config.h`
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)
- Check signal strength and router compatibility

#### **Power Supply Issues**
- ESP32-S3 requires stable 3.3V with 800mA+ current capability
- MB102 breadboard supply may cause restarts under load
- Consider adding 1000µF capacitor for power stability

#### **Display Issues**
- Check I2C connections and addresses
- LCD default address: 0x27
- OLED default address: 0x3C
- Ensure proper 3.3V/5V levels

#### **Sensor Reading Errors**
- Verify pin connections match `config.h`
- Check power supply to sensors
- Ensure proper pull-up resistors where needed

### **Debugging**
- Serial monitor at 9600 baud shows detailed diagnostics
- System information displayed at startup
- Error messages for failed initializations
- Watchdog resets logged with reasons

## 🔮 Future Enhancements

### **Planned Features**
- **Camera Integration**: ESP32-CAM module support
- **SD Card Logging**: Local data storage
- **Web Interface**: Browser-based control panel
- **Email Notifications**: SMTP alert system
- **Voice Alerts**: Text-to-speech integration
- **Mobile Notifications**: Push alerts
- **Data Analytics**: Usage pattern analysis

### **Hardware Expansions**
- **Additional Sensors**: Gas, smoke, water leak detection
- **More Actuators**: Multiple servo doors, lighting control
- **Battery Backup**: UPS functionality
- **External Antenna**: Extended WiFi range
- **LoRa Module**: Long-range communication

## 📄 License

This project is open-source and available under the MIT License.

**Smart Shop Guard** - Protecting your business with intelligent IoT technology! 🛡️🏪
