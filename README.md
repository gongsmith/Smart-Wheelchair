# Smart-Wheelchair 
# Introductions
    Intelligent wheelchair control system design based on embedded microcontroller
    This project is dedicated to the development of an intelligent wheelchair. The modular design is divided into a speed limit module, a health monitoring module, an emergency call module, a GPS positioning module and a power supply module to supply power to the system. Technical requirements:
    Power supply module:Power supply by battery to realize the functions of the intelligent wheelchair.
    Main control module: STM32 microcontroller
    Health detection module: Real-time detection of the user's body temperature and heart rate. 
    GPS positioning module: can accurately locate the user's position.
    Display module: Real-time display of heart rate, body temperature, positioning latitude and longitude.
    Alarm module: When the user's body is in a condition, the temperature and heart rate thresholds can be adjusted by setting buttons, plus two LEDs to distinguish between heart rate and temperature alarms.
    GSM module: When the user's body condition occurs, it will immediately feedback to the guardian's mobile phone (set the content to send once every few seconds or only once: the elderly body condition, please promptly save the heart rate and body temperature value GPS).
    Speed control module: when the speed exceeds the specified speed (set the key to adjust the threshold), feedback to the microcontroller, the microcontroller control the operation of the motor, to achieve the purpose of emergency brake.
# Aims
Design of a smart wheelchair for the elderly with the STM32 microcontroller as the core. Implementing the functions of monitoring heart rate, body temperature, GPS positioning, speed control and display the data on an LCD screen. If the monitored data exceeds the threshold value this data is immediately sent to the guardian's mobile phone.
# Installation
![image](https://user-images.githubusercontent.com/93221038/163683842-c38218f7-503a-4cfa-b670-8b1953345c86.png)
![image](https://user-images.githubusercontent.com/93221038/163683766-bf6e5b4c-201b-4f43-98e7-d4c6168ecf35.png)
![image](https://user-images.githubusercontent.com/93221038/163683712-fc8d5e89-1047-49b7-a9df-d292de64a237.png)


# Hardware Requirements
    ARM Microcontrollers-MCUs：STM32F405RGT6
    A2541HWV-5P
    XL1509-5.0E
    A2541HWV-2P
    AMS1117-3.3
    A2541HWV-4P
    A2541HWV-6P
    MMBTA05
    MMBT3904
    MMBT3904LT1G
    WH160-1-B102(B1K)
    KEY_SKRPACE010
    1.77" TFT LCD display 14PIN
    1.77" TFT LCD display 20PIN
    Running light
    PCB(printed circuit board)
# Credits
* [Xun Gong](https://github.com/gongsmith)  
* [Yuyang Wan](https://github.com/jkZoidberg)  
* [Hanwei Liang](https://github.com/BrippoLiang)
# Social Media

