/**
 * @file credentials_template.h
 * @brief Template file for storing sensitive credentials
 * @author Smart Shop Guard Project
 * @date 2024
 * 
 * INSTRUCTIONS:
 * 1. Copy this file to 'credentials.h'
 * 2. Replace the placeholder values with your actual credentials
 * 3. Add 'credentials.h' to your .gitignore file
 * 4. Never commit the actual credentials.h file to version control
 */

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

// =============================================================================
// BLYNK IOT PLATFORM CREDENTIALS
// =============================================================================
// Get these from your Blynk console: https://blynk.cloud/
#define BLYNK_TEMPLATE_ID "YOUR_BLYNK_TEMPLATE_ID_HERE"
#define BLYNK_TEMPLATE_NAME "Smart Shop Guard System"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN_HERE"

// =============================================================================
// WIFI NETWORK CREDENTIALS
// =============================================================================
// Replace with your WiFi network information
#define WIFI_SSID "YOUR_WIFI_NETWORK_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// =============================================================================
// OPTIONAL: ADDITIONAL SECURITY SETTINGS
// =============================================================================
// Uncomment and configure if using additional services

// MQTT Broker (if used)
// #define MQTT_SERVER "your.mqtt.broker.com"
// #define MQTT_PORT 1883
// #define MQTT_USERNAME "your_mqtt_username"
// #define MQTT_PASSWORD "your_mqtt_password"

// Email Notifications (if implemented)
// #define SMTP_SERVER "smtp.gmail.com"
// #define SMTP_PORT 587
// #define EMAIL_SENDER "your.email@gmail.com"
// #define EMAIL_PASSWORD "your_app_password"
// #define EMAIL_RECIPIENT "alert.recipient@email.com"

#endif // CREDENTIALS_H
