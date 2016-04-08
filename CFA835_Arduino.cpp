/*
 Arduino library for Crystalfontz's CFA835 display by Jesse Shelly.

 Core pieces of this code have been adapted from
 Crystalfontz CFA835 Serial Interfacing Example,
 released to public domain.
 ( Source: https://github.com/crystalfontz/cfa_crc_arduino_example )

 CFA835 Setup:
 The CFA835 serial port can be setup for three baud rates:
 9600, 19200, and 115200 (default). To change from default to 9600:
 * Plug the CFA835 into your PC's USB port.
 * Run cfTest, and connect to the CFA835
 * Select "Interface Options" in the "Packet Type" list.
 * Enter "\000\047\002" in the Data entry box (no quotes)
     and click the "Send Packet" button. You should see
     a "33 (ack)" "NO DATA" in the Packet Flow list which
     means the CFA835 has now set the new serial port
     options.
 * Alternatively, you can change to other baud rates by using:
     - "\000\047\000" for 19200
     - "\000\047\001" for 115200
 * Save the configuration: Select "4 = Store Current State As Boot State".
     then click, "Send Packet".
 
*/

#include <SoftwareSerial.h>
#include <util/crc16.h>
#include "CFA835_Arduino.h"

cfSettings_t cfSettings;

//////////////////////////////

cfLCD::cfLCD(SoftwareSerial *serialPort)
{
  //save the serial port
  port = serialPort;
}

void cfLCD::sendPacket(CFPacket_t *packet)
{
  //calculate the crc
  packet->crc = CRC((uint8_t*)packet, packet->dataLength+2);
  
  //send the packet in sections
  port->write(packet->command);  
  port->write(packet->dataLength);
  port->write(packet->data, packet->dataLength);
  port->write((uint8_t*)&packet->crc, 2);
}


void cfLCD::writeText(uint8_t x, uint8_t y, char *text)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 31;
  packet.dataLength = strlen(text)+2; // length must be 3 to 22
  packet.data[0] = x;
  packet.data[1] = y;
  memcpy(packet.data + 2, text, strlen(text));
  
  //send the packet
  sendPacket(&packet);
}

void cfLCD::drawPixel(uint8_t x, uint8_t y, uint8_t shade)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 40;    // display graphics
  packet.dataLength = 4;  // data length
  packet.data[0] = 5;     // draw a pixel
  packet.data[1] = x;     // 0-243
  packet.data[2] = y;     // 0-67
  packet.data[3] = shade;
  
  //send the packet
  sendPacket(&packet);
}

void cfLCD::drawCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t line, uint8_t fill)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 40;    // Display Graphics
  packet.dataLength = 6;   // (data length)
  packet.data[0] = 8;     // (Draw a circle)
  packet.data[1] = x;
  packet.data[2] = y;
  packet.data[3] = radius;
  packet.data[4] = line;  // line shade 0-255
  packet.data[5] = fill;  // fill shade 0-255 (0=transparent)
  
  //send the packet
  sendPacket(&packet);
}

void cfLCD::drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t shade)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 40;      // display graphics
  packet.dataLength = 6;    // data length
  packet.data[0] = 6;       // draw a line
  packet.data[1] = x1;
  packet.data[2] = y1;
  packet.data[3] = x2;
  packet.data[4] = y2;  // line shade 0-255
  packet.data[5] = shade;  // shade 0-255 (0=transparent)
  
  //send the packet
  sendPacket(&packet);
}

void cfLCD::drawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t line, uint8_t fill)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 40;      // display graphics
  packet.dataLength = 7;    // data length
  packet.data[0] = 7;       // draw a rectangle
  packet.data[1] = x;       // lop-left corner
  packet.data[2] = y;       // top-left corner
  packet.data[3] = width;
  packet.data[4] = height;
  packet.data[5] = line;    // line shade 0-255
  packet.data[6] = fill;    // shade 0-255 (0=transparent)
  
  //send the packet
  sendPacket(&packet);
}

