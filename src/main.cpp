#include <Arduino.h>
#include "settings.h"
#include "simpleTimer.h"
#include "lib7SegmentScreenShifted.h"
#include <TinyGPS++.h>
#include <EncButton.h>
#include <RtcDS1307.h>

#define SP_PRECISE
#include "SunPosition.h"

#ifdef USE_SOFT_SERIAL
  #include <SoftwareSerial.h>
#else
  #include <NeoSWSerial.h>
#endif


// Часики нужны глобально
#ifndef USE_SOFT_RTC
  #include <microWire.h>   // или просто Wire
  RtcDS1307<TwoWire> rtc(Wire); 
#else
  #include "swRTC2000.h"
  swRTC2000 rtc;
#endif

#ifdef FAST_SHIFT_OUT
  #include <FastShiftOut.h>
  #define shiftOut(DATA_PIN, CLOCK, ORDER, VALUE) FSO.write(VALUE)
  FastShiftOut FSO(DATA, CLOCK, LSBFIRST);
#endif

// Экран и энкодер нужны глобально
sevenSegmentScreenShifted bigLEDScreen(LATCH, DATA, CLOCK, PWM, 4, COMMON_ANODE);
EncButton<EB_TICK, ENCODER_S1, ENCODER_S2, ENCODER_KEY> encoder;

// Всё, связанное с GPS, тоже нужно глобально
#ifdef USE_SOFT_SERIAL
  SoftwareSerial GPS_SoftSerial(RX_PIN, TX_PIN);
#else 
  NeoSWSerial GPS_SoftSerial(RX_PIN, TX_PIN);
#endif
TinyGPSPlus   ATGM332D;
TinyGPSCustom ATGM332D_year(ATGM332D,  "GNZDA", 4);
TinyGPSCustom ATGM332D_month(ATGM332D, "GNZDA", 3);
TinyGPSCustom ATGM332D_day(ATGM332D,   "GNZDA", 2);

// Правильно заполняет массив с датой/временем
void makeDateTimeScreen(char* datetime, uint8_t hr, uint8_t min, bool dot);

void initPins(void)
{
  // сразу отключим вывод на экран, чтобы не появлялись рандомные символы.
  pinMode(PWM,        OUTPUT); 
  digitalWrite(PWM,     HIGH);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  pinMode(CLOCK,      OUTPUT);
  pinMode(DATA,       OUTPUT);
  pinMode(LATCH,      OUTPUT); 
  pinMode(TX_PIN,     OUTPUT);
  pinMode(RX_PIN,      INPUT);
  pinMode(ENCODER_KEY, INPUT);
  pinMode(ENCODER_S1,  INPUT);
  pinMode(ENCODER_S2,  INPUT);
}

// Берёт с GPS время, плюсует к нему смещение часового пояса и обновляет RTC часики
void adjustTime(uint32_t GMTSecondsOffset);

// Возвращает правильную яркость, беря из EEPROM значения дневной и ночной яркости
byte calculateBrightness(sunsetSunrise* sun);
byte getDayBrightness(void);
byte getNightBrightness(void);
void EEPROMValuesInit(bool force = false);
uint32_t getGMTOffset(void);
void setupModeGMTOffset(void);
void initATGM332D(bool force = false);

