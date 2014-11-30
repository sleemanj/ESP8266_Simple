HMC5883L_Simple
===============

Simple to use Arduino library to interface to ESP8266 Wifi (802.11 b/g/n) Module


Connecting To Your Arduino
--------------------------

The ESP8266 is a 3.3 Volt Module, both power and signalling is 3.3v, so you can not connect it directly to a 5v Arduino, you will need to level-shift, an example diagram given below.

This library uses SoftSerial, so that your normal hardware serial can still be used for your debugging.

The 3.3v supply must be quite "strong", the on-board 3.3v (if any) that your Arduino supplies may not be sufficient, you will most likely have to give it a 3.3v supply (of course, you must share a ground between your arduino and the ESP8266 for them to talk!)

![Example Wiring Diagram for ESP8266 Level Shifted with Zeners to Arduino](arduino-wiring-diagram.jpg?raw=true "Example Wiring Diagram")