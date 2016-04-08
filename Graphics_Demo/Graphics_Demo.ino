/*
  GRAPHICS DEMO
  =============
  This demonstration shows how to make use of the following graphics funtions:
    - drawPixel()
    - drawCircle()
    - drawLine()
    - drawRectangle()
    - writeText()
    - clearScreen()
  ==============
  
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

#include <CFA835_Arduino.h>

// Arduino serial pins
#define RX_PIN 11         // Define Arduino serial receive pin
#define TX_PIN 10         // Define Arduino serial transmit pin

int i;                    // initialize index variable to use with demos

// Create instances
SoftwareSerial *softSerial;
cfLCD *cfPacket;


void setup() {
  // put your setup code here, to run once:
  // setup the serial port pins and SoftwareSerial library
  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
  softSerial = new SoftwareSerial(RX_PIN, TX_PIN);
  softSerial->begin(9600); // options: 9600, 19200 or 115200(default)
  
  //setup the Crystalfontz serial packet interface
  cfPacket = new cfLCD(softSerial);

  // initialize the pseudo-random number generator
  randomSeed(114);
}


void loop() {

  cfPacket->clearScreen();    // clear the LCD display

  ////////////////////
  //  Draw Pixels  //
  ////////////////////
  for(i=0; i<300; i++){
    cfPacket->drawPixel(random(0,243),random(0,63),random(150,255));
    delay(5);
  }

  cfPacket->clearScreen();    // clear the LCD display

  ////////////////////
  //  Draw Circles  //
  ////////////////////
  for(i=0; i<75; i++){
    cfPacket->drawCircle(random(20,200),random(10,50),random(2,10),random(80,255),random(80,255));
    delay(50);
  }

  cfPacket->clearScreen();    // clear the LCD display

  //////////////////
  //  Draw Lines  //
  //////////////////
  for(i=0; i<75; i++){
    cfPacket->drawLine(random(0,243), random(0,67), random(0,243), random(0,67), random(80,255));
    delay(50);
  }
  
  cfPacket->clearScreen();    // clear the LCD display

  ///////////////////////
  //  Draw Rectangles  //
  ///////////////////////
  for(i=0; i<75; i++){
    cfPacket->drawRectangle(random(20,220), random(10,57), random(2,16), random(2,16), random(50,255), random(5,255));
    delay(50);
  }

  //////////////////
  //  Write Text  //
  //////////////////
  for(i=0; i<10; i++){
    cfPacket->clearScreen();
    cfPacket->writeText(random(20-9), random(0,3), "Demo Text");
    delay(750);
  }

  cfPacket->clearScreen();    // clear the LCD display

  /////////////////////
  //  Sin Wave Demo  //
  /////////////////////
  int x=0,y=32;
  float rad;
  cfPacket->writeText(6, 3, "Sine Wave");       // Display text
  cfPacket->drawLine(0,32,243,32,125);          // Draw horizontal line
  for(i=0; i<250; i++){
    cfPacket->drawRectangle(x,y,1,1,255,255);   // Plot "point" using rectangle
    rad = (8*x) * 3.14159 / 180.;
    y=-sin(rad)*12 + 32;
    delay(15);
    x=x+1;
  }
  
  delay(50);
}
