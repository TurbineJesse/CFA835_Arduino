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

#ifndef CFA835_Arduino_h
#define CFA835_Arduino_h

#include "Arduino.h"
#include <SoftwareSerial.h>
#include <util/crc16.h>


//////////////////////////////

typedef struct
{
  uint8_t   command;
  uint8_t   dataLength;
  uint8_t   data[24];
  uint16_t  crc;
} CFPacket_t;

typedef struct
{
  int keypadBacklight;  // stores value for keypad backlight setting
  int displayBacklight; // stores value for display backlight setting
  int contrast;         // stores value for display contrast
  int retries=3;        // Number of times to retry communication upon CRC failure
} cfSettings_t;

///////////////////////////////

class cfLCD
{
public:
  //vars
  //functions
  cfLCD(SoftwareSerial *serialPort);
  void sendPacket(CFPacket_t *packet);
  void writeText(uint8_t x, uint8_t y, char *text);
  void drawPixel(uint8_t x, uint8_t y, uint8_t shade);
  void drawCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t line, uint8_t fill);
  void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t shade);
  void drawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t line, uint8_t fill);
  void setBacklights(uint8_t lcdDisplay, uint8_t keypad);
  void setLED(uint8_t LED, uint8_t state);
  int getKeypress(void);
  int monitorKeypress (void);
  void clearBuffer(void);
  void clearScreen(void);
  int validateCRC(byte *data);
  void restart(void);
  int readBacklights(void);
  int readContrast(void);
  void setContrast(uint8_t contrast);
  void setCursorPosition(uint8_t column, uint8_t row);
  void setCursorStyle(uint8_t style);

private:
  //vars
  SoftwareSerial  *port;
  //functions
  uint16_t CRC(uint8_t *ptr, uint16_t len);
};

#endif