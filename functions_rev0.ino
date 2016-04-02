/*
 Crystalfontz theLCD Serial Interfacing Example.
 
 Hardware connection:
 * Connect the serial RX pin on the Arduino to the serial
     TX pin on the theLCD (pin 2 on the H1 connector).
 * Connect the serial TX pin on the Arduino to the serial
     RX pin on the theLCD (pin 1 on the H1 connector).   
 * Connect GND on the Ardunio to GND on the theLCD (pin 15
     on the H1 connector, or any of the other GND points).

 See the Arduino manual for the appropriate RX & TX pins
 for your specific board.
 The example below was tested on an Arduino UNO.
 
 theLCD Setup:
 The theLCD serial port needs to be setup for 9600 baud.
 * Plug the theLCD into your PC's USB port.
 * Run cfTest, and connect to the theLCD
 * Select "Interface Options" in the "Packet Type" list.
 * Enter "\000\047\002" in the Data entry box (no quotes)
     and click the "Send Packet" button. You should see
     a "33 (ack)" "NO DATA" in the Packet Flow list which
     means the theLCD has now set the new serial port
     options.

*/

#include <SoftwareSerial.h>
#include <util/crc16.h>

//arduino serial pins
#define RX_PIN 0
#define TX_PIN 1

//////////////////////////////

typedef struct
{
  uint8_t   command;
  uint8_t   theLength;
  uint8_t   data[24];
  uint16_t  crc;
} CFPacket_t;

class theLCD
{
public:
  //vars
  //functions
  theLCD(SoftwareSerial *serialPort);
  void sendPacket(CFPacket_t *packet);
  void writeText(uint8_t x, uint8_t y, char *text);
  void drawCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t line, uint8_t fill);
  void setBrightness(uint8_t lcdDisplay, uint8_t keypad);
  void clearScreen(void);

private:
  //vars
  SoftwareSerial  *port;
  //functions
  uint16_t CRC(uint8_t *ptr, uint16_t len);
};


//////////////////////////////

theLCD::theLCD(SoftwareSerial *serialPort)
{
  //save the serial port
  port = serialPort;
}

void theLCD::sendPacket(CFPacket_t *packet)
{
  //calculate the crc
  packet->crc = CRC((uint8_t*)packet, packet->theLength+2);
  
  //send the packet in sections
  port->write(packet->command);  
  port->write(packet->theLength);
  port->write(packet->data, packet->theLength);
  port->write((uint8_t*)&packet->crc, 2);
}


void theLCD::writeText(uint8_t x, uint8_t y, char *text)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 31;
  packet.theLength = strlen(text)+2; // length must be 3 to 22
  packet.data[0] = x;
  packet.data[1] = y;
  memcpy(packet.data + 2, text, strlen(text));
  
  //send the packet
  sendPacket(&packet);
}

void theLCD::drawCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t line, uint8_t fill)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 40;    // Display Graphics
  packet.theLength = 6;   // (data length)
  packet.data[0] = 8;     // (Draw a circle)
  packet.data[1] = x;
  packet.data[2] = y;
  packet.data[3] = radius;
  packet.data[4] = line;  // line shade 0-255
  packet.data[5] = fill;  // fill shade 0-255 (0=transparent)
  
  //send the packet
  sendPacket(&packet);
}

void theLCD::setBrightness(uint8_t lcdDisplay, uint8_t keypad)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 14;    // Display Graphics
  packet.theLength = 2;   // (data length)
  packet.data[0] = lcdDisplay;     // 1-100, 0=off
  packet.data[1] = keypad; // 1-100, 0=off

  //send the packet
  sendPacket(&packet);
}

void theLCD::clearScreen(void)
{
  CFPacket_t  packet;
  
  //setup the packet
  packet.command = 6;
  packet.theLength = 0;

  //send the packet
  sendPacket(&packet);
}

uint16_t theLCD::CRC(uint8_t *data, uint16_t theLength)
{
  //calculate the CRC for the packet data
  uint16_t crc = 0xFFFF;
  while(theLength--)
    crc = _crc_ccitt_update(crc, *data++);
  return ~crc;
}

////////////////////////////////////////////

//local variables
SoftwareSerial          *softSerial;
theLCD   *cfPacket;

void setup()
{ 
  //firmware setup
  
  //set LED pin as output
  pinMode(13, OUTPUT);
  
  //setup the serial port pins and SoftwareSerial library
  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
  softSerial = new SoftwareSerial(RX_PIN, TX_PIN);
  softSerial->begin(115200); //115200 baud - theLCD's default
  
  //setup the Crystalfontz serial packet interface
  cfPacket = new theLCD(softSerial);
  
  //init random
  randomSeed(167);

  cfPacket->setBrightness(25,25);
  cfPacket->clearScreen();
}

void loop()
{
  //the main loop
  
  //clear the LCD display
  //cfPacket->clearScreen();
  //write text to a random location on the LCD display
//  cfPacket->writeText(random(20-6), random(4), "Press Brake");
cfPacket->drawCircle(random(20,200),random(10,50),random(2,10),255,80);
  //blink the LED
  digitalWrite(13, HIGH);  
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
}
