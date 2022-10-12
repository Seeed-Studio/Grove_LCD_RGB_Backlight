/*
    rgb_lcd.cpp
    2013 Copyright (c) Seeed Technology Inc.  All right reserved.

    Author:Loovee
    2013-9-18

    add rgb backlight fucnction @ 2013-10-15

    The MIT License (MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.1  USA
*/

#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <Wire.h>

#include "rgb_lcd.h"

void rgb_lcd::i2c_send_byte(unsigned char dta) {
    _wire->beginTransmission(LCD_ADDRESS);        // transmit to device #4
    _wire->write(dta);                            // sends five bytes
    _wire->endTransmission();                     // stop transmitting
}

void rgb_lcd::i2c_send_byteS(unsigned char* dta, unsigned char len) {
    _wire->beginTransmission(LCD_ADDRESS);        // transmit to device #4
    for (int i = 0; i < len; i++) {
        _wire->write(dta[i]);
    }
    _wire->endTransmission();                     // stop transmitting
}

rgb_lcd::rgb_lcd()
    : _displayfunction(0),
      _displaycontrol(0),
      _displaymode(0),
      _initialized(0),
      _numlines(0),
      _currline(0)
{
}

void rgb_lcd::begin(uint8_t cols, uint8_t lines, uint8_t dotsize, TwoWire &wire) {

    _wire = &wire;
    _wire->begin();

    if (lines > 1) {
        _displayfunction |= LCD_2LINE;
    }
    _numlines = lines;
    _currline = 0;

    // for some 1 line displays you can select a 10 pixel high font
    if ((dotsize != 0) && (lines == 1)) {
        _displayfunction |= LCD_5x10DOTS;
    }

    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
    delayMicroseconds(50000);


    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    command(LCD_FUNCTIONSET | _displayfunction);
    delayMicroseconds(4500);  // wait more than 4.1ms

    // second try
    command(LCD_FUNCTIONSET | _displayfunction);
    delayMicroseconds(150);

    // third go
    command(LCD_FUNCTIONSET | _displayfunction);


    // finally, set # lines, font size, etc.
    command(LCD_FUNCTIONSET | _displayfunction);

    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();

    // clear it off
    clear();

    // Initialize to default text direction (for romance languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    command(LCD_ENTRYMODESET | _displaymode);


    // check rgb chip model
    _wire->beginTransmission(RGB_ADDRESS_V5);
    if (_wire->endTransmission () == 0)
    {
        rgb_chip_addr = RGB_ADDRESS_V5;
        setReg(0x00, 0x07); // reset the chip
        delayMicroseconds(200); // wait 200 us to complete
        setReg(0x04, 0x15); // set all led always on
    }
    else
    {
        rgb_chip_addr = RGB_ADDRESS;
        // backlight init
        setReg(REG_MODE1, 0);
        // set LEDs controllable by both PWM and GRPPWM registers
        setReg(REG_OUTPUT, 0xFF);
        // set MODE2 values
        // 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
        setReg(REG_MODE2, 0x20);
    }

    setColorWhite();

}

/********** high level commands, for the user! */
void rgb_lcd::clear() {
    command(LCD_CLEARDISPLAY);        // clear display, set cursor position to zero
    delayMicroseconds(2000);          // this command takes a long time!
}

void rgb_lcd::home() {
    command(LCD_RETURNHOME);        // set cursor position to zero
    delayMicroseconds(2000);        // this command takes a long time!
}

void rgb_lcd::setCursor(uint8_t col, uint8_t row) {

    col = (row == 0 ? col | 0x80 : col | 0xc0);
    unsigned char dta[2] = {0x80, col};

    i2c_send_byteS(dta, 2);

}

