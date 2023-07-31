#include <Arduino.h>
#include "settings.h"
#include "EEPROM.h"

byte getDayBrightness(void)
{
    return(EEPROM.read(DEFAULT_BRIGHTNESS_EEPROM_ADDRESS));
}

void setDayBrightness(byte dayBrightness)
{
    EEPROM.put(DEFAULT_BRIGHTNESS_EEPROM_ADDRESS, dayBrightness);
}

byte getNightBrightness(void)
{
    return(EEPROM.read(NIGHT_BRIGHTNESS_EEPROM_ADDRESS));
}

void setNightBrightness(byte nightBrightness)
{
    EEPROM.put(NIGHT_BRIGHTNESS_EEPROM_ADDRESS, nightBrightness);
}

uint32_t getGMTOffset(void)
{
  uint8_t seconds = EEPROM.read(GMT_OFFSET_MINUTES_EEPROM_ADDRESS) * 60;
  if (!(seconds % 15))
  {
    seconds = seconds % 15;
  }
  return(EEPROM.read(GMT_OFFSET_HOURS_EEPROM_ADDRESS) * 60 * 60 + seconds);
}

void setGMTOffset(uint32_t offset)
{
  EEPROM.put(GMT_OFFSET_HOURS_EEPROM_ADDRESS, offset / 60 / 60); 
  EEPROM.put(GMT_OFFSET_MINUTES_EEPROM_ADDRESS, offset / 60 % 60);
}

void EEPROMValuesInit(bool force = false)
{
  if ((EEPROM.read(INIT_ADDR) != INIT_KEY) || force)  // первый запуск или принудительная очистка
  {
    EEPROM.write(INIT_ADDR, INIT_KEY);    // записали ключ
    // записали стандартное значение яркости
    // в данном случае это значение переменной, объявленное выше
    setDayBrightness(DEFAULT_BRIGHTNESS);
    setNightBrightness(NIGHT_BRIGHTNESS);
    setGMTOffset(GMT_SECONDS_OFFSET);
  }
}