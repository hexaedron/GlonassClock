#pragma once
#include <Arduino.h>
#include "lib7segmentfont.h"

#define COMMON_ANODE true
#define COMMON_CATHODE false

#if !(defined(NUMBERS_ONLY) || defined(ALPHANUMERIC_ONLY))
    #define _CHAR_SEARCH_STRING_LENGTH_ 49
    const char PROGMEM _CHAR_SEARCH_STRING_[] = " -.0123456789ABCDEFGHIJLNOPRSTUY?????????????????";
#elif (!defined NUMBERS_ONLY) && (defined ALPHANUMERIC_ONLY)
    #define _CHAR_SEARCH_STRING_LENGTH_ 32
    const char PROGMEM _CHAR_SEARCH_STRING_[] = " -.0123456789ABCDEFGHIJLNOPRSTUY";
#elif (defined NUMBERS_ONLY) && !(defined ALPHANUMERIC_ONLY)
    #define _CHAR_SEARCH_STRING_LENGTH_ 19
    const char PROGMEM _CHAR_SEARCH_STRING_[] =  " -.0123456789ABCDEF";
#endif


class sevenSegmentScreenShifted
{
private:
    PGM_P charSearchString = _CHAR_SEARCH_STRING_;
    byte charSearchStringLength = _CHAR_SEARCH_STRING_LENGTH_;
    byte latchPin;
    byte dataPin; 
    byte clockPin;
    byte numDigits;
    bool commonPin;
    byte pwmPin = 255;
    byte* displaySegmentBytes;
    byte brightness;
public:
    sevenSegmentScreenShifted(byte latchPin, byte dataPin, byte clockPin, byte numDigits, bool common);
    sevenSegmentScreenShifted(byte latchPin, byte dataPin, byte clockPin, byte pwmPin, byte numDigits, bool common);
    void clear(void);
    void setText(const char* text);
    void setBrightness (byte brightness);
    byte getBrightness(void);
    ~sevenSegmentScreenShifted();
};