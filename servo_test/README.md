# 🚪 Servo Test Utility - Smart Shop Guard

## 📖 Overview

This folder contains a simple utility for testing the servo motor functionality used in the Smart Shop Guard's automatic door system. It provides a standalone test environment for validating servo movement and ultrasonic sensor integration before deployment in the main system.

## 🎯 Purpose

- **Hardware Validation** - Test servo motor functionality independently
- **Sensor Calibration** - Verify ultrasonic sensor distance measurements
- **Integration Testing** - Test servo-sensor coordination
- **Troubleshooting Tool** - Isolate and diagnose door system issues

## 📁 Contents

```
servo_test/
└── servo_test.ino    # Servo motor and ultrasonic sensor test utility
```

## 🔧 Hardware Requirements

### 🚪 Servo Motor
- **Standard Servo Motor** (SG90, MG996R, or similar)
- **Operating Voltage:** 4.8V - 6V
- **Torque:** Adequate for door mechanism
- **Rotation:** 180° range

### 📏 Ultrasonic Sensor
- **HC-SR04** Ultrasonic Distance Sensor
- **Operating Voltage:** 5V
- **Range:** 2cm - 400cm
- **Accuracy:** ±3mm

### 🔌 Microcontroller
- **ESP32-S3** (as configured) or compatible board
- **PWM Capability** for servo control
- **Digital I/O** for ultrasonic sensor

## 📌 Pin Configuration

```cpp
// Pin definitions (matching main project)
#define SERVO_PIN 7              // Servo control pin
#define TRIG_PIN 42              // Ultrasonic trigger pin
#define ECHO_PIN 41              // Ultrasonic echo pin
#define DISTANCE_THRESHOLD 12    // Distance threshold in cm
```

## 🚀 Quick Start

### 📋 Prerequisites

- **Arduino IDE** with ESP32 support
- **Required Library:**
  - ESP32Servo

### 🔧 Installation

1. **Install ESP32Servo Library:**
   ```
   Tools → Manage Libraries → Search: "ESP32Servo"
   Install ESP32Servo by Kevin Harrington
   ```

2. **Hardware Connections:**
   ```
   ESP32-S3        Servo Motor
   GPIO 7     ──── Signal (Orange/Yellow wire)
   5V         ──── Power (Red wire)
   GND        ──── Ground (Brown/Black wire)
   
   ESP32-S3        HC-SR04
   GPIO 42    ──── Trig
   GPIO 41    ──── Echo
   5V         ──── VCC
   GND        ──── GND
   ```

3. **Upload and Test:**
   - Select Board: ESP32S3 Dev Module
   - Select correct COM port
   - Upload servo_test.ino
   - Open Serial Monitor (115200 baud)

## 🔄 Test Sequence

### 🚀 Startup Sequence

1. **Initialization Message:** System startup confirmation
2. **Servo Test:** Automatic movement test (0° → 180° → 0°)
3. **Sensor Calibration:** Distance measurement verification
4. **Interactive Mode:** Real-time distance-based control

### 📊 Serial Output Example

```
🚪 Smart Shop Guard - Servo Test Starting...
🚪 Servo initialized to CLOSED (0°)
🚪 Testing servo movement...
🚪 Servo OPEN (180°)
🚪 Servo CLOSED (0°)
🚪 Starting distance-based control...
🚪 Move object within 12cm to open door
📏 Distance: 25cm - DOOR SHOULD BE CLOSED
📏 Distance: 8cm - DOOR SHOULD BE OPEN
📏 Distance: 15cm - DOOR SHOULD BE CLOSED
```

## 🎛️ Operating Modes

### 🔧 Test Mode (First 10 seconds)
- **Servo Movement Test:** Demonstrates full range of motion
- **Position Verification:** Confirms servo responds to commands
- **Mechanical Check:** Validates physical door mechanism

### 🔄 Interactive Mode (Continuous)
- **Distance Monitoring:** Real-time ultrasonic measurements
- **Automatic Control:** Door opens when object within threshold
- **Status Reporting:** Serial output of distance and door state

## ⚙️ Configuration Parameters

### 📏 Distance Settings

```cpp
#define DISTANCE_THRESHOLD 12    // Trigger distance in centimeters
```

**Adjustment Guidelines:**
- **Closer Detection (5-8cm):** More precise triggering
- **Standard Detection (10-15cm):** Balanced sensitivity
- **Wider Detection (15-25cm):** Early door activation

### 🚪 Servo Positions

```cpp
// Door positions
int closedPosition = 0;      // Closed door angle
int openPosition = 180;      // Open door angle
```

**Customization Options:**
- Adjust angles based on physical door mechanism
- Consider mechanical limits and mounting orientation
- Test with actual door hardware for optimal positions

### ⏱️ Timing Settings

```cpp
// Test sequence timing
delay(2000);                 // Position hold time
delay(1000);                 // Status update interval
delay(100);                  // Main loop delay
```