// Turn the display on/off (quickly)
void rgb_lcd::noDisplay() {
    _displaycontrol &= ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void rgb_lcd::display() {
    _displaycontrol |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void rgb_lcd::noCursor() {
    _displaycontrol &= ~LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void rgb_lcd::cursor() {
    _displaycontrol |= LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void rgb_lcd::noBlink() {
    _displaycontrol &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void rgb_lcd::blink() {
    _displaycontrol |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void rgb_lcd::scrollDisplayLeft(void) {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void rgb_lcd::scrollDisplayRight(void) {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void rgb_lcd::leftToRight(void) {
    _displaymode |= LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void rgb_lcd::rightToLeft(void) {
    _displaymode &= ~LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void rgb_lcd::autoscroll(void) {
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void rgb_lcd::noAutoscroll(void) {
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void rgb_lcd::createChar(uint8_t location, uint8_t charmap[]) {

    location &= 0x7; // we only have 8 locations 0-7
    command(LCD_SETCGRAMADDR | (location << 3));


    unsigned char dta[9];
    dta[0] = 0x40;
    for (int i = 0; i < 8; i++) {
        dta[i + 1] = charmap[i];
    }
    i2c_send_byteS(dta, 9);
}

// Control the backlight LED blinking
void rgb_lcd::blinkLED(void) {
    if (rgb_chip_addr == RGB_ADDRESS_V5)
    {
        // attach all led to pwm1
        // blink period in seconds = (<reg 1> + 2) *0.128s
        // pwm1 on/off ratio = <reg 2> / 256
        setReg(0x04, 0x2a);  // 0010 1010
        setReg(0x01, 0x06);  // blink every second
        setReg(0x02, 0x7f);  // half on, half off
    }
    else
    {
        // blink period in seconds = (<reg 7> + 1) / 24
        // on/off ratio = <reg 6> / 256
        setReg(0x07, 0x17);  // blink every second
        setReg(0x06, 0x7f);  // half on, half off
    }
    

}

void rgb_lcd::noBlinkLED(void) {
    if (rgb_chip_addr == RGB_ADDRESS_V5)
    {
        setReg(0x04, 0x15);  // 0001 0101
    }
    else
    {
        setReg(0x07, 0x00);
        setReg(0x06, 0xff);
    }
}

/*********** mid level commands, for sending data/cmds */

// send command
inline void rgb_lcd::command(uint8_t value) {
    unsigned char dta[2] = {0x80, value};
    i2c_send_byteS(dta, 2);
}

// send data
inline size_t rgb_lcd::write(uint8_t value) {

    unsigned char dta[2] = {0x40, value};
    i2c_send_byteS(dta, 2);
    return 1; // assume sucess
}

void rgb_lcd::setReg(unsigned char reg, unsigned char dat) {
    _wire->beginTransmission(rgb_chip_addr); // transmit to device #4
    _wire->write(reg);
    _wire->write(dat);
    _wire->endTransmission();    // stop transmitting
}

void rgb_lcd::setRGB(unsigned char r, unsigned char g, unsigned char b) {
    if (rgb_chip_addr == RGB_ADDRESS_V5)
    {
        setReg(0x06, r);
        setReg(0x07, g);
        setReg(0x08, b);
    }
    else
    {
        setReg(0x04, r);
        setReg(0x03, g);
        setReg(0x02, b);
    }
}

void rgb_lcd::setPWM(unsigned char color, unsigned char pwm) {
    switch (color)
    {
        case WHITE:
            setRGB(pwm, pwm, pwm);
            break;
        case RED:
            setRGB(pwm, 0, 0);
            break;
        case GREEN:
            setRGB(0, pwm, 0);
            break;
        case BLUE:
            setRGB(0, 0, pwm);
            break;
        default:
            break;
    }
}

const unsigned char color_define[4][3] = {
    {255, 255, 255},            // white
    {255, 0, 0},                // red
    {0, 255, 0},                // green
    {0, 0, 255},                // blue
};

void rgb_lcd::setColor(unsigned char color) {
    if (color > 3) {
        return ;
    }
    setRGB(color_define[color][0], color_define[color][1], color_define[color][2]);
}
