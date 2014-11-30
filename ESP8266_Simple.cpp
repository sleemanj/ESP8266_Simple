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
// PLEASE NOTE!
// The Arduino IDE is a bit braindead, even though we include SoftwareSerial.h here, it does nothing
// you must include SoftwareSerial.h in your main sketch, the Arduino IDE will not include Wire
// in the build process otherwise.

#include "ESP8266_Simple.h"
#include "ESP8266_Serial.h"

#if ESP8266_SERIALMODE == ESP8266_SOFTWARESERIAL
ESP8266_Simple::ESP8266_Simple(short rxPin, short txPin)
{
  // this->espSerial = new SoftwareSerial(rxPin,txPin);
  this->espSerial = new ESP8266_Serial(rxPin,txPin);
  this->generalCommandTimeoutMicroseconds = 2000000;
}
#endif

#if ESP8266_SERIALMODE == ESP8266_HARDWARESERIAL
ESP8266_Simple::ESP8266_Simple()
{
  this->espSerial = &Serial;
  this->generalCommandTimeoutMicroseconds = 2000000;
}
#endif

/** Connect to ESP8266 Device */
byte ESP8266_Simple::begin(long baudRate)
{  
  this->espSerial->begin(baudRate);  
  return ESP8266_OK;
}

byte ESP8266_Simple::setupAsWifiStation(const char *SSID, const char *Password, Print *debugPrinter)
{
  byte responseCode;
  
  // Reset the ESP8266 Device (soft reset)
  do
  { // Keep trying to reset until it works
    if(debugPrinter)
    {
      debugPrinter->print(F("Reset: "));
    }
    
    responseCode = this->reset();
    if(responseCode == ESP8266_OK)
    {
      if(debugPrinter)
      {
        debugPrinter->println("OK");
      }
    }
    else
    {
      if(debugPrinter)
      {
        this->debugPrintError(responseCode, debugPrinter);      
      }
      delay(1000);
    }
  } while(responseCode != ESP8266_OK);
  
  
  
  // Connect To The Wifi Network
  do
  { // Keep trying to connect  until it works
    if(debugPrinter)
    {
      debugPrinter->print(F("Connect: "));
    }
    responseCode = this->connectToWifi(SSID,Password);
    if(responseCode == ESP8266_OK)
    {
      if(debugPrinter)
      {
        debugPrinter->println(F("OK"));
      }              
    }
    else
    {
      if(debugPrinter)
      {
        this->debugPrintError(responseCode, debugPrinter);      
      }
      delay(1000);
    }
  } while(responseCode != ESP8266_OK);
  
  // Print Our IP Address
  char ipAddressString[16];
  do
  { // Keep trying to connect  until it works
    if(debugPrinter)
    {
      debugPrinter->print(F("IP Address: "));      
    }
    responseCode = this->getIPAddress(ipAddressString);
    if(responseCode == ESP8266_OK)
    {
      if(debugPrinter)
      {
        debugPrinter->println(ipAddressString);                
      }      
    }
    else
    {
      if(debugPrinter)
      {
        this->debugPrintError(responseCode, debugPrinter);      
      }      
      delay(1000);
    }
  } while(responseCode != ESP8266_OK);

  
  
}

unsigned int ESP8266_Simple::GET(const __FlashStringHelper *serverIp, int port, char *requestPathAndResponseBuffer, int bufferLength, const __FlashStringHelper *httpHost, int bodyResponseOnlyFromLine)
{
  if(!serverIp)                       return ESP8266_ERROR;
  char serverIpBuffer[strlen_P((const char *)serverIp)+1];
  strcpy_P(serverIpBuffer, (const char *) serverIp); 
  
  unsigned long serverIpLong;
  this->ipConvertDatatypeFromTo(serverIpBuffer, serverIpLong);
  return this->GET(serverIpLong, port, requestPathAndResponseBuffer, bufferLength, httpHost, bodyResponseOnlyFromLine);
}

