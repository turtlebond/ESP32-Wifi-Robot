# ESP32-Wifi-Robot

**Introduction**  
This project utilize esp32 camera module with OV2640 to contol a robot thru wifi. This work is based on project in [1]. Esp32 module is used as wifi access point, websocket server and http server. L298N module is employed to control the dc motors. 
Once a client is connected to esp32 module, using address 192.168.4.1 in a web-browser the robot can be maneuvered.  
Codes from [2] was used to set the webpage and websocket server. Codes from arduino esp32 camera webserver example was used to set the OV2640 camera and stream it. 


**Parts**  
ESP32 camera + OV2640  
L298N Motor driver  
2 dc motor  
Robot chassis  
Battery  


**Connections**  
ESP32-CAMERA pin0,1,3 and 16 are set as PWM and connected to L298N motor driver.   
Pin4 is for built-in led.    


**Upload**  
Required USB to TTL Serial Converter to program the module. Refer[3] for connection between the the Serial converter and ESP32 module.  
Using arduino, upload the code as stated in [3]  
&nbsp;&nbsp;&nbsp;Enter programimng  mode by grounding GPI00 and press reset  
&nbsp;&nbsp;&nbsp;Go to Tools > Board and select ESP32 Wrover Module  
&nbsp;&nbsp;&nbsp;Go to Tools > Port and select the COM port the ESP32 is connected to  
&nbsp;&nbsp;&nbsp;In Tools > Partition Scheme, select “Huge APP (3MB No OTA)“  
&nbsp;&nbsp;&nbsp;Press the ESP32-CAM on-board RESET button  
&nbsp;&nbsp;&nbsp;Then, click the upload button to upload the code.  
&nbsp;&nbsp;&nbsp;Once upload done, disconnect GPIO0 from ground and press reset  



**references**  
1. https://hackaday.com/2019/02/11/little-fpv-bot-keeps-it-simple-with-an-esp32/
2. https://www.thingiverse.com/thing:3371661
3. https://randomnerdtutorials.com/esp32-cam-video-streaming-web-server-camera-home-assistant/