//
// Reads the backlight settings for both the display and keypad.
// Returns 1 upon success. The values are stored in
// cfSettings.keypadBacklight and cfSettings.displayBacklight.
// Numerical values are between 0 and 100. (0=off)
//
int cfLCD::readBacklights(void)
{
  CFPacket_t  packet;
  int retry=0;
  while(retry < cfSettings.retries)
  {
      
    //setup the packet
    packet.command = 14;    // Display Graphics
    packet.dataLength = 0;   // (data length)
  
    //send the packet
    port->flush();                // For until all outgoing data is transmitted (flushed)
    delay(50);                    // Wait for any remaining responses from CFA835
    clearBuffer();                // Clear buffer in preparation for receiving response
    sendPacket(&packet);          // Send the command
  
    byte dataIn[30];              // Data array
    int i=0;
  
    while(!port->available());    // Wait for response
    delay(50);                    // Provide sufficient time to receive all data
    while(port->available()){     // Loop until entire buffer is read
      dataIn[i] = port->read();   // Store all data into array
      i++;
    }
  
    // Calculated CRC value and received CRC value must match for a valid response packet
    if(validateCRC(dataIn)){
      if(dataIn[0]==78 && dataIn[1]==2){   // Verify correct response type and length
          cfSettings.displayBacklight = dataIn[2];
          cfSettings.keypadBacklight = dataIn[3];
          return 1;                 // Success, data has been stored to variables
      }
    }
    retry++;
  }
  return 0;
}

//
// Reads the contrast settings for the LCD display.
// Returns 1 upon success. The values are stored in
// cfSettings.contrast. Numerical values are between 0 and 255.
//
int cfLCD::readContrast(void)
{
  CFPacket_t  packet;
  int retry=0;
  while(retry < cfSettings.retries)
  {    
    //setup the packet
    packet.command = 13;          // Contrast
    packet.dataLength = 0;        // (data length)
  
    //send the packet
    port->flush();                // For until all outgoing data is transmitted (flushed)
    delay(50);                    // Wait for any remaining responses from CFA835
    clearBuffer();                // Clear buffer in preparation for receiving response
    sendPacket(&packet);          // Send the command
  
    byte dataIn[30];              // Data array
    int i=0;
  
    while(!port->available());    // Wait for response
    delay(50);                    // Provide sufficient time to receive all data
    while(port->available()){     // Loop until entire buffer is read
      dataIn[i] = port->read();   // Store all data into array
      i++;
    }
  
    // Calculated CRC value and received CRC value must match for a valid response packet
    if(validateCRC(dataIn)){
      if(dataIn[0]==77 && dataIn[1]==1){   // Verify correct response type and length
          cfSettings.contrast = dataIn[2];
          return 1;                 // Success, data has been stored to variable
      }
    }
    retry++;
  }
  return 0;
}

void cfLCD::setContrast(uint8_t contrast)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 13;            // contrast
  packet.dataLength = 1;          // data length
  packet.data[0] = contrast;      // 0-255
                                  // 0-111 very light, 112 light, 127 about right, 168 dark
  //send the packet
  sendPacket(&packet);
}


void cfLCD::setBacklights(uint8_t lcdDisplay, uint8_t keypad)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 14;            // display graphics
  packet.dataLength = 2;          // data length
  packet.data[0] = lcdDisplay;    // 1-100, 0=off
  packet.data[1] = keypad;        // 1-100, 0=off

  //send the packet
  sendPacket(&packet);
}

void cfLCD::clearScreen(void)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 6;
  packet.dataLength = 0;

  //send the packet
  sendPacket(&packet);
}

//
// Restarts the LCD display. May take up to 3 seconds
// before display will respond to further commands afterwards.
//
void cfLCD::restart(void)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 5;     // See data sheet for numerical values
  packet.dataLength = 3;
  packet.data[0] = 8;
  packet.data[1] = 25;
  packet.data[2] = 48;
  
  //send the packet
  sendPacket(&packet);
}

void cfLCD::setLED(uint8_t LED, uint8_t state)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 34;
  packet.dataLength = 2;
  packet.data[0] = LED;     // 5-12 - See below
   /* 5 = LED 3 - (bottom) green
      6 = LED 3 - (bottom) red
      7 = LED 2 - green
      8 = LED 2 - red
      9 = LED 1 - green
      10 = LED 1 - red
      11 = LED 0 - (top) green
      12 = LED 0 - (top) red */
  packet.data[1] = state; // 0=off, 1-99=duty cycle, 100=high
  
  //send the packet
  sendPacket(&packet);
}