unsigned int ESP8266_Simple::GET(unsigned long serverIp, int port, char *requestPathAndResponseBuffer, int bufferLength, const __FlashStringHelper *httpHost, int bodyResponseOnlyFromLine)
{
  if(!serverIp)                       return ESP8266_ERROR;
  if(!requestPathAndResponseBuffer)   return ESP8266_ERROR;
  
  char serverIpBuffer[strlen_P((const char *)serverIp)+1];
  strcpy_P(serverIpBuffer, (const char *) serverIp); 
  
  int  httpResponseCode = 0;
  byte responseCode;
  
  if(httpHost && strlen_P((const char *)httpHost))
  {
    char httpHostBuffer[strlen_P((const char *)httpHost)+1];
    strcpy_P(httpHostBuffer, (const char *)httpHost);
    responseCode = this->sendHttpRequest(serverIp, port, requestPathAndResponseBuffer, bufferLength, httpHostBuffer, bodyResponseOnlyFromLine, &httpResponseCode); 
  } 
  else
  {
    responseCode = this->sendHttpRequest(serverIp, port, requestPathAndResponseBuffer, bufferLength, NULL, bodyResponseOnlyFromLine, &httpResponseCode);     
  }
  
  if(responseCode != ESP8266_OK)
  {
    return responseCode;
  }
  
  return httpResponseCode;
}

/** Reset the device (soft reset) */
byte ESP8266_Simple::reset()
{   
  byte remainingAttempts = 5;
  
  while ( this->sendCommand(F("AT+RST")) != ESP8266_OK )
  {
    if(--remainingAttempts == 0) return ESP8266_ERROR;
    delay(1000);    
  }
  remainingAttempts = 5;
  
  // delay(4000);
  // Once the reset is issued OK, try to issue an AT command
  // and wait until that works
  while ( this->sendCommand("AT") != ESP8266_OK )
  {
    if(--remainingAttempts == 0) return ESP8266_ERROR;
    delay(1000);    
  }
  
  ESP82336_DEBUGLN("RESET OK");
  
  return ESP8266_OK;  
}

byte ESP8266_Simple::getFirmwareVersion(long &versionResponse)
{
  char buffer[11];
  byte responseCode = this->sendCommand(F("AT+GMR"), buffer, 11);
  if(responseCode == ESP8266_OK)
  {
    buffer[10] = 0; // Ensure string is terminated after 10 characters
    versionResponse = atol(buffer);    
  }
  return responseCode;
}

byte ESP8266_Simple::setWifiMode(byte mode)
{
  char modeBuff[12];
  strcpy_P(modeBuff, PSTR("AT+CWMODE="));
  itoa((int)mode,modeBuff+10,10);
  return this->sendCommand(modeBuff);
}

byte ESP8266_Simple::getAccessPointsList(char *buffer, int bufferSize )
{
  return this->sendCommand(F("AT+CWLAP"), buffer, bufferSize);  
}

byte ESP8266_Simple::getIPAddress(unsigned long &ipAddress)
{
  char buffer[16]; // [3].[3].[3].[3][NUL]
  byte errCode;
  
  errCode     = this->sendCommand(F("AT+CIFSR"), buffer, sizeof(buffer));
  if(errCode) return errCode;
  
  this->ipConvertDatatypeFromTo(buffer, ipAddress);
  
  return ESP8266_OK;
}

byte ESP8266_Simple::getIPAddress(char * ipAddress)
{    
  unsigned long ipAddressLong;
  byte errCode;
  errCode = this->getIPAddress(ipAddressLong);
  if(errCode) return errCode;
  
  this->ipConvertDatatypeFromTo(ipAddressLong, ipAddress);    
  return ESP8266_OK;
}

byte ESP8266_Simple::setTimeout(int seconds)
{
  this->generalCommandTimeoutMicroseconds = seconds * 1000 * 1000;
  
  return ESP8266_OK;
}

long ESP8266_Simple::connectToWifi(const char *SSID, const char *Password)
{
  byte returnValue;
  unsigned long previousTimeout = this->generalCommandTimeoutMicroseconds;
  
  // First set to client mode
  returnValue = this->setWifiMode(ESP8266_STATION);
  if(returnValue != ESP8266_OK) return returnValue;
  
  // Not sure if a QAP is a good idea here?
  /*
    returnValue = this->disconnectFromWifi();
    if(returnValue != ESP8266_OK) return returnValue;
  */
  
  
  char atCmd[11];
  strcpy_P(atCmd, PSTR("AT+CWJAP=\"")); // PSTR monkeying to save a few bytes
  const char *cmdParts[] = {
    atCmd,
    SSID,
    "\",\"",
    Password,
    "\""
  };
  
  // Connecting to Wifi takes a while
  this->generalCommandTimeoutMicroseconds = max((long)5*1000*1000, this->generalCommandTimeoutMicroseconds);

  returnValue = this->sendCommand((const char **)cmdParts,(byte)5, (char *)NULL,(int)0, (byte)1);
  this->generalCommandTimeoutMicroseconds = previousTimeout;
  return returnValue;
}

