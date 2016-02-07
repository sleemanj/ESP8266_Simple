ESP8266_Simple
===============

Simple to use Arduino library to interface to ESP8266 Wifi (802.11 b/g/n) Module

Firmware Version
----------------

This library was written when 0.9.2.4 was the common firmware on ESP8266 devices (roughly late 2014 through to somewhere around march-april 2015).

Subsequently I have updated it to work with 0.9.5.2, you can [get this firmware here](firmware/README.md)

Later versions (1.0 and greater) may or may not work, I have not tested.

Download, Install and Example
-----------------------------

* Download: http://sparks.gogo.co.nz/ESP8266_Simple.zip
* Open the Arduino IDE (1.0.5)
* Select the menu item Sketch > Import Library > Add Library
* Choose to install the ESP8266_Simple.zip file you downloaded
* Now you can choose File > Examples > ESP8266_Simple > HelloWorld
* Edit the SSID and PASSWORD where indicated, upload, and open the serial terminal
  
Note the standard Serial baud rate is set to 115k so be sure to select that in your terminal, we want our debugging terminal as fast as possible so that we can keep up with the ESP8266 on the SoftwareSerial connection and avoid buffer overflows.

However, **if your particular Arduino has trouble communicating on it's serial port at 115k (you get continual garbage in the serial terminal), you can drop it down, just change the value in the sketch**.  Usually it's fine, but technically speaking it's pushing limits a bit with 16MHz crystals.

Connecting To Your Arduino
--------------------------

The ESP8266 is a 3.3 Volt Module, both power and signalling is 3.3v, so you can not connect it directly to a 5v Arduino, you will need to level-shift, an example diagram given below.

This library uses SoftSerial, so that your normal hardware serial can still be used for your debugging.

The 3.3v supply must be quite "strong", the on-board 3.3v (if any) that your Arduino supplies may not be sufficient, you will most likely have to give it a 3.3v supply (of course, you must share a ground between your arduino and the ESP8266 for them to talk!)

![Example Wiring Diagram for ESP8266 Level Shifted with Zeners to Arduino](arduino-wiring-diagram.jpg?raw=true "Example Wiring Diagram")

There are a few different modules of ESP8266, this is the most common.

![Pinout Of Common (V091) ESP8266 Module](esp8266-091.jpg?raw=true "Pinout Of Common (V091) ESP8266 Module")



Usage
--------------------------

Open the HelloWorld example, it really is as simple as can be.  Also provided is an HTTP Server example.

Caveats
--------------------------

Not multi-threaded, you can request or serve one thing at a time.

Only SoftwareSerial is supported currently, although I will eventually make it work with HardwareSerial as well probably.

This is all very experimental.

Patches Welcome
--------------------------

By all means!


