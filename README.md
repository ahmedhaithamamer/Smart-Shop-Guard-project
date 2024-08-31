# Smart Shop Guard project
- prototype of a smart automated shop that provide service ,safety and security![image](https://github.com/user-attachments/assets/26049882-d437-4d1e-b57c-f3ec01c69638)
- Security system with RFID technology, and motion sensors.

# Request testing Folder
In the request testing folder, the **HTTP requests** to/from the (Blynk and ADAfruit) cloud services are tested with the help of **inter-process communication** (**message queues**) and a **Bash script** that 
sends dummy readings to a message queue then the message is read by the Blynk and ADAfruit files simultaneously.

# Main Project Folder
In the Main Project Folder, the full system has been implemented in **Python**. Also, a **Bash script** is added to run all the 3 programs in parallel. A **socket program** was used to implement a **keep-alive** system that checks the active connection between the cloud service and the system. You can find a video for the whole running system in the following section.

# Video for the whole running system



https://github.com/xSkkarf/IoT-Smart-home-admin-and-security-system/assets/113522732/03feafff-fa0a-4c8f-8075-7c5e3dc84c7d