byte ESP8266_Simple::disconnectFromWifi()
{
  return this->sendCommand(F("AT+CWQAP"));  
}

byte ESP8266_Simple::startHttpServer(unsigned port, int (* requestHandler)(char *buffer, int requestLength, int bufferLength))
{
  
  return ESP8266_ERROR;
}

byte ESP8266_Simple::stopHttpServer()
{
  return ESP8266_ERROR;
}

int ESP8266_Simple::serveHttpRequest()
{
  return ESP8266_ERROR;
}



// serverIpAddress = ip address to connect to
// port = port to connect to (80)
// requestPathAndResponseBuffer = the path to GET (eg "/blah"), this buffer will also receive the null-terminated response
// bufferLength = the length of the buffer
// httpHost, if provided an HTTP/1.0 request will be sent and include the Host: header, if not provided then NO HTTP version 
//    will be appended to the GET.  
//    
//    If httpHost is NULL, then you won't be able to contact "virtual hosts" on servers that
//    share an IP with multiple sites, only the default site on the IP will respond.  Additionally, at least for Apache
//    servers, you will not be sent any headers at all, body only.  
//    
// bodyResponseOnlyFromLine = If httpHost is not NULL, then we are sent headers in the response, by default these will be stripped 
//    because bodyResponseOnlyFromLine = 1, if you set bodyResponseOnlyFromLine = 0 you will get all headers (pending buffer space)
//    if you set bodyResponseOnlyFromLine = -1, you will get headers from the line matching the absolute value
//
// httpResponseCode = if not null, then if we are sent headers, try and find the response code and set this variable to it

byte ESP8266_Simple::sendHttpRequest( unsigned long serverIpAddress, int port,  char *requestPathAndResponseBuffer, int bufferLength, char *httpHost, int bodyResponseOnlyFromLine, int *httpResponseCode )
{  
  byte responseCode;
  char cmdBuffer[64];    
  memset(cmdBuffer,0,sizeof(cmdBuffer));
  
  // Build up the command string
  //  AT+CIPSTART="TCP","[IP]",[PORT]
  strncpy_P(cmdBuffer, PSTR("AT+CIPSTART=\"TCP\",\""), sizeof(cmdBuffer));        // Command itself with opening quote for IP
  this->ipConvertDatatypeFromTo(serverIpAddress, cmdBuffer+strlen(cmdBuffer));  // the IP address
  strcpy(cmdBuffer+strlen(cmdBuffer),"\",");                                    // closing quote for IP
  itoa(port,cmdBuffer+strlen(cmdBuffer), 10);                                   // port  
  
  responseCode = this->sendCommand(cmdBuffer);
  if(responseCode != ESP8266_OK) return responseCode;
  
  // Create the data command
  memset(cmdBuffer,0,sizeof(cmdBuffer));
  strcpy_P(cmdBuffer, PSTR("AT+CIPSEND="));
  //                          "GET "                                       "\r\n"
  int httpRequestDataLength =   4  + strlen(requestPathAndResponseBuffer) + 2;
  if(httpHost)
  {
    //                        " HTTP/1.1\r\nHost: "                        "\r\n"
    httpRequestDataLength +=          17            + strlen(httpHost)     + 2;
  }
  
  itoa(httpRequestDataLength, cmdBuffer+strlen(cmdBuffer), 10); // Add 2 bytes for the trailing CRLF sequence that sendCommand will append
  
  responseCode = this->sendCommand(cmdBuffer);
  if(responseCode != ESP8266_OK)
  {
    // Close response
    this->sendCommand(F("AT+CIPCLOSE"));
    return responseCode;
  }
  
  memset(cmdBuffer,0,sizeof(cmdBuffer));  
  if(httpHost)
  {
    // NOTE!  If you specify HTTP/1.1, then Apache+PHP insist on sending chunked transfers
    //        this means that you'll get chunk lengths in your stream
    //        chunk lengths are hexadecimal integers specifying the chunk length given 
    //        before that chunk, eg your stream looks like
    //            [http response headers which include Transfer-Encoding: chunked]
    //            [blank line]
    //            [chunk length]
    //            [blank line]
    //            [data bytes (chunk length of)]
    //            etc...
    //        which would be a pain to parse, so we will just use HTTP/1.0, which works 
    //        well enough for our purposes and does not include any stupid chunking, yay.
    strcpy_P(cmdBuffer, PSTR(" HTTP/1.0\r\nHost: "));   
  }
  
  const char *builtUpCommand[] = {
    "GET ",
    requestPathAndResponseBuffer,
    cmdBuffer,
    httpHost,
    "\r\n" // The sendCommand will include another \r\n for us indicating end of headers
  };
  
  responseCode = this->sendCommand((const char **)builtUpCommand, httpHost ? 5 : 2, NULL, 0, 1); 
  if(responseCode != ESP8266_OK)
  {
    this->sendCommand(F("AT+CIPCLOSE"));
    return responseCode;
  }
  
  if(httpHost)
  {
    int httpResponseCodeBuffer;
    this->readIPD(requestPathAndResponseBuffer,bufferLength,bodyResponseOnlyFromLine, &httpResponseCodeBuffer);
    
    if(httpResponseCode)
    {
      *httpResponseCode = httpResponseCodeBuffer;
    }
  }
  else
  {
    this->readIPD(requestPathAndResponseBuffer,bufferLength,bodyResponseOnlyFromLine);
  }
  
  if(this->unlinkConnection() != ESP8266_OK)
  {
    this->reset();
  }
  this->sendCommand(F("AT+CIPSTATUS"));
  return ESP8266_OK;    
}

