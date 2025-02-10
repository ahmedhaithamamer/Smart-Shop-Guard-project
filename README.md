# Smart Shop Guard Project

![Smart Shop Prototype](https://github.com/user-attachments/assets/817044e5-c3e2-4efb-a62e-8bb6452c51aa)

This project is a prototype of a smart automated shop that provides service, safety, and security. It is implemented using **Arduino Uno** and **ESP8266** microcontrollers and features four main systems:

- **Automatic Cooling System**
- **Firefighting System**
- **Automated Air Conditioning System**
- **Security System** for intruder detection

## Automatic Entry Detection System

This system detects customers approaching the shop using an **ultrasonic sensor** and automatically opens the door with a **servo motor**. The door remains open until the customer enters, then closes behind them, ensuring a seamless and secure entry process.

> **Components:**
> - Servo Motor
> - Ultrasonic Sensor

![Automatic Entry Detection](https://github.com/user-attachments/assets/f86acbed-ca17-4638-957b-704dc8fc865c)

## Firefighting System

The firefighting system is designed to detect and respond to fire hazards immediately. It uses a **flame sensor** to detect the presence of fire. Upon detection, a **buzzer** and an LCD display alert customers, and an **AC water pump** is activated to extinguish the fire. The pump is controlled via a **relay** to step up the voltage from 5V to 12V, ensuring effective operation.

> **Components:**
> - Flame Sensor
> - Relay
> - Buzzer
> - AC Water Pump

![Firefighting System](https://github.com/user-attachments/assets/8c1ad3f7-ac2c-4030-a00f-9a05861aa29b)

## Automated Air Conditioning System

This system monitors temperature and humidity using a **DHT11 sensor**. When either parameter exceeds a predefined threshold, a 12V fan is activated. The fan is controlled via a transistor, which steps up the voltage from 5V (provided by the Arduino) to 12V. Temperature and humidity readings are displayed on an LCD screen.

> **Components:**
> - 12V Fan
> - DHT11 Temperature & Humidity Sensor
> - LCD with I2C

![Automated Air Conditioning System](https://github.com/user-attachments/assets/a0c8a47a-d772-433e-973a-89955d4440b0)

## After Assembling

![Assembled Prototype](https://github.com/user-attachments/assets/9f83cc28-a480-4b8b-b2f9-9614dc355d6c)

## Videos of the Running System

- **Automatic Entry Detection System:**
  [Watch Video](https://github.com/user-attachments/assets/28712764-492d-467e-9aa7-fb2b8b069abf)

- **Firefighting System:**
  [Watch Video](https://github.com/user-attachments/assets/1052b2f2-3211-4acc-8836-fc73112fbd1a)

- **LCD Warning:**
  [Watch Video](https://github.com/user-attachments/assets/c1f41d64-3401-45b4-99e5-8f7c6153e420)

- **Automated Air Conditioning System:**
  [Watch Video](https://github.com/user-attachments/assets/e4350ce7-36f6-45f2-aca9-62d095d811f2)

Copyright © 2025 Ahmed Haitham Amer