//
// Sets cursor position
//
void cfLCD::setCursorPosition(uint8_t column, uint8_t row)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 11;        // cursor position
  packet.dataLength = 2;
  packet.data[0] = column;    // 0-19 valid
  packet.data[1] = row;       // 0-3 valid
  
  //send the packet
  sendPacket(&packet);
}

//
// Sets cursor style, 5 options to choose from.
//
void cfLCD::setCursorStyle(uint8_t style)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 12;      // cursor style
  packet.dataLength = 1;
  packet.data[0] = style;     // 0-4 - See below
   /* 0 = no cursor
      1 = blinking block cursor
      2 = underscore cursor
      3 = blinking block plus underscore
      4 = inverting, blinking block */
  
  //send the packet
  sendPacket(&packet);
}

void cfLCD::clearBuffer(void) // Clear incoming serial buffer
{
  while(port->available())
    port->read();
}


//
// FUNCTION: getKeypress
// DESCRIPTION: Program execution will wait for user keypress,
// which will then be returned (range: 1-11)
// USE: Great for use within a menu system or other selection UI
//
int cfLCD::getKeypress (void)
{
  int retry=0;
  while(retry < cfSettings.retries){
    byte dataIn[30];              // Data array
    int i=0;
  
    delay(20);                    // Wait for any remaining responses from CFA835
    clearBuffer();                // Clear buffer in preparation for receiving response
    while(!port->available());    // Wait until a key is pressed
    delay(20);                    // Provide sufficient time to receive all data
    while(port->available()){     // Loop until entire buffer is read
      dataIn[i] = port->read();   // Store all data into array
      i++;
    }
    
    // Calculated CRC value and received CRC value must match for a valid response packet
    if(validateCRC(dataIn)){
      if(dataIn[0]==128 && dataIn[1]==1){   // Verify correct response type and length
          return dataIn[2];                 // Return bitmask showing keys currently pressed
      }
    }
    retry++;
  } // End retries
  return 0;
}


//
// FUNCTION: monitorKeypress
// DESCRIPTION: The program will NOT wait for user keypress,
// instead it will query the CFA835 display to verify the current
// state of the buttons. This result will be returned. If no buttons
// are currently pressed, function will return 0.
//
int cfLCD::monitorKeypress (void)
{
  CFPacket_t  packet;
  
  int retry=0;
  while(retry < cfSettings.retries){
    
    byte dataIn[30];              // Data array
    int i=0;
    
    //setup the packet
    packet.command = 24;            // 24 = Read Keypad
    packet.dataLength = 0;
    delay(20);                      // Wait for any remaining responses from CFA835
    clearBuffer();                  // Clear buffer in preparation for receiving response
    sendPacket(&packet);            // Send the packet
    delay(20);                      // Provide time to receive response
    if(port->available())           // Check that a response has been received
    {  
      while(port->available()){     // Loop until entire buffer is read
        dataIn[i] = port->read();   // Store all data into array
        i++;
      }
  
      // Calculated CRC value and received CRC value must match for a valid response packet
      if(validateCRC(dataIn)){
        if(dataIn[0]==88 && dataIn[1]==3){    // Verify correct response type and length
            return dataIn[2];                 // Return bitmask showing keys currently pressed
        }
      }
    }
    retry++;
  } // End retries
  return 0;
}


/**
  * Validates whether a packet response is valid.
  * Compares received CRC value with calculated CRC value.
  * 
  * Output: Returns 1 if valid, 0 otherwise.
  */
int cfLCD::validateCRC(byte *data)
{
  uint16_t dataLength = data[1]+2;  /* Actual data length, plus two CRC bytes */
  uint16_t receivedCRC = data[dataLength+1] << 8 | data[dataLength]; /* CRC value from CFA835 response packet */
  uint16_t calcCRC = CRC(data, dataLength);    /* CRC value calculated from response packet received. */
  if(calcCRC == receivedCRC) return 1;
  else return 0;
}

uint16_t cfLCD::CRC(uint8_t *data, uint16_t dataLength)
{
  //calculate the CRC for the packet data
  uint16_t crc = 0xFFFF;
  while(dataLength--)
    crc = _crc_ccitt_update(crc, *data++);
  return ~crc;
}

////////////////////////////////////////////