unsigned int ESP8266_Simple::readIPD(char *responseBuffer, int responseBufferLength, int bodyResponseOnlyFromLine, int *parseHttpResponse)
{  
  if(!this->espSerial->waitUntilAvailable()) return 0;
  
 // Serial.print("BRFL: ");
 // Serial.println(bodyResponseOnlyFromLine);
  
  char cmdBuffer[128];
  
  memset(cmdBuffer,0,sizeof(cmdBuffer));
  memset(responseBuffer,0,responseBufferLength);
  
  int cmdBufferIndex          = 0;
  int responseBufferIndex     = 0;    
  int packetLength            = -1;
  int bytesRead               = 0;
  int lineNumber              = -1;
  
  // For HTTP parsing only
  byte headerEnd              = 0;
 
  do
  {
    this->espSerial->waitUntilAvailable();
    
    if(packetLength == -1)
    { // Look for the +IPD response from the ESP8266 to get the number of bytes to read
      // note that we might also pick up an "OK" and other sillyness from the trailing of 
      // a previous +IPD segment (each +IPD is a packet's contents, maximum MTU of your network 
      // limits it (less a bit for TCP/IP overheads)).
      if((bytesRead = this->espSerial->readBytesUntilAndIncluding(':', cmdBuffer, sizeof(cmdBuffer), 1)) > 4)
      {
        // This is probably a bit brittle, but we don't want to waste time 
        // while the serial buffer is filling.  Ideally we would check that
        // there is a series of digits before the : and the characters "+IPD," before those        
        if(cmdBuffer[bytesRead-1] == ':')
        {
          // There might be whitespace at the start of this line, we are looking at 
          //  +IPD,1234
          //  where 1234 is the number of bytes to follow, 
          //  so we will walk backwards to find the , and then from that point forward
          //  convert to an integer
          for(cmdBufferIndex = bytesRead-1; cmdBufferIndex >= 0; cmdBufferIndex--)
          {
            if(cmdBuffer[cmdBufferIndex-1] == ',') break;
          }
          packetLength = atoi(cmdBuffer+cmdBufferIndex);   
        
          ESP82336_DEBUG("Packet Length: ");
          ESP82336_DEBUGLN(packetLength);
        }
        else if(cmdBuffer[0] == 'O') // "OK" - signals end of packet data
        {
          
        }
        else if(cmdBuffer[0] == 'U' && cmdBuffer[5] == 'k') // "Unlink" - signals end of stream
        {
          break;
        }
      }
      
      memset(cmdBuffer,0,sizeof(cmdBuffer));
      cmdBufferIndex = 0;
      continue;      
    }
    else
    {      
      // If packet is empty, or responseBuffer is full, just 
      // finish up now and discard everything
      if(min(packetLength,responseBufferLength-responseBufferIndex-1) <= 0)
      {
        break;
      }
      
      if(this->espSerial->overflow())
      {
        ESP82336_DEBUGLN("OVERFLOW");
      }
      
      // Read up to the next newline, or all the remaining response, or as much as we can fit in the buffer, whichever comes first      
      bytesRead = this->espSerial->readBytesUntilAndIncluding('\n', responseBuffer+responseBufferIndex, min(packetLength,responseBufferLength-responseBufferIndex-1));
      
      // If we read 1 byte, the index for the next write goes up one (effectivly responseBufferIndex is always the trailing null position
      //  or if you prefer, the number of bytes stored in the buffer)
      responseBufferIndex    += bytesRead;
      
      // Reduce the remaining response by the # of bytes read
      packetLength -= bytesRead;
      
      
      // The first time round, lineNumber will still be zero
      if(lineNumber == -1)
      {
        // First line of first packet should be status code (hopefully nobody out there has preceeding whitespace)
        if(parseHttpResponse)
        {
          if(strcmp_P(responseBuffer, PSTR("HTTP/")) == 0)
          {
            *parseHttpResponse = atoi(responseBuffer+9); // 9 == strlen("HTTP/1.1 ")
          }
          else
          {
            *parseHttpResponse = 0; // Unknown
          }
        }
        
        // And now we will rename line 0 to be line 1
        lineNumber = 1;
      }
      
      // if we are parsing HttpResponse, we are getting headers, see if 
      // we should skip them
      
      // If bodyResponseOnlyFromLine is 1 or more and we have headers, we need to skip the headers        
      if(parseHttpResponse && bodyResponseOnlyFromLine > 0 && !headerEnd )
      {
        // Because we are discarding lines up to the trailing \r\n, we can
        // just test from the start of the buffer
        if(responseBufferIndex > 1 && responseBuffer[0] == '\n' && (responseBuffer[2] == '\n' || responseBuffer[1] == '\n'))
        {
          // \r\n\r\n is found          
          headerEnd = true;
          memset(responseBuffer,0,responseBufferLength);
          responseBufferIndex = 0;  
          lineNumber = 1;
        }
        else
        {   
          //ESP82336_DEBUGLN(responseBuffer);
          /*
          ESP82336_DEBUG("0=")
          ESP82336_DEBUG((byte)responseBuffer[0]);
          ESP82336_DEBUG("; 1=")
          ESP82336_DEBUG((byte)responseBuffer[1]);
          ESP82336_DEBUG("; 2=")
          ESP82336_DEBUG((byte)responseBuffer[2]);
          ESP82336_DEBUG("; X=")
          ESP82336_DEBUGLN((byte)responseBuffer[responseBufferIndex-1]);
          */
          
          responseBuffer[0] = responseBuffer[responseBufferIndex-1];  
          
          //ESP82336_DEBUG("L=")
          //ESP82336_DEBUGLN((byte)responseBuffer[responseBufferIndex-1]);
          
          memset(responseBuffer+1,0,responseBufferIndex);            
          responseBufferIndex = 1;       
          
          // Increment the line number for the next round if we finished on a newline
          //  otherwise the next round is a continuation of this line
          if(responseBuffer[responseBufferIndex-1] == '\n')
          {
            lineNumber++;
          }
        }
        
      }
      
      // A "negative" bodyResponseOnlyFromLine means skip this many headers and then include the rest 
      // provided that headers are avalable (which will be the case only if httpHost is defined)      
      // if we have not reached the minimum line yet, empty the buffer
      else if( abs(bodyResponseOnlyFromLine) > lineNumber)
      {
        /*
        Serial.print("SKIP ");
        Serial.print(lineNumber);
        Serial.print(" :");
        Serial.println(responseBuffer);
        */
        
         // Increment the line number for the next round if we finished on a newline
        //  otherwise the next round is a continuation of this line
        if(responseBuffer[responseBufferIndex-1] == '\n')
        {
          lineNumber++;
        }
        
        memset(responseBuffer,0,responseBufferLength);
        responseBufferIndex = 0;        
      }
     
      
      // If there is room in the buffer and more to be got, try to get more
      if(packetLength > 0 && responseBufferIndex < (responseBufferLength - 1))
      {   
        this->espSerial->waitUntilAvailable();
        continue;
      }
      else if(packetLength <= 0 && responseBufferIndex < (responseBufferLength - 1))
      {
        // Try for the next packet
        packetLength = -1;
        continue;
      }
      
      break;
    }    
  }
  while(1); //  Timeout to go here
  
  
  return responseBufferIndex;
}

