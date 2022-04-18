
 # Smart-Wheelchair
![image](https://user-images.githubusercontent.com/102697479/163850656-06e660ca-49cc-49de-a5f4-69c175156153.png)


# Introductions
## Smart wheelchair based on the STM32 chip developed on the Linux platform  
This project is dedicated to the development of an intelligent wheelchair. The modular design is divided into a speed limit module, a health monitoring module, an emergency call module, a GPS positioning module and a power supply module to supply power to the system.   
**Technical requirements:**  
**Power supply module:** Power supply by battery to realize the functions of the intelligent wheelchair.  
**Main control module:** STM32 microcontroller  
**Health detection module:** Real-time detection of the user's body temperature and heart rate.   
**GPS positioning module:** can accurately locate the user's position.  
**Display module:** Real-time display of heart rate, body temperature, positioning latitude and longitude.  
**Alarm module:** When the user's body is in a condition, the temperature and heart rate thresholds can be adjusted by setting buttons, plus two LEDs to distinguish between heart rate and temperature alarms.  
**GSM module:** When the user's body condition occurs, it will immediately feedback to the guardian's mobile phone (set the content to send once every few seconds or only once: the elderly body condition, please promptly save the heart rate and body temperature value GPS).  
**Speed control module:** when the speed exceeds the specified speed (set the key to adjust the threshold), feedback to the microcontroller, the microcontroller control the operation of the motor, to achieve the purpose of emergency brake.
# Aims
Design of a smart wheelchair for the elderly with the STM32 microcontroller as the core. Implementing the functions of monitoring heart rate, body temperature, GPS positioning, speed control and display the data on an LCD screen. If the monitored data exceeds the threshold value this data is immediately sent to the guardian's mobile phone.
# Installation
<p align="center">
Digital circuit diagramming
</p>
 
 
![image](https://user-images.githubusercontent.com/93221038/163683842-c38218f7-503a-4cfa-b670-8b1953345c86.png)

##

<p align="center">
Chip pinout diagram
 </p>
 
![image](https://user-images.githubusercontent.com/93221038/163683766-bf6e5b4c-201b-4f43-98e7-d4c6168ecf35.png). 

##

<p align="center">
PCB board drawing
</p>        
             
 <div align="center"> 
<img src="https://user-images.githubusercontent.com/93221038/163684213-78e1b1f5-92fb-4b85-a4a0-5a3d691a925f.png">
</div>

# Hardware Requirements
ARM Microcontrollers-MCUs：STM32F405RGT6  
    Download interface：A2541HWV-5P  
    1.77" TFT LCD display 14PIN  
    1.77" TFT LCD display 20PIN  
    Running light  
    PCB(printed circuit board)
    
    
# Code Description
Main Functions--main.c   
Underlying drivers--bsp.c  
Parameter storage--app_System_Paramet  
LCD display driver--app_tft_lcd.c  
LCD display main interface--app_main_interface.c  
LCD display setup interface--app_machine_set.c  
STM32 Library Files--STM32 Library Files  
Operating system files--Operating system files  

# Credits
* [Xun Gong](https://github.com/gongsmith)  
* [Yuyang Wan](https://github.com/jkZoidberg)  
* [Hanwei Liang](https://github.com/BrippoLiang)
# Social Media

 <div align="center"> 

[<img src="https://user-images.githubusercontent.com/102697479/163846803-5ffc0828-5dfa-442f-956b-b11ea940175b.png" width="200" height="100" alt="youtube"/><br/>](https://www.youtube.com/watch?v=4pA6-YC3y7Y)[<img src="https://user-images.githubusercontent.com/102697479/163846955-113c0a68-9902-4f2c-83f9-a514ce7df907.png" width="200" height="100" alt="tiktok"/><br/>](https://www.tiktok.com/@smartwheelchair/video/7087989169334652166?is_copy_url=1&is_from_webapp=v1)

 </div>

 
