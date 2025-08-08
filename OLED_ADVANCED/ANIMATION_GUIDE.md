# Smart Shop Guard - Animation Guide

## 🎬 **Intro Animation Sequence**

The system now features a comprehensive startup animation that plays when the device powers on:

### **Animation Timeline (Total: ~7.2 seconds)**

1. **"SMART" Text** (0.8s)
   - 16pt font, centered
   - Appears at position (30, 15)

2. **"SHOP" Text** (0.8s) 
   - Added below "SMART"
   - Position (35, 35)

3. **"GUARD" + Shield Icon** (1.0s)
   - Screen clears
   - Shield icon at (20, 10)
   - "GUARD" text at (40, 15)

4. **Loading Sequence** (2.4s)
   - "Smart Shop Guard" title
   - Animated progress bar (8 steps)
   - "Loading..." with animated dots
   - Each step: 300ms

5. **System Ready** (1.2s)
   - "SYSTEM" text
   - "READY!" confirmation

6. **Fade Effect** (1.2s)
   - 3 fade cycles
   - "Initializing..." message
   - Each cycle: 400ms

## 🎭 **Runtime Animations**

### **Status Page Animations**

#### **Emergency Alerts:**
- **Fire Alert**: 
  - Icon alternates: `fire_icon` ↔ `fire_alt1`
  - Text alternates: "FIRE DETECTED!" ↔ ">>> FIRE! <<<"
  
- **Motion Alert**:
  - Icon alternates: `alert_icon` ↔ `warning_icon` 
  - Text alternates: "MOTION DETECTED!" ↔ ">>> MOTION! <<<"

#### **Normal Operation:**
- **Heartbeat Indicator**: Small circle in bottom-right
  - Filled circle (●) ↔ Empty circle (○)
  - Position: (120, 55)
  - Indicates system is alive

### **Sensor Page Animations**

#### **Temperature-Based Icon:**
- **Normal Temp** (≤30°C): Standard `temp_icon`
- **High Temp** (>30°C): Warning `temp_alt1` icon

#### **Activity Indicators:**
- **Temperature Reading**: Asterisk (*) ↔ Circle (o)
- **Humidity Reading**: Asterisk (*) ↔ Circle (o)
- Position: (100, 26) and (100, 36)

### **System Page Animations**

#### **Health-Based Icon:**
- **Normal Memory** (≥10KB): Standard `system_icon`
- **Low Memory** (<10KB): Warning `warning_icon`

#### **Activity Indicators:**
- **System Status**: 3x3 pixel squares
- **Active State**: Filled squares (■)
- **Idle State**: Empty squares (□)
- Positions: (120, 26), (120, 36), (120, 46)

## ⏱️ **Animation Timing**

### **Global Animation Variables:**
```cpp
unsigned long lastAnimation = 0;
bool alertFlashing = false;
```

### **Animation Cycle:**
- **Frequency**: 500ms (2 Hz)
- **Emergency Alerts**: Fast flashing for visibility
- **Normal Indicators**: Gentle pulsing for status

### **Animation States:**
- **alertFlashing = true**: Active/Alert state
- **alertFlashing = false**: Normal/Idle state

## 🎨 **Visual Effects**

### **Text Animations:**
- **Emergency Text**: Uses ">>>" brackets for attention
- **Loading Dots**: Cycles through 0-3 dots
- **Progress Bar**: 8-step loading animation

### **Icon Animations:**
- **Fire**: Alternates between two fire icons
- **Motion**: Warning triangle when alerting
- **Temperature**: Changes based on reading
- **System**: Warning icon for low memory

### **Geometric Animations:**
- **Circles**: Heartbeat indicator (filled/empty)
- **Squares**: System activity (filled/empty)
- **Progress Bar**: Animated loading rectangle

## 🔧 **Animation Configuration**

### **Timing Constants:**
```cpp
const unsigned long longPressDelay = 1000;      // Button timing
const unsigned long autoSwipeDelay = 15000;     // Page switching
// Animation refresh: 500ms in handleAnimations()
```

### **Animation Triggers:**
- **Emergency**: `sensors.fireDetected` or `sensors.motionDetected`
- **System Health**: Memory levels, temperature thresholds
- **Activity**: Always active during normal operation

## 📱 **Display Compatibility**

### **Font Usage in Animations:**
- **Intro**: 16pt for titles, 10pt for details
- **Runtime**: Only 10pt (maintains OLED_1 compatibility)
- **Icons**: 16x16 pixels (standard size)

### **Screen Area Usage:**
- **Intro**: Full screen (128x64)
- **Runtime**: Content area only (54 pixels height)
- **Indicators**: Corner positions to avoid text overlap

## 🚀 **Performance Impact**

### **Memory Usage:**
- **Minimal Impact**: Uses existing variables
- **No Additional Arrays**: Reuses animation state
- **Efficient Timing**: Single 500ms cycle

### **CPU Usage:**
- **Low Overhead**: Simple boolean toggles
- **Non-blocking**: Doesn't interfere with navigation
- **Optimized**: Minimal drawing operations

## 🎯 **User Experience Benefits**

### **Professional Startup:**
- **Brand Recognition**: "Smart Shop Guard" prominently displayed
- **Progress Feedback**: Loading bar shows initialization
- **Status Confirmation**: "SYSTEM READY" provides confidence

### **Visual Feedback:**
- **Emergency Alerts**: Impossible to miss with flashing
- **System Status**: Always know the system is working
- **Activity Indicators**: See sensor readings are live

### **Accessibility:**
- **High Contrast**: Animations work with inverted theme
- **Clear Timing**: 500ms cycle is comfortable for viewing
- **Non-intrusive**: Normal operation isn't disrupted

---

## ✨ **Animation Summary**

The Smart Shop Guard now features:
- **🎬 7-second intro sequence** with "Smart Shop Guard" branding
- **🚨 Emergency alert animations** with flashing text and icons  
- **💓 Heartbeat indicators** showing system is alive
- **📊 Activity indicators** for sensors and system status
- **🎨 Dynamic icons** that change based on conditions

All animations maintain the **compact 10pt font** requirement while adding **professional visual appeal** and **critical safety alerts**!

**Total Animation Duration**: ~7.2 seconds startup + continuous runtime effects
**Performance**: Minimal impact, 500ms refresh cycle
**Compatibility**: Works perfectly with 1.3" 128x64 OLED display