byte ESP8266_Simple::unlinkConnection()
{
  char cmdBuffer[8];
  memset(cmdBuffer, 0, sizeof(cmdBuffer));
  ESP82336_DEBUGLN();
  ESP82336_DEBUGLN("UNLINKING");
  // Blindly send a CIPCLOSE to try and kill the connection now
  // NOTE: Nope, this tends to cause the ESP to crash out
  // this->espSerial->println(F("AT+CIPCLOSE"));

  // Dump everything else until we see "Unlink" or nothing else seems to be available
  do
  {    
    if(this->espSerial->waitUntilAvailable() == 0 || !this->espSerial->readBytesUntilAndIncluding('\n', cmdBuffer, sizeof(cmdBuffer)-1))
    {   
      return ESP8266_TIMEOUT;      // Caller might want to do a reset()
    }
    ESP82336_DEBUG(cmdBuffer);
  } while( // We really need to keep up and not have any overflow, so not using string comparison here
    cmdBuffer[0] != 'U' || 
    cmdBuffer[1] != 'n' || 
    cmdBuffer[2] != 'l' || 
    cmdBuffer[3] != 'i' || 
    cmdBuffer[4] != 'n' || 
    cmdBuffer[5] != 'k'
  );
  
  if(strncmp_P(cmdBuffer, PSTR("Unlink"), 6) == 0)
  {    
    return ESP8266_OK;
  }
  else
  {
    return ESP8266_ERROR;
  }
}

