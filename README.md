# Arduino Library For Crystalfontz's CFA835 LCD Display

This Arduino library is built upon a set of 'core' functions originally written by Crystalfontz. I've greatly extended the number of available functions and compiled it all into an easy-to-use library, including a couple example scripts.

The following are the functions available in the library. The (*) symbol designates the functions written by Crystalfontz. The rest I have added.

CFA835_Arduino Library Functions:

    sendPacket() *
    writeText(x, y, text) *
    drawPixel(x, y, shade)
    drawCircle(x, y,  radius, line, fill);
    drawLine(x1, y1, x2, y2, shade);
    drawRectangle(x, y, width, height, line, fill);
    readBacklights();
    setBacklights(lcdDisplay, keypad);
    setLED( LED, state);
    getKeypress();
    monitorKeypress ();
    clearBuffer();
    clearScreen();
    restart();
    validateCRC(data);
    readContrast();
    setContrast(contrast);
    setCursorPosition(column, row);
    setCursorStyle(style);


Webpage: http://nuclearprojects.com/blog/arduino-with-crystalfontz-cfa835-display/