## 🔍 Diagnostic Features

### 📊 Real-time Monitoring

The test utility provides comprehensive real-time feedback:

- **Distance Readings:** Continuous ultrasonic measurements
- **Servo Position:** Current door position status
- **Trigger Events:** When threshold conditions are met
- **System Status:** Overall operational state

### 🐛 Debug Information

```cpp
Serial.printf("📏 Distance: %dcm ", distance);
if (distance <= DISTANCE_THRESHOLD) {
  Serial.println("- DOOR SHOULD BE OPEN");
  myServo.write(180);
} else {
  Serial.println("- DOOR SHOULD BE CLOSED");
  myServo.write(0);
}
```

## 🛠️ Troubleshooting

### 🚪 Servo Issues

**Servo Not Moving:**
- Check power supply (servo requires 5V, adequate current)
- Verify signal wire connection to GPIO 7
- Test with different servo if available
- Ensure ESP32Servo library is properly installed

**Erratic Movement:**
- Check for loose connections
- Verify power supply stability
- Add capacitor across power supply if needed
- Reduce servo load (disconnect from door mechanism)

**Insufficient Torque:**
- Upgrade to higher torque servo (MG996R recommended)
- Check power supply current capacity
- Verify mechanical linkage isn't binding
- Consider gear reduction for heavy doors

### 📏 Ultrasonic Sensor Issues

**No Distance Readings:**
- Verify trigger and echo pin connections
- Check 5V power supply to sensor
- Test with different sensor if available
- Ensure clear line of sight for sensor

**Inaccurate Readings:**
- Check for electromagnetic interference
- Verify sensor mounting is secure
- Test in different environmental conditions
- Consider temperature compensation

**Intermittent Readings:**
- Check for loose connections
- Verify adequate power supply
- Add delays between readings if needed
- Monitor for timeout conditions

### 🔌 Power Supply Issues

**System Resets:**
- Servo draws significant current during movement
- Use adequate power supply (5V, 2A minimum)
- Add bulk capacitor (1000µF) across power rails
- Consider separate power supply for servo

## 📈 Performance Optimization

### ⚡ Response Time

```cpp
// Optimize for faster response
#define SENSOR_UPDATE_RATE 50    // Update every 50ms
#define SERVO_MOVE_DELAY 15      // Minimum servo update delay
```

### 🔋 Power Efficiency

```cpp
// Power saving options
if (distance > DISTANCE_THRESHOLD + 5) {
  // Servo can be detached when not needed
  myServo.detach();
} else {
  // Re-attach when movement required
  myServo.attach(SERVO_PIN);
}
```

## 🔗 Integration Preparation

This test utility helps prepare for main system integration:

### ✅ Validation Checklist

- [ ] Servo moves smoothly through full range
- [ ] Distance readings are accurate and consistent
- [ ] Door opens when object approaches threshold
- [ ] Door closes when object moves away
- [ ] No mechanical binding or excessive noise
- [ ] Power supply handles servo current draw
- [ ] Serial output shows expected behavior

### 🔧 Calibration Data

Record these values for main system configuration:

```cpp
// Calibrated values from testing
#define OPTIMAL_DISTANCE_THRESHOLD 12    // Tested threshold
#define SERVO_OPEN_ANGLE 180            // Tested open position
#define SERVO_CLOSED_ANGLE 0            // Tested closed position
#define SERVO_MOVE_DELAY 500            // Tested response time
```

## 🔄 Migration to Main System

After successful testing:

1. **Copy Calibrated Values:** Transfer working parameters to main code
2. **Verify Pin Assignments:** Ensure consistency with main project
3. **Test Integration:** Verify servo works within complete system
4. **Performance Validation:** Confirm timing meets system requirements

## 📊 Test Results Documentation

### 📝 Test Log Template

```
Date: ___________
Hardware: ESP32-S3, Servo Model: _______, Sensor: HC-SR04

Distance Threshold Testing:
- 5cm:  Response time: ___ms, Reliability: ___%
- 10cm: Response time: ___ms, Reliability: ___%
- 15cm: Response time: ___ms, Reliability: ___%

Servo Performance:
- Open Time: ___ms (0° to 180°)
- Close Time: ___ms (180° to 0°)
- Position Accuracy: ±___°

Power Consumption:
- Idle: ___mA
- Moving: ___mA
- Peak: ___mA

Issues Encountered:
- ________________________
- ________________________

Recommended Settings:
- Distance Threshold: ___cm
- Servo Angles: Open=___°, Closed=___°
- Update Rate: ___ms
```

## 📞 Support

For servo test utility issues:
- Monitor serial output at 115200 baud for diagnostic information
- Test servo and sensor independently before combined testing
- Verify power supply meets current requirements
- Check pin assignments match your hardware configuration

---

This servo test utility provides a reliable foundation for validating the automatic door system before integration into the complete Smart Shop Guard project.