void ESP8266_Simple::ipConvertDatatypeFromTo(const char *ipAddressString, unsigned long &ipAddressLong)
{  
  byte i;
  
  ipAddressLong = (unsigned long) atoi(ipAddressString);
  
  for(i = 0; i < 15; i++)
  {
    if(ipAddressString[i] == '.')
    {
      
      ipAddressLong = (ipAddressLong << 8) | (unsigned long) atoi(ipAddressString+i+1);
      break;
    }
  }
  
  for(i++; i < 15; i++)
  {
    if(ipAddressString[i] == '.')
    {      
      ipAddressLong = (ipAddressLong << 8) | (unsigned long) atoi(ipAddressString+i+1);
      break;
    }
  }
  
  for(i++; i < 15; i++)
  {
    if(ipAddressString[i] == '.')
    {
      ipAddressLong = (ipAddressLong << 8) | (unsigned long) atoi(ipAddressString+i+1);
      break;
    }
  }    
}

void ESP8266_Simple::ipConvertDatatypeFromTo(unsigned long ipAddressLong, char *ipAddressStringBuffer)
{
  char quadBuff[4];
  
  memset(ipAddressStringBuffer, 0, 16); // Clear buffer
  
  // First byte
  ultoa(((ipAddressLong >> 24) & 0xFF), quadBuff, 10);
  strncpy(ipAddressStringBuffer,quadBuff, 3);
  ipAddressStringBuffer[strlen(ipAddressStringBuffer)] = '.';
  
  // Second byte
  ultoa(((ipAddressLong >> 16) & 0xFF), quadBuff, 10);
  strncpy(ipAddressStringBuffer+strlen(ipAddressStringBuffer),quadBuff, 3);
  ipAddressStringBuffer[strlen(ipAddressStringBuffer)] = '.';
  
  // Third byte
  ultoa(((ipAddressLong >> 8) & 0xFF), quadBuff, 10);
  strncpy(ipAddressStringBuffer+strlen(ipAddressStringBuffer),quadBuff, 3);
  ipAddressStringBuffer[strlen(ipAddressStringBuffer)] = '.';
  
  // Final byte
  ultoa(((ipAddressLong >> 0) & 0xFF), quadBuff, 10);
  strncpy(ipAddressStringBuffer+strlen(ipAddressStringBuffer),quadBuff, 3);
}

