#include <Arduino.h>

void makeDateTimeScreen(char* datetime, uint8_t hr, uint8_t min, bool dot = true)
{
  char varchr[3];

  byte dotOffset = 0;
  if(dot)
    dotOffset = 1;

  datetime[2] = '.';

  if(hr == 0)
    {
      datetime[0] = ' ';
      datetime[1] = '0';
    }
    else if ((hr > 0) && (hr <= 9))
    {
      datetime[0] = ' ';
      itoa(hr, varchr, 10);
      datetime[1] = varchr[0];
    }
    else
    {
      itoa(hr, varchr, 10);
      datetime[0] = varchr[0];
      datetime[1] = varchr[1];
    }

    if( min == 0 )
    {
      datetime[2 + dotOffset] = '0';
      datetime[3 + dotOffset] = '0';
    }
    else if ((min > 0) && (min <= 9))
    {
      datetime[2 + dotOffset] = '0';
      itoa(min, varchr, 10);
      datetime[3 + dotOffset] = varchr[0];
    }
    else
    {
      itoa(min, varchr, 10);
      datetime[2 + dotOffset] = varchr[0];
      datetime[3 + dotOffset] = varchr[1];
    }
    
    datetime[4 + dotOffset] = '\0';
}

