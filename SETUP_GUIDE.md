# 🚀 Smart Shop Guard - Complete Setup Guide

## 📖 Overview

This comprehensive guide will walk you through the complete setup process for the Smart Shop Guard system, from hardware assembly to software deployment and cloud configuration.

## 🎯 Setup Options

Choose the implementation that best fits your needs:

| Implementation | Complexity | Features | Recommended For |
|---------------|------------|----------|-----------------|
| **Main_RTOS_ADDED** | Advanced | Full FreeRTOS, Multi-core | Production, Advanced Users |
| **Smart_Shop_Guard_Arduino** | Intermediate | Complete functionality | Learning, Prototyping |
| **Individual Demos** | Basic | Specific components | Testing, Development |

## 🛠️ Hardware Setup

### 📦 Required Components

#### 🖥️ Core Components
- **ESP32-S3-WROOM-1 N16R8** development board
- **Breadboard** or **PCB** for connections
- **Jumper wires** (male-to-male, male-to-female)
- **Power supply** (5V, 3A recommended)

#### 📺 Display Systems
- **16x2 LCD Display with I2C backpack** (Address: 0x27)
- **1.3" OLED Display 128x64** (SH1106 controller, Address: 0x3C)

#### 🔍 Sensors
- **DHT11** Temperature & Humidity Sensor
- **Flame Sensor** (IR flame detection)
- **PIR Motion Sensor** (HC-SR501 or similar)
- **Ultrasonic Sensor** (HC-SR04)

#### ⚡ Actuators & Output
- **Servo Motor** (SG90 or MG996R for higher torque)
- **Relay Module** (5V, optically isolated)
- **Piezo Buzzer** (active or passive)
- **12V Fan** (with appropriate driver circuit)

#### 🔧 Additional Components
- **Resistors:** 10kΩ pull-up resistors for buttons
- **Capacitors:** 1000µF for power supply stabilization
- **Transistor:** NPN (2N2222) for fan control
- **Diode:** 1N4007 for relay protection

### 🔌 Wiring Diagram

```
ESP32-S3 Pin Assignments:
┌─────────────────────────────────────────┐
│  Component          │  ESP32-S3 Pin    │
├─────────────────────────────────────────┤
│  DHT11 Data         │  GPIO 40         │
│  Fan Control        │  GPIO 12         │
│  Flame Sensor       │  GPIO 4          │
│  Buzzer             │  GPIO 17         │
│  Relay Control      │  GPIO 1          │
│  PIR Motion         │  GPIO 5          │
│  Servo Signal       │  GPIO 7          │
│  Ultrasonic Trig    │  GPIO 42         │
│  Ultrasonic Echo    │  GPIO 41         │
│  LCD SDA            │  GPIO 21         │
│  LCD SCL            │  GPIO 22         │
│  OLED SDA           │  GPIO 8          │
│  OLED SCL           │  GPIO 9          │
│  Button Next        │  GPIO 47         │
│  Button Prev        │  GPIO 48         │
│  LED Strip          │  GPIO 20         │
│  Loud Buzzer        │  GPIO 19         │
└─────────────────────────────────────────┘
```

### ⚠️ Important Wiring Notes

1. **Power Distribution:**
   - ESP32-S3: 3.3V from development board
   - Sensors: Most require 5V (check specifications)
   - Servo: Requires 5V with adequate current (1-2A)
   - Relay: 5V for coil, handle AC/DC switching loads safely

2. **I2C Connections:**
   - **LCD:** Uses default I2C (SDA=21, SCL=22)
   - **OLED:** Uses custom I2C (SDA=8, SCL=9)
   - **Pull-up resistors:** 4.7kΩ on SDA and SCL lines

3. **Safety Considerations:**
   - **Relay isolation:** Never connect AC mains directly to microcontroller
   - **Current limits:** Use appropriate fuses and current limiting
   - **Ground connections:** Ensure common ground for all components

## 💻 Software Setup

### 🔧 Development Environment

#### Option 1: PlatformIO (Recommended for Main_RTOS_ADDED)