byte ESP8266_Simple::sendCommand(const char *cmd, char *responseBuffer, int responseBufferLength, byte getResponseFromLine)
{
  return this->sendCommand(&cmd, 1, responseBuffer, responseBufferLength, getResponseFromLine);
}

byte ESP8266_Simple::sendCommand(const __FlashStringHelper *cmd, char *responseBuffer, int responseBufferLength, byte getResponseFromLine)
{
  char cmdBuffer[strlen_P((const char *)cmd)+1];
  strcpy_P(cmdBuffer, (const char *) cmd); 
  
  // WHY DOES THIS NOT WORK??
  // return this->sendCommand((const char **)&cmdBuffer, 1, responseBuffer, responseBufferLength, getResponseFromLine);
  
  return this->sendCommand((const char *)cmdBuffer, responseBuffer, responseBufferLength, getResponseFromLine);
}

// Send command and get response into a buffer
byte ESP8266_Simple::sendCommand(const char **cmdPartsToConcatenate, byte numParts, char *responseBuffer, int responseBufferLength, byte getResponseFromLine)
{
  char statusBuffer[64];
  unsigned long waitingMicroSeconds = 0;
  int  responseBufferIndex = 0;
  byte statusBufferIndex   = 0;
  byte responseLineNum     = 0;
  int bytesRead            = 0;
  
  // Clear response buffer
  if(responseBufferLength)
  {
    memset(responseBuffer,0,responseBufferLength);
  }
  
  // Clear status buffer
  memset(statusBuffer,0,sizeof(statusBuffer));
    
  this->clearSerialBuffer();

  // this->espSerial->print("AT+");
  // this->espSerial->println(cmd);
  ESP82336_DEBUGLN()
  ESP82336_DEBUG("SEND {{{");
  for(bytesRead = 0; bytesRead < numParts; bytesRead++)
  {
    ESP82336_DEBUG(cmdPartsToConcatenate[bytesRead]);
    this->espSerial->print((const char *)cmdPartsToConcatenate[bytesRead]);
    this->clearSerialBuffer(); // The character echo will fill up the buffer and overflow, we just clear them out.
                               // the last println() we keave so that we don't accidentally clear any response.
  }  
  this->espSerial->println();
  ESP82336_DEBUGLN("}}}");
  bytesRead = 0;
    
  do
  {
    #if ESP8266_SERIALMODE == ESP8266_SOFTWARESERIAL
      // There is no overflow() in the HardwareSerial code, guess we'll just
      // hope it never happens
      if(this->espSerial->overflow())
      {
        Serial.println("Overflow in HERE");
        this->clearSerialBuffer();
        return ESP8266_OVERFLOW;   
      }
    #endif
    
    if(this->espSerial->available())
    {
      memset(statusBuffer,0,sizeof(statusBuffer));
      if(!responseLineNum)
      {
        // read bytes up until the first \n and discard them, this is just the echo
        // incidentally, the echo contains the string sent, including the CR but not the LF, 
        // followed by a second CRLF pair, eg if you send
        // AT+RST[CR][LF] the first response back is
        // AT+RST[CR][CR][LF]
        // the [CR] you send actually seems to form part of the command (if no CR is sent, 
        // the command is not successful, if no LF is sent, the command is not even attempted      
        if((bytesRead = this->espSerial->readBytesUntil('\n',statusBuffer,sizeof(statusBuffer)-1)))
        {        
          ESP82336_DEBUG(statusBuffer);
          
          // readBytesUntil does NOT include the terminator, conveniently as we are getting
          // CRLF termination, we can check for the CR instead.
          if(statusBuffer[bytesRead-1] == '\r')
          {       
            ESP82336_DEBUG('\n');
            responseLineNum = 1;                    
          }
        }      
      }
      else
      {      
        // Fill status buffer
        if((bytesRead = this->espSerial->readBytesUntil('\n',statusBuffer,sizeof(statusBuffer)-1)))
        {
          ESP82336_DEBUG(statusBuffer);
          if(statusBuffer[bytesRead-1] == '\r')
          {
            ESP82336_DEBUG('\n');
          }
          
          if(strncmp_P(statusBuffer, PSTR("SEND OK"),  7) == 0) return ESP8266_OK;
          if(strncmp_P(statusBuffer, PSTR("OK"),       2) == 0) return ESP8266_OK;
          if(strncmp_P(statusBuffer, PSTR(">"),        1) == 0) return ESP8266_OK;
          if(strncmp_P(statusBuffer, PSTR("ERROR"),    5) == 0) return ESP8266_ERROR;                                
          if(strncmp_P(statusBuffer, PSTR("nochange"), 8) == 0) return ESP8266_OK;         
          if(strncmp_P(statusBuffer, PSTR("no change"),9) == 0) return ESP8266_OK;         
          if(strncmp_P(statusBuffer, PSTR("ready"),    5) == 0) return ESP8266_READY;            
          if(strncmp_P(statusBuffer, PSTR("busy"),     4) == 0) return ESP8266_BUSY;    
          if(strncmp_P(statusBuffer, PSTR("Unlink"),   6) == 0) return ESP8266_OK;
          
          // If we are using a response buffer, and we have reached the start line
          // requested (defaults to line 1)        
          if(responseBufferLength && ( getResponseFromLine <= responseLineNum))
          {      
            // If there is room in the response buffer less one byte, copy the statusbuffer there
            if(responseBufferIndex < responseBufferLength-1)
            {
              memcpy(responseBuffer+responseBufferIndex, statusBuffer, min(bytesRead, responseBufferLength-1-responseBufferIndex));
              responseBufferIndex += min(bytesRead, responseBufferLength-1-responseBufferIndex);
              
              if(statusBuffer[bytesRead-1] == '\r')
              {
                if(responseBufferIndex<responseBufferLength-1)
                {
                  responseBuffer[responseBufferIndex++]='\n';
                }
              }
            }         
          }               

          // readBytesUntil does NOT include the terminator, conveniently as we are getting
          // CRLF termination, we can check for the CR instead.
          if(statusBuffer[bytesRead-1] == '\r')
          {
            responseLineNum++;               
          }    
        }
      }
    }
    // At 9600bps, I think we should keep this below 100uS to be able to keep up with certainty
    delayMicroseconds(100);    
    waitingMicroSeconds += 100;    
  }
  while(waitingMicroSeconds < this->generalCommandTimeoutMicroseconds);    
  
  ESP82336_DEBUGLN("TIMED OUT");
  
  // If we got here, we never say an OK or ERROR in the time allowed
  return ESP8266_TIMEOUT;
}

