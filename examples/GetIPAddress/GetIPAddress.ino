/** 
 * Copyright (C) 2014 James Sleeman
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 * 
 * @author James Sleeman, http://sparks.gogo.co.nz/
 * @license MIT License
 */

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266_Simple.h>

// These are the SSID and PASSWORD to connect to your Wifi Network
#define ESP8266_SSID  ""
#define ESP8266_PASS  ""

ESP8266_Simple wifi(8,9);

// See HelloWorld example for further comments.

void setup()
{  
  Serial.begin(115200); // Reduce this if your Arduino has trouble talking so fast
  Serial.println("ESP8266 Demo Sketch");

  wifi.begin(9600);  
  wifi.setupAsWifiStation(ESP8266_SSID, ESP8266_PASS, &Serial);    
  
  // A blank line just for debug formatting 
  Serial.println();
}

void loop()
{
  
  // This example is just to show you how to grab the IP address of your
  //  device after it has connected, in case you want to do something with
  //  it.
  
  // You can get it either as a 16 byte string, or as a long unsigned integer 
  // representation.  
  
  char          ipAddressString[16]; // 16 bytes needed
  unsigned long ipAddressLong;       // only 4 bytes needed
  
  // Either way, you get the address exactly the same.
  wifi.getIPAddress(ipAddressString); 
  wifi.getIPAddress(ipAddressLong);
  
  // And then do what you like with it.
  Serial.print("The string IP is: ");
  Serial.println(ipAddressString);  
  Serial.print("The unsigned long IP is: ");
  Serial.println(ipAddressLong);
  
  // You can convert from one to the other
  strcpy(ipAddressString,"127.0.0.1");    
  wifi.ipConvertDatatypeFromTo(ipAddressString,ipAddressLong);  
  Serial.print("The IP address ");      Serial.print(ipAddressString);
  Serial.print(" has integer value ");  Serial.println(ipAddressLong);
  
  ipAddressLong = 0x36F1256B;
  wifi.ipConvertDatatypeFromTo(ipAddressLong,ipAddressString);    
  Serial.print("The integer value ");  Serial.print(ipAddressLong);
  Serial.print(" has IP address ");    Serial.println(ipAddressString);
    
  Serial.println("End of Demo");  
  Serial.println();
  delay(5000);  
}
