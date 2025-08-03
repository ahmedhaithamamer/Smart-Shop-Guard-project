# Smart Shop Guard - Modular Version

A modular, RTOS-based IoT security system for smart shops using ESP32-S3.

## 🏗️ **Modular Architecture**

This project has been refactored into a modular architecture with the following components:

### **Core Modules**

1. **Config Manager** (`config.h`)
   - Centralized configuration management
   - Pin definitions and constants
   - Task priorities and stack sizes

2. **WiFi Manager** (`wifi_manager.h/cpp`)
   - Robust WiFi connection handling
   - Automatic reconnection logic
   - Connection status monitoring

3. **Blynk Manager** (`blynk_manager.h/cpp`)
   - IoT platform integration
   - Virtual pin operations
   - Remote control handling

4. **Sensor Manager** (`sensor_manager.h/cpp`)
   - DHT temperature/humidity sensor
   - PIR motion sensor
   - Flame sensor
   - Ultrasonic distance sensor
   - Servo motor control

5. **Actuator Manager** (`actuator_manager.h/cpp`)
   - Fan control
   - Buzzer alarm system
   - Relay control
   - Automatic actuator logic

6. **Display Manager** (`display_manager.h/cpp`)
   - LCD display handling
   - OLED display handling
   - Multi-display coordination

7. **Task Manager** (`task_manager.h/cpp`)
   - RTOS task management
   - System health monitoring
   - Task coordination

## 📁 **Project Structure**

```
New_RTOS_Modular/
├── platformio.ini          # PlatformIO configuration
├── src/
│   ├── main.cpp            # Main application entry point
│   ├── wifi_manager.cpp    # WiFi management
│   ├── blynk_manager.cpp   # Blynk IoT platform
│   ├── sensor_manager.cpp  # Sensor handling
│   ├── actuator_manager.cpp # Actuator control
│   ├── display_manager.cpp # Display management
│   ├── task_manager.cpp    # RTOS task management
│   └── oled_display.cpp    # OLED display (legacy)
├── include/
│   ├── config.h            # Configuration constants
│   ├── wifi_manager.h      # WiFi manager interface
│   ├── blynk_manager.h     # Blynk manager interface
│   ├── sensor_manager.h    # Sensor manager interface
│   ├── actuator_manager.h  # Actuator manager interface
│   ├── display_manager.h   # Display manager interface
│   ├── task_manager.h      # Task manager interface
│   └── oled_display.h     # OLED display interface
├── lib/                    # Custom libraries
├── test/                   # Test files
└── README.md              # This file
```

## 🚀 **Key Features**

### **Enhanced Reliability**
- **Robust WiFi Management**: Automatic reconnection with exponential backoff
- **Blynk Reconnection**: Automatic IoT platform reconnection
- **Error Handling**: Comprehensive error handling and recovery
- **Task Monitoring**: Real-time task health monitoring

### **Modular Design**
- **Separation of Concerns**: Each module handles specific functionality
- **Easy Maintenance**: Isolated components for easier debugging
- **Scalability**: Easy to add new features or modify existing ones
- **Reusability**: Modules can be reused in other projects

### **Advanced Features**
- **Multi-Display Support**: LCD and OLED displays
- **Smart Actuator Control**: Automatic fan control based on conditions
- **Security Monitoring**: Motion, flame, and distance detection
- **Remote Control**: Blynk-based remote monitoring and control

## 🔧 **Hardware Requirements**

- **ESP32-S3-DevKitM-1** development board
- **DHT11** temperature/humidity sensor
- **PIR** motion sensor
- **Flame sensor**
- **Ultrasonic sensor** (HC-SR04)
- **Servo motor**
- **Relay module**
- **Buzzer**
- **Fan**
- **LCD I2C display** (16x2)
- **OLED display** (128x64)

## 📋 **Pin Configuration**

| Component | Pin | Function |
|-----------|-----|----------|
| DHT11 | 5 | Temperature/Humidity |
| Fan | 6 | Fan control |
| Flame Sensor | 7 | Fire detection |
| Buzzer | 8 | Alarm system |
| Relay | 4 | Relay control |
| PIR | 0 | Motion detection |
| Servo | 3 | Servo motor |
| Ultrasonic Trig | 1 | Distance measurement |
| Ultrasonic Echo | 2 | Distance measurement |

## 🛠️ **Installation & Setup**

1. **Clone the project**:
   ```bash
   git clone <repository-url>
   cd New_RTOS_Modular
   ```

2. **Install PlatformIO** (if not already installed):
   ```bash
   pip install platformio
   ```

3. **Build the project**:
   ```bash
   pio run
   ```

