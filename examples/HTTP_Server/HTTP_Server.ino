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
//  put details appropriate for your network between the quote marks,
//  eg  #define ESP8266_SSID "YOUR_SSID"
#define ESP8266_SSID  ""
#define ESP8266_PASS  ""

// Create the ESP8266 device on pins 
//   8 for Arduino RX (TX on ESP connects to this pin) 
//   9 for Arduino TX (RX on ESP connects to this pin)
//
// REMEMBER!  The ESP8266 is a 3v3 device, if your arduino is 
//   5v powered, you MUST "level shift" TX/RX to 3v3, a zener 
//   like this will work, do it for both TX and RX
//
// [ARDUINO 8] => [1k Resistor] => + => [ESP8266 TX]
//                                 |
//                                 + => [(cathode) 3v3 Zener Diode (anode) ] => GROUND
//
// [ARDUINO 9] => [1k Resistor] => + => [ESP8266 RX]
//                                 |
//                                 + => [(cathode) 3v3 Zener Diode (anode) ] => GROUND
//
// The ESP8266 RST pin and CH_PD pin must both be connected to 3v3 
// (best via a 1k resistor).  The PWR must go to 3v3, and GND to
//  ground of course.   The other pins can be left floating for 
//  normal operation.

ESP8266_Simple wifi(8,9);

void setup()
{
  // As usual, we will output debugging information to the normal
  // serial port, the wifi runs on SoftwareSerial using the pins 
  // set above so it does not interfere with your normal debugging.
  Serial.begin(115200); // Reduce this if your Arduino has trouble talking so fast
  Serial.println("ESP8266 Demo Server Sketch");

  // Set the baud rate, this depends on your ESP8266's previous setup
  // as it remembers the rate.  9600 is the default for recent firmware
  // and is the recommended speed since we will be using SoftwareSerial
  // to communicate with it.
  wifi.begin(9600);
  
  // Connect to the given Wifi network as a station (as opposed to an 
  // Access Point running it's own Wifi network).
  //
  // See how &Serial is passed, the library will print status and debug
  // information there when it is connecting. 
  //
  // Note that this will not return until it has successfully connected,
  // it will just keep retrying.  I like to keep things simple!  
  wifi.setupAsWifiStation(ESP8266_SSID, ESP8266_PASS, &Serial);
  

  // Define our server handlers, each handler has a PSTR() which is matched
  //  against an incoming request, and a function name which will be called
  //  to handle the request.  The functions are defined further below in this
  //  file.
  //
  // The handlers arematches from top to bottom, first one that matches "wins"
  //  this means that the "GET " handler goes at the bottom of the list and
  //  that way will catch anything that didn't get processed.
  //
  // CAUTION! Make sure you declare this static (or make it a global).  
  static ESP8266_HttpServerHandler myServerHandlers[] = {
    { PSTR("GET /millis"), httpMillis },    
    { PSTR("GET /led"),    httpLed    },
    { PSTR("GET "),        http404    } 
  };
  
  // Start an "HTTP Server" on port 80, using our handlers above to process
  // the requests, with a maximum buffer size of 250 bytes.  Note that
  // your response to any requests must fit within the buffer size, and
  // of course, the buffer must fit inside your available RAM while handling
  // a request (when not handling a request, the buffer does not take ram).  
  wifi.startHttpServer(80, myServerHandlers, sizeof(myServerHandlers), 250, &Serial);
  Serial.println("( Now you can use your web browser to hit the IP address above. )");
  
  // A blank line just for debug formatting 
  Serial.println();
}


void loop()
{        
  wifi.serveHttpRequest(); 
  
  // You should call wifi.serveHttpRequest() as often as possible to ensure 
  //  that you don't miss requests.  Also, when you are making a request 
  //  to your "Arduino Web Server", keep it to one request at a time, we
  //  do not have any multithreading or queue ability.
  
  return;  
}

// An example of a server handler, this one simply outputs the millis()
// as an HTML page, notice how we zero-out the buffer, then put our response
// html into it, and then
//    return ESP8266_HTML | 200
// which means this is a text/html response and the status code is 200

unsigned long httpMillis(char *buffer, int bufferLength)
{
  // empty the buffer
  memset(buffer,0,bufferLength);
  
  // Fill it with our HTML response (note, take care about the bufferLength 
  //  available, your entire response must fit in the buffer.
  strncpy_P(buffer, PSTR("<h1>Millis</h1><p>The current millis() are: "), bufferLength-strlen(buffer));
  ultoa(millis(),buffer+strlen(buffer),10);
  strncpy_P(buffer+strlen(buffer), PSTR("</p>"), bufferLength-strlen(buffer));        

  // And return the type and HTTP response code combined with "|" (bitwise or)
  // Valid  types are: ESP8266_HTML, ESP8266_TEXT, ESP8266_RAW
  // Valid  response codes are: any standard HTTP response code
  // The RAW type is sent without adding any headers, the other types add HTTP 
  // headers appropriately.
  
  return ESP8266_HTML | 200;  
}

// An example of a server handler, this one toggles an LED on pin D13
// and outputs the new status of the LED as a text/plain response using 
//    return ESP8266_TEXT | 200
// which means this is a text/plain response and the status code is 200

unsigned long httpLed(char *buffer, int bufferLength)
{  
  static byte ledStatus = 0;
  
  // Let's toggle the usual D13 LED
  pinMode(13, OUTPUT);
  if(ledStatus)
  { 
    digitalWrite(13, LOW);
    ledStatus = 0;
  }
  else
  {
    digitalWrite(13, HIGH);
    ledStatus = 1;
  }    
  
  // And give a report of it's new status
  memset(buffer,0,bufferLength);
  strncpy_P(buffer, ledStatus ? PSTR("The D13 LED Status Is: ON") : PSTR("The D13 LED Status Is: OFF"), bufferLength-strlen(buffer));
  
  // And return the type and HTTP response code combined with "|" (bitwise or)
  // Valid  types are: ESP8266_HTML, ESP8266_TEXT, ESP8266_RAW
  // Valid  response codes are: any standard HTTP response code (typically, 200 for OK, 404 for not found, and 500 for error)
  
  return ESP8266_TEXT | 200;
}

// And finally this example provides a helpful 404 response when the user requests 
// a "page" that does not exist.

unsigned long http404(char *buffer, int bufferLength)
{  
  memset(buffer, 0, bufferLength);  
  strcpy_P(buffer, PSTR("<h1>Error, Unknown Command</h1>\r\n<p>Try <a href=\"/millis\">/millis</a>, and <a href=\"/led\">/led</a></p>"));
  return ESP8266_HTML | 404;
}
