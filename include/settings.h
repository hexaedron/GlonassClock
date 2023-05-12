#pragma once

#define CLOCK      10   //SH_CP 
#define DATA        8   //DS   
#define LATCH       9   //ST_CP 
#define PWM         6   // 
#define ENCODER_S1  A0  // 
#define ENCODER_S2  A1  //
#define ENCODER_KEY A2  // 
#define TX_PIN       5  // GPS
#define RX_PIN       4  // GPS

#define GMT_SECONDS_OFFSET (3 * 60 * 60) // Московское время +3 часа
#define DEFAULT_BRIGHTNESS_EEPROM_ADDRESS 0 
#define NIGHT_BRIGHTNESS_EEPROM_ADDRESS 1
#define GMT_OFFSET_HOURS_EEPROM_ADDRESS 2
#define GMT_OFFSET_MINUTES_EEPROM_ADDRESS 3
#define DEFAULT_BRIGHTNESS 190 
#define NIGHT_BRIGHTNESS 40
#define INIT_ADDR 1023  // номер резервной ячейки
#define INIT_KEY 66     // ключ первого запуска. 0-254, на выбор

//#define DEBUG_ENABLE // Потом убрать
#define FAST_SHIFT_OUT
//#define NEED_GPS_SETUP
#define NUMBERS_ONLY // Для экрана
//#define USE_SOFT_SERIAL
#define SOFT_GPS_BAUD_RATE 9600

#ifdef DEBUG_ENABLE
  #define DEBUG(x) Serial.println(x)
#else
  #define DEBUG(x)
#endif

struct sunsetSunrise
{
  uint16_t set;
  uint16_t rise;
};