1. **Install PlatformIO:**
   - Download and install [PlatformIO IDE](https://platformio.org/install/ide)
   - Or install PlatformIO extension in VS Code

2. **Clone Repository:**
   ```bash
   git clone https://github.com/your-username/Smart-Shop-Guard-project.git
   cd Smart-Shop-Guard-project/Main_RTOS_ADDED
   ```

3. **Install Dependencies:**
   ```bash
   pio lib install
   ```

#### Option 2: Arduino IDE (For Arduino version)

1. **Install Arduino IDE:**
   - Download from [Arduino.cc](https://www.arduino.cc/en/software)
   - Install version 2.0 or later

2. **Add ESP32 Support:**
   - File → Preferences
   - Additional Boards Manager URLs:
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Tools → Board → Boards Manager
   - Search "ESP32" and install

3. **Install Required Libraries:**
   ```
   - WiFi (built-in)
   - BlynkSimpleEsp32
   - LiquidCrystal_I2C
   - SH1106Wire (or ESP8266 OLED Driver)
   - DHT sensor library
   - ESP32Servo
   - Adafruit_NeoPixel (if using LEDs)
   ```

### 🔐 Credentials Configuration

#### For Main_RTOS_ADDED (PlatformIO):

1. **Copy Template:**
   ```bash
   cd Main_RTOS_ADDED
   cp credentials_template.h credentials.h
   ```

2. **Edit Credentials:**
   ```cpp
   // credentials.h
   #define BLYNK_TEMPLATE_ID "YOUR_BLYNK_TEMPLATE_ID"
   #define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"
   #define WIFI_SSID "YOUR_WIFI_NETWORK"
   #define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
   ```

#### For Arduino Version:

Edit the configuration section in `Smart_Shop_Guard_Arduino.ino`:
```cpp
#define BLYNK_TEMPLATE_ID "YOUR_BLYNK_TEMPLATE_ID"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"
#define WIFI_SSID "YOUR_WIFI_NETWORK"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
```

## 📱 Blynk Cloud Setup

### 1. Create Blynk Account

1. Download **Blynk IoT** app from App Store/Google Play
2. Create account at [blynk.cloud](https://blynk.cloud)
3. Create new template for "Smart Shop Guard"

### 2. Configure Template

**Template Settings:**
- **Name:** Smart Shop Guard System
- **Hardware:** ESP32
- **Connection Type:** WiFi

**Datastreams:**
```
V0 - Temperature (°C)    - Virtual Pin, Double, 0-50
V1 - Humidity (%)        - Virtual Pin, Double, 0-100
V3 - Motion Status       - Virtual Pin, Integer, 0-1
V4 - Fire Status         - Virtual Pin, Integer, 0-1
V5 - Day/Night Mode      - Virtual Pin, Integer, 0-1
V6 - AC Control          - Virtual Pin, Integer, 0-1
```

### 3. Mobile App Configuration

**Add Widgets:**
1. **Gauge** (V0) - Temperature display
2. **Gauge** (V1) - Humidity display
3. **LED** (V3) - Motion detection indicator
4. **LED** (V4) - Fire detection indicator
5. **Switch** (V5) - Day/Night mode toggle
6. **Switch** (V6) - AC system control

**Notification Settings:**
- Enable push notifications for V3 and V4
- Set alert thresholds as needed

### 4. Get Credentials

From Blynk Console:
- Copy **Template ID**
- Copy **Auth Token** from device info
- Use these in your code configuration

## 🚀 Deployment Process

### 📤 Upload Code

#### PlatformIO Method:
```bash
cd Main_RTOS_ADDED
pio run -t upload
```

#### Arduino IDE Method:
1. Open `Smart_Shop_Guard_Arduino/Smart_Shop_Guard_Arduino.ino`
2. Select Board: "ESP32S3 Dev Module"
3. Select Port: Your ESP32 COM port
4. Click Upload button

### 📊 Monitor System

#### Serial Monitor:
```bash
# PlatformIO
pio device monitor

# Arduino IDE
Tools → Serial Monitor (115200 baud)
```

**Expected Output:**
```
🛡️ Smart Shop Guard System Starting...
🔧 Initializing hardware...
📺 Displays initialized
🔍 Sensors initialized
🌐 Connecting to WiFi...
✅ WiFi connected: 192.168.1.100
☁️ Blynk connected
🚀 System ready for operation
```

## 🔧 Testing & Calibration

### 1. Individual Component Testing

**Test each component separately using demo files:**

```bash
# Test OLED display
cd OLED_DEMO
# Upload and test display functionality

# Test LED effects
cd smart_guard_leds_demo
# Upload and test LED patterns

# Test servo motor
cd servo_test
# Upload and test door mechanism
```

### 2. Sensor Calibration

**Temperature/Humidity:**
- Compare readings with known reference
- Adjust thresholds in config.h if needed

**Motion Sensor:**
- Test detection range and sensitivity
- Adjust positioning for optimal coverage

**Ultrasonic Sensor:**
- Verify distance accuracy
- Calibrate threshold for door activation

**Flame Sensor:**
- Test with controlled flame source
- Ensure proper sensitivity without false positives

### 3. System Integration Testing

**Test Sequence:**
1. **Power On** - Verify startup sequence
2. **WiFi Connection** - Check network connectivity
3. **Blynk Sync** - Confirm cloud communication
4. **Sensor Readings** - Validate all sensor data
5. **Alert System** - Test fire and motion alerts
6. **Display Updates** - Verify both LCD and OLED
7. **Remote Control** - Test Blynk app controls

## 🛠️ Troubleshooting

### 🔌 Hardware Issues

**Power Problems:**
- Check voltage levels (3.3V for ESP32, 5V for most sensors)
- Verify current capacity for servo and other high-draw components
- Add decoupling capacitors if experiencing resets

**Connection Issues:**
- Use multimeter to verify continuity
- Check for loose connections
- Verify pin assignments match code configuration

**Sensor Problems:**
- Test sensors individually with simple code
- Check power supply to sensors
- Verify proper pull-up resistors where needed

### 💻 Software Issues

**Compilation Errors:**
- Verify all libraries are installed
- Check board selection and settings
- Ensure credentials are properly configured

**Runtime Issues:**
- Monitor serial output for error messages
- Check memory usage (heap monitoring)
- Verify FreeRTOS task creation success

**Network Issues:**
- Confirm WiFi credentials
- Check 2.4GHz network availability
- Monitor Blynk connection status

### 📱 Blynk Issues

**Connection Problems:**
- Verify auth token and template ID
- Check internet connectivity
- Confirm Blynk server status

**Widget Issues:**
- Verify virtual pin assignments
- Check datastream configuration
- Test with simple virtual pin writes

## 📈 Performance Optimization

### ⚡ System Performance

**Memory Management:**
- Monitor heap usage via serial output
- Optimize task stack sizes
- Use static allocations where possible

**Task Scheduling:**
- Balance task priorities
- Minimize blocking operations
- Use appropriate delays in tasks

**Power Efficiency:**
- Implement sleep modes if battery powered
- Optimize display update frequencies
- Use interrupt-driven sensor reading where applicable

### 🔧 Maintenance

**Regular Checks:**
- Monitor system uptime and stability
- Check sensor accuracy periodically
- Verify mechanical components (servo, relay)
- Update firmware as improvements are released

**Data Monitoring:**
- Review Blynk historical data
- Monitor for sensor drift
- Check for false alarms
- Analyze system performance metrics

## 📞 Support & Resources

### 📚 Documentation
- [ESP32 Documentation](https://docs.espressif.com/projects/esp32/en/latest/)
- [Blynk Documentation](https://docs.blynk.io/)
- [PlatformIO Documentation](https://docs.platformio.org/)

### 🤝 Community Support
- GitHub Issues for bug reports
- Community forums for general questions
- Project documentation for detailed information

### 🔧 Professional Support
For commercial deployments or advanced customization:
- Hardware design consultation
- Custom firmware development
- System integration services
- Training and documentation

---

This setup guide provides a complete roadmap for implementing the Smart Shop Guard system. Follow each section carefully, and don't hesitate to test components individually before full system integration.

**Success Checklist:**
- [ ] Hardware assembled and tested
- [ ] Software uploaded successfully
- [ ] WiFi and Blynk connectivity confirmed
- [ ] All sensors providing accurate readings
- [ ] Alert systems functioning properly
- [ ] Mobile app control working
- [ ] System running stably for 24+ hours

*Happy building! 🛡️🏪*
