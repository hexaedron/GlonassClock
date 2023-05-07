#include <Arduino.h>
#include "settings.h"

#ifdef USE_SOFT_SERIAL
    #include <SoftwareSerial.h>
    extern SoftwareSerial GPS_SoftSerial;
#else
    #include <NeoSWSerial.h>
    extern NeoSWSerial GPS_SoftSerial;
#endif

// $PCAS00*01\r\n в конце строки вызывает запись в EEPROM чипа. Поскольку мы делаем инит при каждом запуске, это не имеет
// смысла и только изнашивает EEPROM.
void initATGM332D(bool force = false)
{
    if(force)
        GPS_SoftSerial.print(F("$PCAS03,1,0,1,0,1,0,1,0,0,0,0,,0,0*32\r\n$PCAS04,7*1E\r\n$PCAS00*01\r\n"));
    else
        GPS_SoftSerial.print(F("$PCAS03,1,0,1,0,1,0,1,0,0,0,0,,0,0*32\r\n$PCAS04,7*1E\r\n"));
}