int main(int argc, char **argv) 
{
  init();

  initPins();

  #ifdef DEBUG_ENABLE
    Serial.begin(9600);
  #endif

  EEPROMValuesInit();

  // Задали вид экрана, сразу ставим ночную, чтобы если отключится элекричество
  // среди ночи, пользователю не выжгло бы глаза
  bigLEDScreen.setBrightness(getNightBrightness());
  // Почистили
  bigLEDScreen.clear();

  // Время удержания энкодера в миллисеундах для сброса к заводским настройкам
  encoder.setHoldTimeout(2000);

  char datetime[6] = "00.00";
  byte prevBrightness;

  bigLEDScreen.setText(datetime);

  #ifndef USE_SOFT_RTC
    DEBUG("Begin RTC");

    // Запускаем часики
    rtc.Begin();
    DEBUG("Begin i2c ok");
    DEBUG(rtc.LastError());
    
    rtc.LastError();
    rtc.SetIsRunning(true);

    DEBUG(rtc.GetIsRunning());
    DEBUG("Start RTC ok");

    DEBUG(rtc.GetDateTime().Hour());
    DEBUG(rtc.GetDateTime().Minute());
    DEBUG("End RTC"); 

    // Если время валидно, покажем его
    if(rtc.IsDateTimeValid())
    {
      DEBUG("RTC is valid");
      makeDateTimeScreen(datetime, rtc.GetDateTime().Hour(), rtc.GetDateTime().Minute(), true);
      // И сразу показали снова, а то вдруг оно поменялось
      prevBrightness = bigLEDScreen.getBrightness();
      bigLEDScreen.setBrightness(0);
      bigLEDScreen.setText(datetime); 
      bigLEDScreen.setBrightness(prevBrightness); 
    }
    else
    {
      DEBUG("RTC is invalid");
    }
  #else
    rtc.setDeltaT(SOFT_RTC_DELTA_T); 
  #endif 

DEBUG("Begin GPS");
GPS_SoftSerial.begin(SOFT_GPS_BAUD_RATE);
delay(3000);

#ifdef NEED_GPS_SETUP
  DEBUG("Begin GPS setup");
  initATGM332D();
  DEBUG("End GPS setup");
#endif 

  DEBUG("Begin GPS Fix");
  while(GPS_SoftSerial.available() > 0)
  {
      ATGM332D.encode(GPS_SoftSerial.read());
  }
  

  DEBUG(ATGM332D.time.hour());
  DEBUG(ATGM332D.time.minute());
  DEBUG("***");

  #ifndef USE_SOFT_RTC
    // Если железные часы, то попробуем поставить время и идём дальше.
    if(GPS_TIME_IS_VALID() && atoi(ATGM332D_year.value()) >= 2023)
      adjustTime(getGMTOffset());
  #else
    // А если часы софтверные, то первую установку надо делать до победного
    DEBUG("First RTC setting...");
    while(!(GPS_TIME_IS_VALID() && atoi(ATGM332D_year.value()) >= 2023))
    {
      while(GPS_SoftSerial.available() > 0)
      {
          ATGM332D.encode(GPS_SoftSerial.read());
      }
    }
    adjustTime(getGMTOffset());
    DEBUG("Done!");
  #endif

  DEBUG(ATGM332D_year.value());
  #ifndef USE_SOFT_RTC
    DEBUG(rtc.GetDateTime().Hour());
    DEBUG(rtc.GetDateTime().Minute());
  #else
    DEBUG(rtc.getHours());
    DEBUG(rtc.getMinutes());
  #endif
  DEBUG("***");

  float lat = 0.0;
  float lon = 0.0;
  
  // Тут будут координаты. Их мы будем далее использовать для расчёта восхода/заката.
  if(GPS_IS_VALID())
  {
    lat = ATGM332D.location.lat();
    lon = ATGM332D.location.lng();
  }

  DEBUG(lat);
  DEBUG(lon);
  DEBUG("End GPS"); 

  // Тут мы сразу посчитаем время захода и восхода
  DEBUG("Begin sunset/sunrise");
  sunsetSunrise sun;
  SunPosition sunHelper;
  if((lat == 0) && (lon == 0))
  {
    sun.rise = 0;
    sun.set = 0;
  }
  else
  {
    #ifndef USE_SOFT_RTC
      sunHelper.compute(lat, lon, rtc.GetDateTime().TotalSeconds(), getGMTOffset() / 60);
    #else
      sunHelper.compute(lat, lon, rtc.getTimestamp2000(), getGMTOffset() / 60);
    #endif
    sun.rise = sunHelper.sunrise();
    sun.set = sunHelper.sunset();
  }
  DEBUG(sun.rise);
  DEBUG(sun.set);
  DEBUG("End sunset/sunrise");

  // Теперь можно задавать правильную яркость
  bigLEDScreen.setBrightness(calculateBrightness(&sun));
  
  #ifndef USE_SOFT_RTC
    makeDateTimeScreen(datetime, rtc.GetDateTime().Hour(), rtc.GetDateTime().Minute(), true);
  #else
    makeDateTimeScreen(datetime, rtc.getHours(), rtc.getMinutes(), true);
  #endif
  // И сразу показали снова, а то вдруг оно поменялось
  prevBrightness = bigLEDScreen.getBrightness();
  bigLEDScreen.setBrightness(0);
  bigLEDScreen.setText(datetime); 
  bigLEDScreen.setBrightness(prevBrightness);
  

  bool dotRefreshFlag = true; // Флаги тут для того, чтобы не делать действия несколько раз за секунду!
  bool minRefreshFlag = true;
  Timer16 clockTimer(500); // Для опроса часов по I2C не чаще раза в полсекунды.

  #ifndef USE_SOFT_RTC
    uint8_t hour = rtc.GetDateTime().Hour();
    uint8_t minute = rtc.GetDateTime().Minute();
    uint8_t second = rtc.GetDateTime().Second();
  #else
    uint8_t hour = rtc.getHours();
    uint8_t minute = rtc.getMinutes();
    uint8_t second = rtc.getSeconds();
  #endif

  DEBUG("Start main cycle!");
  for(;;) 
  {
    // Опрос энкодера
    encoder.tick();

    // Если нажат энкодер, проваливаемся во вложенные режимы установки параметров.
    if(encoder.click())
    {
      setupModeGMTOffset();
      // И как только вернулись, надо персчитать время с учётом новых оффсетов и подтюнить яркость с учётом новых настроек.
      adjustTime(getGMTOffset());
      prevBrightness = calculateBrightness(&sun);
      bigLEDScreen.setBrightness(0);
      bigLEDScreen.setText(datetime); 
      bigLEDScreen.setBrightness(prevBrightness);
      encoder.resetState();
    }

    // Сброс к заводским настройкам!
    if(encoder.held())
    {
      EEPROMValuesInit(true);
      adjustTime(getGMTOffset());
      prevBrightness = calculateBrightness(&sun);
      bigLEDScreen.setBrightness(0);
      bigLEDScreen.setText(datetime); 
      bigLEDScreen.setBrightness(prevBrightness);
      encoder.resetState();
    }
    
    // Время с датчика надо брать постоянно, чтобы не переполнился буфер
    while(GPS_SoftSerial.available() > 0)
      ATGM332D.encode(GPS_SoftSerial.read());
    
    // Сохраняем значения, чтобы не дёргать лишний раз I2C
    if(clockTimer.ready())
    {
      #ifndef USE_SOFT_RTC
        hour   = rtc.GetDateTime().Hour();
        minute = rtc.GetDateTime().Minute();
        second = rtc.GetDateTime().Second();
      #else
        hour   = rtc.getHours();
        minute = rtc.getMinutes();
        second = rtc.getSeconds();
      #endif
      //DEBUG(second);
    }

    // Рисуем время только тогда, когда сменится минута (т.е. будет ровно 0 секунд)
    // Флаг тут для того, чтобы не делать это несколько раз за секунду!
    // Тут доп условие. Мигаем точкой только если за последние 10 сек было обновление по GPS
    if(((second % 2) && dotRefreshFlag))  
    {
      if(GPS_TIME_IS_VALID())
      {
        makeDateTimeScreen(datetime, hour, minute, false);
        prevBrightness = bigLEDScreen.getBrightness();
        bigLEDScreen.setBrightness(0); 
        noInterrupts();  // Иначе неприятно мерцает при перерисовке
          bigLEDScreen.setText(datetime); 
        interrupts(); 
        bigLEDScreen.setBrightness(prevBrightness);
      }
      dotRefreshFlag = !dotRefreshFlag;
    }
    else if((!(second % 2)) && !dotRefreshFlag)
    {
      makeDateTimeScreen(datetime, hour, minute, true);
      prevBrightness = bigLEDScreen.getBrightness();
      bigLEDScreen.setBrightness(0);
      noInterrupts();  // Иначе неприятно мерцает при перерисовке
        bigLEDScreen.setText(datetime); 
      interrupts();
      bigLEDScreen.setBrightness(prevBrightness);
      dotRefreshFlag = !dotRefreshFlag;
    }
      
    // Каждые 5 мин фиксим время и локацию, на всякий
    // Сюда же воткнём установку яркости, супер часто это делать смысла немного
    // И пересчёт восхода/заката сюда же, раз в час, ЧЧ:05
    if(minute % 5 ) 
    {
      if(minRefreshFlag)
      {
          if(ATGM332D.time.isValid())
          {
            adjustTime(getGMTOffset());
          }
          
          if(ATGM332D.location.isValid())
          {
            lat = ATGM332D.location.lat();
            lon = ATGM332D.location.lng();
          }

          minRefreshFlag = false;

          // И яркость проставим как надо
          if((lat == 0) && (lon == 0))
          {
            DEBUG("Set default sunset/sunrise");
            sun.rise = DEFAULT_SUNRISE_TIME;
            sun.set  =  DEFAULT_SUNSET_TIME;
          }
          else
          {
             #ifndef USE_SOFT_RTC
              sunHelper.compute(lat, lon, rtc.GetDateTime().TotalSeconds(), getGMTOffset() / 60);
            #else
              sunHelper.compute(lat, lon, rtc.getTimestamp2000(), getGMTOffset() / 60);
              DEBUG("TIMESTAMP2000:");
              DEBUG(rtc.getTimestamp2000());
            #endif
            sun.rise = sunHelper.sunrise();
            sun.set = sunHelper.sunset();
          }
          bigLEDScreen.setBrightness(calculateBrightness(&sun));
      }
    }
    else
      minRefreshFlag = true;
  }
}

