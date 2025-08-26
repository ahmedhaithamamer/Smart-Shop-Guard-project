# 🤝 Contributing to Smart Shop Guard

Thank you for your interest in contributing to the Smart Shop Guard project! We welcome contributions from developers of all skill levels.

## 🎯 Ways to Contribute

### 🐛 Bug Reports
- **Search existing issues** before creating new ones
- **Use clear, descriptive titles** for bug reports
- **Include system information** (hardware, software versions)
- **Provide reproduction steps** with expected vs actual behavior
- **Add photos/videos** for hardware-related issues

### 💡 Feature Requests
- **Check existing feature requests** to avoid duplicates
- **Describe the problem** your feature would solve
- **Explain the proposed solution** with implementation details
- **Consider backward compatibility** and system impact

### 🔧 Code Contributions
- **Fork the repository** and create a feature branch
- **Follow existing code style** and conventions
- **Add comprehensive comments** for new functionality
- **Test thoroughly** on actual hardware when possible
- **Update documentation** for any changes

### 📚 Documentation Improvements
- **Fix typos and grammatical errors**
- **Improve clarity** of existing explanations
- **Add missing information** or examples
- **Update outdated content**
- **Translate documentation** to other languages

## 🛠️ Development Setup

### 📋 Prerequisites
- **PlatformIO IDE** or **Arduino IDE**
- **ESP32-S3 development board**
- **Required hardware components** (see hardware requirements)
- **Git** for version control

### 🔧 Setup Process
1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/Smart-Shop-Guard-project.git
   cd Smart-Shop-Guard-project
   ```
3. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```
4. **Set up credentials** (copy `credentials_template.h` to `credentials.h`)
5. **Test the existing system** before making changes

## 📝 Code Style Guidelines

### 🎨 General Principles
- **Readability first** - code should be self-documenting
- **Consistency** - follow existing patterns and conventions
- **Modularity** - keep functions focused and reusable
- **Error handling** - implement proper error checking and recovery

### 📋 Specific Guidelines

#### 🔤 Naming Conventions
```cpp
// Functions: camelCase
void initSensors();
void readTemperatureHumidity();

// Variables: camelCase
int temperatureReading;
bool isSystemActive;

// Constants: UPPER_SNAKE_CASE
#define MAX_TEMPERATURE 50
#define SENSOR_UPDATE_INTERVAL 1000

// Classes/Structs: PascalCase
struct SensorData {
    int temperature;
    int humidity;
};
```

#### 📄 File Organization
```cpp
// File header with description
/**
 * @file sensors.cpp
 * @brief Sensor management and data processing
 * @author Your Name
 * @date 2024
 */

// Includes (system first, then local)
#include <Arduino.h>
#include <Wire.h>
#include "sensors.h"
#include "config.h"

// Constants and definitions
#define SENSOR_TIMEOUT 5000

// Global variables (minimize usage)
static int lastReading = 0;

// Function implementations
```

#### 💬 Comments and Documentation
```cpp
/**
 * @brief Initializes all sensor components
 * @details Sets up pin modes, initializes I2C communications,
 *          and performs initial sensor calibration
 * @return true if initialization successful, false otherwise
 */
bool initSensors() {
    // Initialize DHT sensor for temperature/humidity readings
    dht.begin();
    
    // Configure pin modes for digital sensors
    pinMode(FLAME_SENSOR_PIN, INPUT);   // Flame detection (active low)
    pinMode(PIR_PIN, INPUT);            // Motion detection
    
    return true;
}
```

#### 🔄 FreeRTOS Specific Guidelines
```cpp
// Task functions should be clearly named and documented
void TaskSensorPoll(void* pvParameters) {
    // Task initialization
    Serial.printf("[CORE %d] TaskSensorPoll started\n", xPortGetCoreID());
    
    // Task main loop
    for(;;) {
        // Task implementation
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

// Use proper mutex handling
if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
    // Critical section code
    xSemaphoreGive(i2cMutex);
} else {
    Serial.println("Failed to acquire I2C mutex");
}
```

## 🧪 Testing Guidelines

### 🔍 Testing Requirements
- **Unit Testing** - Test individual functions in isolation
- **Integration Testing** - Test component interactions
- **Hardware Testing** - Verify on actual hardware setup
- **Regression Testing** - Ensure existing functionality still works

### 🛠️ Testing Process
1. **Test existing functionality** before making changes
2. **Test your changes** thoroughly with various inputs
3. **Test edge cases** and error conditions
4. **Document test results** in pull request description

