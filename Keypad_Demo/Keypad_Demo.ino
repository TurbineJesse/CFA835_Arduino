/*
  KEYPAD DEMO
  =============
  This demonstration shows how to make use of the following keypad funtion:
    - monitorKeypress()
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


// Define variables for the demo
int keyin;            // stores which key was pressed
String text;          // stores the text string to be sent to display
char textOut[12];     // holds the text string convert to char array (required for writeText() function.


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

  // Display ready message
  cfPacket->writeText(0,0,"Press any key...");
}


void loop() {

  keyin = cfPacket->monitorKeypress();  // Check for keypress. Returns true if any key is pressed.
  if(keyin>0){                          // If a key is pressed, check which one.
    switch(keyin){                      // See data sheet for bitmask definition
      case 1: text = "UP"; break;
      case 2: text = "ENTER"; break;
      case 4: text = "CANCEL"; break;
      case 8: text = "LEFT"; break;
      case 16: text = "RIGHT"; break;
      case 32: text = "DOWN"; break;
      default: text = "COMBINATION"; break;   // Multiple buttons pressed at one time
    }
    text.toCharArray(textOut,12);         // Convert string to char array
    cfPacket->clearScreen();              // Clear LCD screen
    cfPacket->writeText(0,0,textOut);     // Send text to display
  }
  
  delay(100);
}