void adjustTime(uint32_t GMTSecondsOffset)
{
  #ifndef USE_SOFT_RTC
    rtc.SetDateTime
    (
      RtcDateTime
      (
        atoi(ATGM332D_year.value()), 
        atoi(ATGM332D_month.value()), 
        atoi(ATGM332D_day.value()), 
        ATGM332D.time.hour(), 
        ATGM332D.time.minute(),
        ATGM332D.time.second() 
      ) + GMTSecondsOffset
    );
  #else
    RtcDateTime dt
    (
      atoi(ATGM332D_year.value()), 
      atoi(ATGM332D_month.value()), 
      atoi(ATGM332D_day.value()), 
      ATGM332D.time.hour(), 
      ATGM332D.time.minute(),
      ATGM332D.time.second() 
    );
    dt += GMTSecondsOffset;
    rtc.stopRTC();
      rtc.setDate(dt.Day(), dt.Month(), dt.Year());
      rtc.setTime(dt.Hour(), dt.Minute(), dt.Second());
    rtc.startRTC();
  #endif
}

byte calculateBrightness(sunsetSunrise* sun)
{
  DEBUG("Enter calculateBrightness()");
  #ifndef USE_SOFT_RTC
    RtcDateTime timeNow  = rtc.GetDateTime();
  #else
    RtcDateTime timeNow(rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
  #endif
  uint32_t    timeSet  = RtcDateTime(timeNow.Year(), timeNow.Month(), timeNow.Day(), 0, 0 ,0).TotalSeconds() + ((uint32_t)sun->set * 60); 
  uint32_t    timeRise = RtcDateTime(timeNow.Year(), timeNow.Month(), timeNow.Day(), 0, 0 ,0).TotalSeconds() + ((uint32_t)sun->rise * 60); 

  DEBUG(sun->rise);
  DEBUG(sun->set);

  if((sun->rise == 0) && (sun->set == 0)) 
  {
    DEBUG("RISE=SET=0");
    return getNightBrightness(); // Если время восхода/заката непонятно, то яркость ночная, чтобы случайно не сжечь глаза.
  }
  else
  {
    DEBUG("RISE/SET:");
    DEBUG(timeRise);
    DEBUG(timeSet);
    DEBUG(timeNow.TotalSeconds());
    if((timeNow.TotalSeconds() > timeRise) && (timeNow.TotalSeconds() < timeSet)) // День, поскольку мы между закатом и рассветом
      {
        DEBUG("It's a day!");
        return getDayBrightness();
      }
    else
      {
        DEBUG("It's a night!");
        return getNightBrightness();
      }
  }
}
