# Smart Shop Guard project
- A prototype of a smart automated shop that provides service, safety, and security, implemented using both **Arduino Uno** and **ESP8266** microcontrollers
- The project consists of four main systems: an **automatic cooling system**, a **firefighting system**, an **automated air conditioning system**, and a **security system** for intruder detection.

## Automatic entry detection system
This system is designed to detect customers approaching the shop using an **ultrasonic sensor** and automatically opens the door with a **servo motor**. The door remains open until the customer enters and then closes behind them, ensuring a seamless and secure entry process.
> Servo motor, Ultrasonic sensor
![image](https://github.com/user-attachments/assets/f86acbed-ca17-4638-957b-704dc8fc865c)


## Firefighting system
The firefighting system is designed to detect and respond to fire hazards immediately. It uses a **flame sensor** to detect the presence of fire. Once a fire is detected, a **buzzer** is activated to alert customers, as well as an LCD that was utilized for the same purpose, and an **AC water pump** is turned on to extinguish the fire. The pump is connected to a **relay**, which amplifies the voltage from 5 to 12 volts, ensuring that the pump operates effectively.
> Flame sensor, Relay, Buzzer, AC water pump
![Screenshot 2024-08-31 150033](https://github.com/user-attachments/assets/8c1ad3f7-ac2c-4030-a00f-9a05861aa29b)


## Automated air conditioning system
The air conditioning system monitors temperature and humidity using a DHT11 sensor. When either of these parameters exceeds a predefined threshold, a fan is activated. The fan, which requires 12V for operation, is controlled via a transistor. Since the Arduino only outputs 5V, the transistor acts as a switch to step up the voltage to 12V. Additionally, the temperature and humidity readings are displayed on an LCD screen.
> 12V Fan, DHT11 temperature & humidity sensor, LCD included with an I2C
![Screenshot 2024-08-31 151107](https://github.com/user-attachments/assets/a0c8a47a-d772-433e-973a-89955d4440b0)


# Video for the whole running system

https://drive.google.com/file/d/1etwb1SVXW_5A4gW-wf63lvFS1rl1BFbs/view?usp=sharing