4. **Upload to ESP32-S3**:
   ```bash
   pio run --target upload
   ```

5. **Monitor serial output**:
   ```bash
   pio device monitor
   ```

## 🔧 **Configuration**

### **WiFi Settings**
Edit `include/config.h`:
```cpp
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"
```

### **Blynk Settings**
The Blynk configuration is already set in `platformio.ini`:
```ini
build_flags = 
    -D BLYNK_TEMPLATE_ID="\"TMPL2jt8pOqfP\""
    -D BLYNK_TEMPLATE_NAME="\"Smart Secure Smart Shop\""
    -D BLYNK_AUTH_TOKEN="\"LVODBytZ9qETpIjcYFFx5-4b4UFfRUdg\""
```

## 📊 **System Architecture**

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   WiFi Manager  │    │  Blynk Manager  │    │ Sensor Manager  │
│                 │    │                 │    │                 │
│ • Connection    │    │ • IoT Platform  │    │ • DHT Sensor    │
│ • Reconnection  │    │ • Virtual Pins  │    │ • PIR Sensor    │
│ • Status Check  │    │ • Remote Control│    │ • Flame Sensor  │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
                    ┌─────────────────┐
                    │  Task Manager   │
                    │                 │
                    │ • RTOS Tasks    │
                    │ • Coordination  │
                    │ • Health Monitor│
                    └─────────────────┘
                                 │
         ┌───────────────────────┼───────────────────────┐
         │                       │                       │
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│ Actuator Manager│    │ Display Manager │    │ Config Manager  │
│                 │    │                 │    │                 │
│ • Fan Control   │    │ • LCD Display   │    │ • Pin Defs      │
│ • Buzzer Alarm  │    │ • OLED Display  │    │ • Constants     │
│ • Relay Control │    │ • Multi-Display │    │ • Task Config   │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

## 🔍 **Monitoring & Debugging**

### **Serial Monitor**
The system provides comprehensive logging:
```
Smart Shop Guard - Modular Version
Initializing system...
WiFi connected successfully
Blynk connected successfully
Sensors initialized
Actuators initialized
Displays initialized
Task Manager initialized
Starting RTOS tasks...
All tasks created successfully
System initialization complete!
```

### **System Status**
The system prints status every 30 seconds:
```
=== System Status ===
System Running: YES
Tasks Created: YES
Temperature: 24.5°C
Humidity: 45.2%
Flame Detected: NO
Motion Detected: NO
Fan State: OFF
Alarm Active: NO
===================
```

## 🚨 **Security Features**

1. **Motion Detection**: PIR sensor for intrusion detection
2. **Fire Detection**: Flame sensor for fire safety
3. **Environmental Monitoring**: Temperature and humidity monitoring
4. **Automatic Response**: Fan activation, alarms, and relay control
5. **Remote Monitoring**: Real-time status via Blynk

## 🔄 **RTOS Tasks**

| Task | Priority | Stack Size | Function |
|------|----------|------------|----------|
| BlynkTask | High | 4096 | WiFi/Blynk management |
| SensorTask | Medium | 4096 | Sensor reading & Blynk updates |
| ActuatorTask | Medium | 2048 | Actuator control |
| DisplayTask | Low | 2048 | Display updates |
| SystemTask | Low | 2048 | System monitoring |

## 🛡️ **Error Handling**

- **WiFi Reconnection**: Automatic reconnection with exponential backoff
- **Blynk Reconnection**: Automatic IoT platform reconnection
- **Sensor Validation**: Data validation for sensor readings
- **Task Health Monitoring**: Real-time task status monitoring
- **Emergency Stop**: System-wide emergency stop functionality

## 📈 **Performance Optimizations**

- **Non-blocking Operations**: All operations are non-blocking
- **Efficient Task Scheduling**: Optimized task priorities and timing
- **Memory Management**: Proper memory allocation and deallocation
- **Power Management**: Efficient power usage patterns

## 🔧 **Customization**

### **Adding New Sensors**
1. Add sensor class to `sensor_manager.h`
2. Implement sensor logic in `sensor_manager.cpp`
3. Update task manager to handle new sensor data

### **Adding New Actuators**
1. Add actuator class to `actuator_manager.h`
2. Implement actuator logic in `actuator_manager.cpp`
3. Update task manager to control new actuators

### **Adding New Displays**
1. Add display class to `display_manager.h`
2. Implement display logic in `display_manager.cpp`
3. Update task manager to update new displays

## 📝 **License**

This project is licensed under the MIT License - see the LICENSE file for details.

## 🤝 **Contributing**

1. Fork the project
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📞 **Support**

For support and questions, please open an issue on the project repository. 