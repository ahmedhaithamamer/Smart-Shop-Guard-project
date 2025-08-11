# OLED Button Logic Guide

## Overview
This document explains the button logic implementation for the Smart Shop Guard OLED display system.

## Button Configuration
- **NEXT Button**: Pin 47 (with internal pull-up resistor)
- **PREV Button**: Pin 48 (with internal pull-up resistor)
- Both buttons are configured as INPUT_PULLUP, so they read LOW when pressed

## Button Functions

### 1. Page Navigation (Normal Mode)
- **NEXT Button**: Move to next page
- **PREV Button**: Move to previous page
- **Pages Available**: 5 total pages
  - Status Page (0)
  - Sensors Page (1)
  - System Page (2)
  - Alerts Page (3)
  - Settings Page (4)

### 2. Settings Mode Access
- **Long Press NEXT** (1 second) on Settings Page: Enter/Exit Settings Mode
- **NEXT Button**: Navigate between settings options
- **PREV Button**: Execute/Apply selected setting

### 3. Special Functions
- **Both Buttons Pressed** (1 second): Toggle Auto-Swipe mode
- **Auto-Swipe**: Automatically cycles through pages every 15 seconds

## Settings Options

### 1. Auto-Swipe (Setting 0)
- **ON**: Pages automatically cycle every 15 seconds
- **OFF**: Manual navigation only
- **Action**: Toggle auto-swipe functionality

### 2. Pages Info (Setting 1)
- **Display**: Shows total number of pages
- **Action**: Information only (no change)

## Button Logic Implementation

### Debouncing
- **Debounce Delay**: 300ms to prevent false triggers
- **Rapid Press Protection**: 50ms minimum between button presses
- **Long Press Detection**: 1000ms for settings access

### State Management
- **Page State**: Tracks current page (0-4)
- **Settings State**: Tracks current setting selection (0-1)
- **Menu State**: Tracks whether in normal or settings mode

### Safety Features
- **Error Checking**: Validates button readings
- **State Validation**: Prevents invalid state transitions
- **Timeout Protection**: Prevents multiple rapid triggers

## Visual Feedback

### Settings Mode
- **Selection Highlight**: Current setting is highlighted with filled rectangle
- **Navigation Hints**: Shows "NEXT: Select PREV: Execute"
- **Access Hint**: Shows "Long press NEXT to enter" when not in settings

### Button Actions
- **Page Changes**: Serial output shows new page number
- **Setting Changes**: Serial output confirms changes
- **Auto-Swipe Toggle**: Display flashes to confirm change

## Integration with Main Loop

### Button Handling
- `handleOLEDButtons()` is called every loop iteration
- `handleButtons()` processes button input
- Button states are updated and actions executed

### Display Updates
- **Immediate Updates**: Critical changes (alerts, temp changes)
- **Regular Updates**: Page navigation and settings changes
- **Auto-Swipe**: Automatic page cycling when enabled

## Troubleshooting

### Common Issues
1. **Buttons Not Responding**
   - Check pin connections (47, 48)
   - Verify pull-up resistors are working
   - Check for button hardware issues

2. **False Triggers**
   - Increase debounce delay if needed
   - Check for electrical interference
   - Verify button quality and connections

3. **Settings Not Accessible**
   - Ensure you're on Settings Page (Page 4)
   - Long press NEXT button (1 second)
   - Check Serial output for confirmation

### Debug Information
- Serial output shows all button actions
- Button state changes are logged
- Error conditions are reported

## Code Structure

### Key Functions
- `initOLEDButtons()`: Initialize button pins
- `handleButtons()`: Main button processing logic
- `toggleSettings()`: Switch between normal and settings modes
- `executeSetting()`: Apply selected setting changes

### Global Variables
- `currentPage`: Current page number (0-4)
- `currentSetting`: Current setting selection (0-1)
- `currentState`: Menu state (PAGES or SETTINGS)
- `oledConfig`: Display configuration structure (auto-swipe only)

## Best Practices

1. **Button Pressing**
   - Press firmly but gently
   - Hold for 1 second for long-press actions
   - Wait for visual feedback before next action

2. **Navigation**
   - Use NEXT to move forward through pages
   - Use PREV to move backward through pages
   - Both buttons for special functions

3. **Settings**
   - Enter settings from Settings Page only
   - Navigate with NEXT, execute with PREV
   - Long press NEXT to exit settings mode

## Future Enhancements

### Potential Improvements
1. **Haptic Feedback**: Add vibration feedback for button presses
2. **Sound Feedback**: Audio confirmation of button actions
3. **Customizable Delays**: User-adjustable timing parameters
4. **Button Combinations**: Additional multi-button shortcuts
5. **Gesture Recognition**: Swipe gestures for navigation

### Code Optimization
1. **Interrupt-Based**: Use button interrupts for better responsiveness
2. **Power Management**: Optimize button reading for battery life
3. **Memory Efficiency**: Reduce static variable usage
4. **Error Recovery**: Better handling of button failures 