// Blindly send command
byte ESP8266_Simple::sendCommand(const char *cmd)
{
  // Pass off to the normal send command just without a response buffer
  return this->sendCommand(cmd, NULL, 0);
}

byte ESP8266_Simple::sendCommand(const __FlashStringHelper *cmd)
{
  char cmdBuffer[strlen_P((const char *)cmd)+1];
  strcpy_P(cmdBuffer, (const char *) cmd); 
  
  return this->sendCommand(cmdBuffer, NULL, 0);  
}

void ESP8266_Simple::clearSerialBuffer()
{
  while(this->espSerial->available()) this->espSerial->read();  
  this->espSerial->overflow();
}

void ESP8266_Simple::getErrorMessage(byte responseCode, char *bufferWithMinLength50Char)
{
  memset(bufferWithMinLength50Char, 0, 50);
  switch(responseCode)
  {
    case ESP8266_ERROR:    strncpy_P(bufferWithMinLength50Char, PSTR("General Error"), 49); break;
    case ESP8266_TIMEOUT:  strncpy_P(bufferWithMinLength50Char, PSTR("Timeout Waiting For Response"), 49); break;
    case ESP8266_OVERFLOW: strncpy_P(bufferWithMinLength50Char, PSTR("Overflow In Serial Buffer"), 49); break;
    case ESP8266_BUSY:     strncpy_P(bufferWithMinLength50Char, PSTR("Device Is Busy"), 49); break;
    case ESP8266_READY:    strncpy_P(bufferWithMinLength50Char, PSTR("Device issued \"ready\" unexpectedly (rebooted)"), 49); break;
  }
}


void ESP8266_Simple::debugPrintError(byte responseCode, Print *debugPrinter)
{  
  if(!debugPrinter) return;
  
  char buffer[50];
  this->getErrorMessage(responseCode, buffer);
  debugPrinter->println(buffer);  
}