### 📊 Hardware Testing Checklist
- [ ] All sensors provide accurate readings
- [ ] Actuators respond correctly to commands
- [ ] Display updates work properly
- [ ] WiFi and Blynk connectivity functional
- [ ] System runs stable for extended periods
- [ ] Power consumption within acceptable limits

## 📤 Submission Process

### 🔄 Pull Request Process
1. **Ensure your branch is up-to-date**:
   ```bash
   git checkout main
   git pull upstream main
   git checkout feature/your-feature-name
   git rebase main
   ```

2. **Create a comprehensive pull request**:
   - **Clear title** describing the change
   - **Detailed description** of what was changed and why
   - **Testing performed** and results
   - **Screenshots/videos** for UI changes
   - **Breaking changes** clearly documented

3. **Respond to feedback** promptly and constructively

### ✅ Pull Request Checklist
- [ ] Code follows project style guidelines
- [ ] All tests pass
- [ ] Documentation updated for changes
- [ ] Commit messages are clear and descriptive
- [ ] No sensitive information (credentials) included
- [ ] Hardware tested (if applicable)
- [ ] Backward compatibility maintained

## 🏗️ Project Structure

### 📁 Directory Guidelines
- **`Main_RTOS_ADDED/`** - Primary FreeRTOS implementation
- **`Smart_Shop_Guard_Arduino/`** - Arduino IDE compatible version
- **`docs/`** - Additional documentation
- **`examples/`** - Example configurations and use cases
- **`tests/`** - Test files and utilities

### 📋 File Naming
- **Source files**: `snake_case.cpp`
- **Header files**: `snake_case.h`
- **Documentation**: `UPPER_CASE.md`
- **Examples**: `descriptive_name.ino`

## 🎯 Contribution Areas

### 🔧 Hardware Support
- **New sensor integrations**
- **Additional display types**
- **Different microcontroller support**
- **Power management improvements**

### 💻 Software Features
- **Enhanced security features**
- **Additional IoT platform support**
- **Web interface development**
- **Mobile app improvements**

### 📚 Documentation
- **Tutorial improvements**
- **API documentation**
- **Troubleshooting guides**
- **Video tutorials**

### 🌍 Internationalization
- **Multi-language support**
- **Localized documentation**
- **Regional hardware variations**

## 🛡️ Security Considerations

### 🔐 Security Guidelines
- **Never commit credentials** or sensitive information
- **Use secure communication** protocols where possible
- **Validate all inputs** to prevent injection attacks
- **Follow secure coding practices**

### 🔍 Security Review Process
- **Security-related changes** require additional review
- **Penetration testing** for network-connected features
- **Code review** for authentication and authorization code

## 📞 Getting Help

### 💬 Communication Channels
- **GitHub Issues** - Bug reports and feature requests
- **GitHub Discussions** - General questions and ideas
- **Email** - For sensitive security issues

### 📚 Resources
- **Project Documentation** - Comprehensive guides and references
- **Hardware Schematics** - Circuit diagrams and connection guides
- **Video Tutorials** - Step-by-step implementation guides

## 🏆 Recognition

### 🎖️ Contributor Recognition
- **Contributors list** maintained in project documentation
- **Special recognition** for significant contributions
- **Community highlights** for innovative implementations

### 📈 Contribution Types
- **Code contributions** - Direct code improvements
- **Documentation** - Guides, tutorials, and references
- **Testing** - Hardware testing and bug reports
- **Community support** - Helping other users

## 📜 Code of Conduct

### 🤝 Community Standards
- **Be respectful** and inclusive in all interactions
- **Provide constructive feedback** and criticism
- **Help newcomers** learn and contribute
- **Maintain professional communication**

### 🚫 Unacceptable Behavior
- **Harassment or discrimination** of any kind
- **Spam or off-topic content**
- **Sharing others' private information**
- **Commercial promotion** without permission

## 📅 Release Process

### 🔄 Version Management
- **Semantic versioning** (MAJOR.MINOR.PATCH)
- **Release notes** documenting all changes
- **Backward compatibility** considerations
- **Migration guides** for breaking changes

### 🎯 Release Schedule
- **Regular releases** based on feature completion
- **Security updates** released promptly
- **Community input** on release priorities

---

Thank you for contributing to the Smart Shop Guard project! Your contributions help make this system better for everyone. 🛡️🏪

*Happy coding!* 🚀
