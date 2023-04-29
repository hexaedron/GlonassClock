#include <Arduino.h>
#include "lib7SegmentScreenShifted.h"

// Simple constructor
sevenSegmentScreenShifted::sevenSegmentScreenShifted(byte latchPin, byte dataPin, byte clockPin, byte numDigits, bool common = COMMON_ANODE)
{
    this->latchPin  =  latchPin;
    this->dataPin   =   dataPin;
    this->clockPin  =  clockPin;
    this->numDigits = numDigits;
    this->commonPin =    common;

    this->displaySegmentBytes = new byte[this->numDigits];
}

// Constructor with PWM mode
sevenSegmentScreenShifted::sevenSegmentScreenShifted(byte latchPin, byte dataPin, byte clockPin, byte pwmPin, byte numDigits, bool common)
{
    this->latchPin  =  latchPin;
    this->dataPin   =   dataPin;
    this->clockPin  =  clockPin;
    this->numDigits = numDigits;
    this->pwmPin    =    pwmPin;
    this->commonPin =    common;
    
    this->displaySegmentBytes = new byte[this->numDigits];
}

sevenSegmentScreenShifted::~sevenSegmentScreenShifted()
{
    delete[] this->displaySegmentBytes;
}

// We just fill all the digits with charSearchString[0] (i.e. blank space " ")
// just making all bits inverted if we use a common cathode display
void sevenSegmentScreenShifted::clear(void)
{
    digitalWrite(this->latchPin, LOW);
    
    if (this->commonPin == COMMON_ANODE)
        for (byte i = 0; i < this->numDigits; i++)
            shiftOut(this->dataPin, this->clockPin, LSBFIRST, pgm_read_byte(&numberSegmentsFont[0]));
    else    
        for (byte i = 0; i < this->numDigits; i++)
            shiftOut(this->dataPin, this->clockPin, LSBFIRST, ~pgm_read_byte(&numberSegmentsFont[0]));

    digitalWrite(this->latchPin, HIGH);
}

// Here we parse the input text and set it to the shift register
// just making all bits inverted if we use a common cathode display
void sevenSegmentScreenShifted::setText(const char* text)
{
    byte i = 0;
    byte digitNum = 0;

    // text should not be empty or start with ".". If it does, that's definetly an user's mistake,
    // so we clear the screen and exit.
    if((text[0] == '.') || (text[0] == '\0'))
    {
        this->clear();
        return;
    }

    // Make this->displaySegmentBytes[] contain all bits to manipulate individual segments
    do
    { 
        this->displaySegmentBytes[digitNum] = pgm_read_byte(&numberSegmentsFont[0]); // Set digit to blank space in case there is an unprintable char
        for (byte j = 0; j < this->charSearchStringLength; j++)
        {            
            if (text[i] == '.')
            {
                bitClear(this->displaySegmentBytes[digitNum - 1], 0); // That's only a dot. So we apply it to previous character. 
                
                i++; // Since it'a dot, we'll have to iterate tis symbol one more time
                continue; 
            }

            byte segByte = pgm_read_byte(&this->charSearchString[j]); 
            
            if (segByte == (byte)text[i])
            { 
                this->displaySegmentBytes[digitNum] = pgm_read_byte(&numberSegmentsFont[j]);           
            }
        }

        digitNum++;
        i++;
    } while ((text[i] != '\0') || (digitNum < this->numDigits));

    // Shift out this->displaySegmentBytes[] to 74HC595 registers to make our screen glow!
    // Again, we are making all bits inverted if we use a common cathode display
    for (byte j = this->numDigits; j > 0; j--)
    {
        digitalWrite(this->latchPin, LOW);
            
        if (this->commonPin == COMMON_ANODE)
            shiftOut(this->dataPin, this->clockPin, LSBFIRST, this->displaySegmentBytes[j - 1]);
        else    
            shiftOut(this->dataPin, this->clockPin, LSBFIRST, ~this->displaySegmentBytes[j - 1]);

        digitalWrite(this->latchPin, HIGH);
    }
}

// We set brightness using one more pin for PWM. Just connect it to
// each 74HC595's OE pin. In case of a common cathode we invert the brightness
void sevenSegmentScreenShifted::setBrightness (byte newBrightness)
{
    this->brightness = newBrightness;
    if(this->pwmPin != 255)
    {
        if(this->commonPin == COMMON_ANODE)
            analogWrite(this->pwmPin, 255 - newBrightness);
        else
            analogWrite(this->pwmPin, newBrightness);
    }
}

byte sevenSegmentScreenShifted::getBrightness(void)
{
    return(this->brightness);
}