HMC5883L_Simple
===============

Simple to use Arduino library to interface to ESP8266 Wifi (802.11 b/g/n) Module

Download, Install and Example
-----------------------------

* Download: http://sparks.gogo.co.nz/ESP8266_Simple.zip
* Open the Arduino IDE (1.0.5)
* Select the menu item Sketch > Import Library > Add Library
* Choose to install the ESP8266_Simple.zip file you downloaded
* Now you can choose File > Examples > ESP8266_Simple > HelloWorld
* Edit the SSID and PASSWORD where indicated, upload, and open the serial terminal
  
Note the standard Serial baud rate is set to 115k so be sure to select that in your terminal, we want our debugging terminal as fast as possible so that we can keep up with the ESP8266 on the SoftwareSerial connection and avoid buffer overflows.

Connecting To Your Arduino
--------------------------

The ESP8266 is a 3.3 Volt Module, both power and signalling is 3.3v, so you can not connect it directly to a 5v Arduino, you will need to level-shift, an example diagram given below.

This library uses SoftSerial, so that your normal hardware serial can still be used for your debugging.

The 3.3v supply must be quite "strong", the on-board 3.3v (if any) that your Arduino supplies may not be sufficient, you will most likely have to give it a 3.3v supply (of course, you must share a ground between your arduino and the ESP8266 for them to talk!)

![Example Wiring Diagram for ESP8266 Level Shifted with Zeners to Arduino](arduino-wiring-diagram.jpg?raw=true "Example Wiring Diagram")

Usage
--------------------------

Open the HelloWorld example, it really is as simple as can be.

Status
--------------------------

Only making HTTP GET requests are supported so far.  
HTTP Server mode will be implemented next.
Currently only the ESP8266 single connection at a time mode is supported, not MUX.
Only SoftwareSerial is supported currently, although I will eventually make it work with HardwareSerial as well probably.

Patches Welcome
--------------------------

By all means!


