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

#if 0
#define ESP82336_DEBUG(...)   Serial.print(__VA_ARGS__); 
#define ESP82336_DEBUGLN(...) Serial.println(__VA_ARGS__); 
#else
#define ESP82336_DEBUG(...)
#define ESP82336_DEBUGLN(...)
#endif

#ifndef ESP8266_SERIALMODE
  #define ESP8266_SERIALMODE   ESP8266_SOFTWARESERIAL
#endif

#define ESP8266_STATION 1
#define ESP8266_AP      2
#define ESP8266_BOTH    3

#define ESP8266_HTML    0x01000000
#define ESP8266_TEXT    0x02000000
#define ESP8266_RAW     0x04000000

#include "ESP8266_Serial.h"

struct ESP8266_HttpServerHandler
{
    const char     *requestMatches;
    unsigned long (* handlerFunction)(char *, int);
};

class ESP8266_Simple
{
  
    public:
            
#if ESP8266_SERIALMODE == ESP8266_SOFTWARESERIAL
      ESP8266_Simple(short rxPin, short txPin);      
#endif
      
#if ESP8266_SERIALMODE == ESP8266_HARDWARESERIAL
      // This isn't going to work yet, TBD
      ESP8266_Simple();      
#endif
                  
      /**
       * Begin the ESP8266 Connection
       * 
       * @param baudRate The rate to communicate with the device, typically 9600 
       * @return ESP8266_OK, or an error code
       */
      
      byte begin(long baudRate);    
      
      /** 
       * Connect to an existing WIFI network and get an IP address from it with DHCP.
       * Optionally print information to a Print class (eg, "&Serial")
       * 
       * @param SSID The SSID to connect to.
       * @param Password The Password for this wifi network.
       * @param debugPrinter An optional place to print some information (eg, &Serial)
       * 
       * @return ESP8266_OK, or an error code
       */
      
      byte setupAsWifiStation(const char *SSID, const char *Password, Print *debugPrinter = NULL);
      
      /**
       * Perform an HTTP GET operation to get data from a server on the network (or internet).
       *  F() macro version.
       * 
       * See the HelloWorld example for more information.
       * 
       * @param serverIp The IP address of the server provided via the F() macro (eg, F("127.0.0.1"))
       * @param port     The port to connect to
       * @param requestPathAndResponseBuffer A buffer location which contains the request and will
       *   be over-written with the response.  THe request consists of a path string (eg, "/foo")
       * @param bufferLength The length of the buffer in bytes.
       * @param httpHost The hostname you are connecting to, required for virtual servers with more
       *  than one host per IP, and if you wish to get headers (eg, F("example.com"))
       * @param bodyResponseOnlyFromLine If set to 1, only the body is returned, if set to 0, the 
       *   heaaders are also returned, if set to -1, only the headers are returned, if set to a
       *   number greater than 1, only the body from that line number is returned.
       * 
       * @return  ESP8266_OK, or an error code
       */
      
      unsigned int GET(const __FlashStringHelper *serverIp, int port, char *requestPathAndResponseBuffer, int bufferLength, const __FlashStringHelper *httpHost = NULL, int bodyResponseOnlyFromLine = 1);
      
      /** Start an "HTTP Server" with a number of "handlers" provided to serve various
       *   requests.
       * 
       * See the HTTP_Server example for more information.
       * 
       * @param port The port to open, usually 80 for HTTP
       * @param httpServerHandlers an array of handlers, see the HTTP_Server example for 
       *    demonstration of this.
       * @param numOfHandlers the size of said array of handlers
       * @param maxBufferSize the size of the buffer to use when serving requests, this
       *  buffer must be big enough to hold your desired response(s) in full.
       * @param debugPrinter   An optional place to print some information (eg, &Serial)
       * 
       * @return ESP8266_OK, or an error code
       */
      byte startHttpServer(unsigned port, ESP8266_HttpServerHandler *httpServerHandlers, unsigned int numOfHandlers, unsigned int maxBufferSize= 250, Print *debugPrinter = NULL);
      
      
      unsigned int GET(unsigned long serverIp, int port, char *requestPathAndResponseBuffer, int bufferLength, const __FlashStringHelper *httpHost = NULL, int bodyResponseOnlyFromLine = 1);
      
      
      // More General/Advanced Commands
      byte reset();      
      byte getFirmwareVersion(long &versionResponse);            // firmware version put into versionResponse
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
      

      byte startHttpServer(unsigned int port, unsigned long (* requestHandler)(char *buffer, int bufferLength), unsigned int maxBufferSize = 250);
      byte stopHttpServer();
      
      // Implements a naieve HTTP server. When a get request comes in, it it passed to
      // a callback function, this callback function should examine the contents of 
      //  the buffer[0..requestLength-1] 
      //  if can then write a response into the buffer[0..bufferLength-1]
      //  and return an integer response code combined with a bitmask that indicates
      //  response type
      //
      //   return ESP8266_HTML | 200;
      //   return ESP8266_TEXT | 200;
      //   return ESP8266_TEXT | 404;
      //   return ESP8266_RAW  | 200; --- RAW will mean that you have put headers into the buffer
      //
      //  returns ESP8266_OK/ERROR
      byte serveHttpRequest();
      
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
      unsigned int readIPD(char *responseBuffer, int responseBufferLength, int bodyResponseOnlyFromLine = 1, int *parseHttpResponse = NULL, int *muxChannel = NULL);
      byte         unlinkConnection();
      
      
      
      unsigned long (* httpServerRequestHandler)(char *, int );
      unsigned int  httpServerMaxBufferSize;
         
      
      unsigned long              httpServerRequestHandler_Builtin(char *buffer, int bufferLength);
      ESP8266_HttpServerHandler *httpServerHandlers;
      unsigned int               httpServerHandlersLength;      
      
};


#endif

