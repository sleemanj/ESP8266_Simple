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

// Please note, the Arduino IDE is a bit retarded, if the below define has an
// underscore other than _h, it goes mental.  Wish it wouldn't  mess
// wif ma files!
#ifndef ESP8266Simple_h
#define ESP8266Simple_h

#define ESP8266_SOFTWARESERIAL 1
#define ESP8266_HARDWARESERIAL 0

#define ESP8266_OK             0
#define ESP8266_ERROR          1
#define ESP8266_TIMEOUT        2
#define ESP8266_OVERFLOW       3
// READY might be better set to ESP8266_OK
#define ESP8266_READY          4
#define ESP8266_BUSY           5

#define ESP82336_DEBUG(...)   Serial.print(__VA_ARGS__); 
#define ESP82336_DEBUGLN(...) Serial.println(__VA_ARGS__); 

#define ESP82336_DEBUG(...)
#define ESP82336_DEBUGLN(...)

#ifndef ESP8266_SERIALMODE
  #define ESP8266_SERIALMODE   ESP8266_SOFTWARESERIAL
#endif

#define ESP8266_STATION 1
#define ESP8266_AP      2
#define ESP8266_BOTH    3

#include "ESP8266_Serial.h"

class ESP8266_Simple
{
  
    public:
            
#if ESP8266_SERIALMODE == ESP8266_SOFTWARESERIAL
      ESP8266_Simple(short rxPin, short txPin);      
#endif
      
#if ESP8266_SERIALMODE == ESP8266_HARDWARESERIAL
      ESP8266_Simple();      
#endif
                  
      // Connect to the ESP8266 device
      byte begin(long baudRate);    
      
      // Super Easy Commands
      byte setupAsWifiStation(const char *SSID, const char *Password, Print *debugPrinter = NULL);
      
      unsigned int GET(const __FlashStringHelper *serverIp, int port, char *requestPathAndResponseBuffer, int bufferLength, const __FlashStringHelper *httpHost = NULL, int bodyResponseOnlyFromLine = 1);
      unsigned int GET(unsigned long serverIp, int port, char *requestPathAndResponseBuffer, int bufferLength, const __FlashStringHelper *httpHost = NULL, int bodyResponseOnlyFromLine = 1);
      
      
      // More General/Advanced Commands
      byte reset();      
      byte getFirmwareVersion(long &versionResponse);            // firmware version put int versionResponse
      byte setWifiMode(byte mode);                               // ESP8266_STATION, ESP8266_AP, ESP8266_BOTH
      byte getAccessPointsList(char *buffer, int bufferSize );   // puts into buffer
      byte getIPAddress(unsigned long &ipAddress);               // ip address put into ipAddress (as 32 bits)
      byte getIPAddress(char *ipAddress);                        // ip address put into ipAddress (as 15 byte + 1 null)
      byte setTimeout(int seconds);                              // 0-28800
                
      // Station Mode, returns IPv4 address (as 4 bytes)
      long connectToWifi(const char *SSID, const char *Password);          
      
      
      // https://scargill.wordpress.com/2014/10/08/esp8266-as-an-access-point/
      // Access Point Mode, returns IPv4 address (as 4 bytes)
      // @TODO
      // long createWifiNetwork(const char *SSID, const char *Password, byte Encryption, byte Channel);
     
      // Disconnect from access point
      byte disconnectFromWifi();            
      
      byte startHttpServer(unsigned int port, int (* requestHandler)(char *buffer, int requestLength, int bufferLength));
      byte stopHttpServer();
      
      // Implements a naieve HTTP server. When a get request comes in, it it passed to
      // a callback function, this callback function should examine the contents of 
      //  the buffer[0..requestLength-1] 
      //  if can then write a response into the buffer[0..bufferLength-1]
      //  and return an integer response code combined with a bitmask that indicates
      //  response type
      //
      //   return ESP8266_HTML & 200;
      //   return ESP8266_TEXT & 200;
      //   return ESP8266_TEXT & 404;
      //   return ESP8266_RAW  & 200; --- RAW will mean that you have put headers into the buffer
      //
      //  The function will return 0 for nothing served, or the response code we issued otherwise
      int serveHttpRequest();
      
      // Issue an HTTP Get Request to some destination IP address
      // the request string, null terminated, is placed in buffer      
      // the response code from the server is returned
      // the first line of the response body,
      // up to bufferLength-1 bytes, not including newline is put into buffer
      // headers are not returned
      //
      byte sendHttpRequest(unsigned long serverIpAddress, int port, char *requestPathAndResponseBuffer, int bufferLength, char *httpHost = NULL, int bodyResponseOnlyFromLine = 1, int *httpResponseCode = NULL);
      
      
      // Convert Dotted quad (123.123.123.123) into 32 bits
      void ipConvertDatatypeFromTo(const char *ipAddressString, unsigned long &ipAddressLong);
      
      // Convert 32 bits into a dotted quad (123.123.123.123)
      // IMPORTANT!  buffer must be 16 bytes long (12 bytes for digits, 3 bytes for dots, and a null termination byte)
      void ipConvertDatatypeFromTo(unsigned long ipAddressLong, char *ipAddressStringBuffer); // Make sure your ipAddressString is at least 16 bytes (null termination)
      
      // Send command and get response into a buffer
      byte sendCommand(const char *cmd, char *responseBuffer, int responseBufferLength, byte getResponseFromLine = 1);
      byte sendCommand(const __FlashStringHelper *cmd, char *responseBuffer, int responseBufferLength, byte getResponseFromLine = 1);
      
      // Send command without getting the response string, we still wait for an OK or ERROR and return that
      // as a return code....
      //    0 = OK
      //    1 = ERROR
      //    2 = TIMEOUT WAITING FOR RESPONSE
      byte sendCommand(const char *cmd);
      byte sendCommand(const __FlashStringHelper *cmd);
      
      // Send a command consisting of multiple strings to be concatenated
      byte sendCommand(const char **cmdPartsToConcatenate, byte numParts, char *responseBuffer, int responseBufferLength, byte getResponseFromLine);
      
      void clearSerialBuffer();
      
      // Some help for debugging
      void getErrorMessage(byte responseCode, char *bufferWithMinLength50Char);            
      void debugPrintError(byte responseCode, Print *debugPrinter); // you can pass &Serial to debugPrinter
     
    private:
      #if ESP8266_SERIALMODE == ESP8266_SOFTWARESERIAL
      //   SoftwareSerial *espSerial;
      ESP8266_Serial *espSerial;
      #endif
        
      #if ESP8266_SERIALMODE == ESP8266_HARDWARESERIAL
        HardwareSerial *espSerial;
      #endif
          
      unsigned long generalCommandTimeoutMicroseconds;
             
    protected:
      unsigned int readIPD(char *responseBuffer, int responseBufferLength, int bodyResponseOnlyFromLine = 1, int *parseHttpResponse = NULL);
      byte         unlinkConnection();
      
};

#endif

