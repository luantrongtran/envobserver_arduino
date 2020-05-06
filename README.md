# EnvObserver

EnvObserver is a a hobby project I created to record temperature, humidity, and soil moisture. This has 3 modules:

*   EnvObserver_arduino: which is an Arduino project (Arduino wifi board + sensors) to record temperature, humidity and soil moisture
*   [EnvObserver](https://github.com/luantrongtran/envobserver): web services to which is the endpoint where Arduino units upload data
*  [EnvObserver\_mobile](https://github.com/luantrongtran/envobservers_mobile): a hybrid mobile app to see the data uploaded by Arduino


##A. Setup Arduino IDE
###1. Instal driver
*   Download the driver (for Mac) from the following link:

[https://raw.githubusercontent.com/nodemcu/nodemcu-devkit/master/Drivers/CH341SER_MAC.ZIP](https://raw.githubusercontent.com/nodemcu/nodemcu-devkit/master/Drivers/CH341SER_MAC.ZIP)



###2. Install arduino Libraries
*   Install RTC DS3231 library named "RTC by Makuna" by Michael C. Miller




###3. Board Setup
*   Install driver (if using Mac)
*   Select the board in Arduino: tool -> Board -> "Node MCU 1.0 (ESP-12E Module)"




###4. Port selection

    There are different models of ESP8266 nodeMCU, therefore, the port appearing in the Arduino may vary. A way to know which is the port of the ESP8266 is to compare the port list before and after the board plugged into the PC.
    Otherwise, try port names containing “1430”

###5. Upload code on ESP8266 board
    1. Download source code at [https://codeload.github.com/luantrongtran/envobserver_arduino/zip/master](https://codeload.github.com/luantrongtran/envobserver_arduino/zip/master) 
    2. Open Android IDE
    3. Unzip and Open the downloaded folder
    4. Select board and port as above instruction
    5. Plug the ESP8266 into your PC
    6. Click “Upload” button, which is the arrow icon on top left corner
 





#B. Schematic
##1. Humidity & Temperature Sensor

<table>
  <tr>
   <td>
DH11 / DH12
   </td>
   <td>ESP8266
   </td>
  </tr>
  <tr>
   <td>+
   </td>
   <td>3.3v
   </td>
  </tr>
  <tr>
   <td>-
   </td>
   <td>Ground
   </td>
  </tr>
  <tr>
   <td>out
   </td>
   <td>D3
   </td>
  </tr>
</table>




##2. RTC (DS3231

<table>
  <tr>
   <td>
DS3231
   </td>
   <td>esp8266
   </td>
  </tr>
  <tr>
   <td>+
   </td>
   <td>3.3v
   </td>
  </tr>
  <tr>
   <td>-
   </td>
   <td>Ground
   </td>
  </tr>
  <tr>
   <td>SDA
   </td>
   <td>D2
   </td>
  </tr>
  <tr>
   <td>SCL
   </td>
   <td>D1
   </td>
  </tr>
</table>




##3. Soil Sensor

<table>
  <tr>
   <td>
Moisture Sensor
   </td>
   <td>ESP8266
   </td>
  </tr>
  <tr>
   <td>+
   </td>
   <td>3.3v
   </td>
  </tr>
  <tr>
   <td>-
   </td>
   <td>Ground
   </td>
  </tr>
  <tr>
   <td>out
   </td>
   <td>A0
   </td>
  </tr>
</table>



# 



#3. Mobile app

    The mobile app can be used to control EnvObserver devices, and observe their data.
    Github: https://github.com/luantrongtran/envobservers_mobile


# 


#4. Arduino Source Code
 Github: https://github.com/luantrongtran/envobserver_arduino
