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

#define GMT_SECONDS_OFFSET (3 * 60 * 60) // Московское время +3 часа по умолчанию
#define DEFAULT_BRIGHTNESS_EEPROM_ADDRESS 0 
#define NIGHT_BRIGHTNESS_EEPROM_ADDRESS 1
#define GMT_OFFSET_HOURS_EEPROM_ADDRESS 2
#define GMT_OFFSET_MINUTES_EEPROM_ADDRESS 3
#define DEFAULT_BRIGHTNESS 190 
#define NIGHT_BRIGHTNESS 40
#define INIT_ADDR 1023  // номер резервной ячейки
#define INIT_KEY 66     // ключ первого запуска. 0-254, на выбор
#define SOFT_RTC_DELTA_T 1210 // Компенсация отставаний софт часов

// Это на случай, если координаты не подгрузились
#define DEFAULT_SUNRISE_TIME (9 * 60)
#define DEFAULT_SUNSET_TIME (20 * 60)

//#define DEBUG_ENABLE // Для включения отладки раскомментировать
#define FAST_SHIFT_OUT
//#define NEED_GPS_SETUP
#define NUMBERS_ONLY // Для экрана
//#define USE_SOFT_SERIAL
#define SOFT_GPS_BAUD_RATE 9600
#define USE_SOFT_RTC

#define GPS_IS_VALID() (ATGM332D.time.isValid() && ATGM332D_day.isValid() && ATGM332D_month.isValid() && ATGM332D_year.isValid() && ATGM332D.location.isValid())
#define GPS_TIME_IS_VALID() (ATGM332D.time.isValid() && ATGM332D_day.isValid() && ATGM332D_month.isValid() && ATGM332D_year.isValid())

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