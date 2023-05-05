#include <Arduino.h>
#include "settings.h"

// Эксперимент
#ifdef FAST_SHIFT_OUT
  #include <FastShiftOut.h>
  #define shiftOut(DATA_PIN, CLOCK, ORDER, VALUE) FSO.write(VALUE)
  FastShiftOut FSO(DATA, CLOCK, LSBFIRST);
#endif

#include "lib7SegmentScreenShifted.h"

#define SP_PRECISE
#include "SunPosition.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
//#include <NeoSWSerial.h>
#include <microWire.h>   // или просто Wire
//#include <Wire.h>
#include <RtcDS1307.h>
#include <EncButton.h>

// Часики и экран и энкодер нужны глобально
RtcDS1307<TwoWire> rtc(Wire);
sevenSegmentScreenShifted bigLEDScreen(LATCH, DATA, CLOCK, PWM, 4, COMMON_ANODE);
EncButton<EB_TICK, ENCODER_S1, ENCODER_S2, ENCODER_KEY> encoder;

// Всё, связанное с GPS, тоже нужно глобально
SoftwareSerial GPS_SoftSerial(TX_PIN, RX_PIN);
TinyGPSPlus ATGM332D;
TinyGPSCustom ATGM332D_year(ATGM332D, "GNZDA", 4);
TinyGPSCustom ATGM332D_month(ATGM332D, "GNZDA", 3);
TinyGPSCustom ATGM332D_day(ATGM332D, "GNZDA", 2);

// Правильно заполняет массив с датой/временем
void makeDateTimeScreen(char* datetime, uint8_t hr, uint8_t min, bool dot);

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

  #ifdef DEBUG_ENABLE
    Serial.begin(9600);
  #endif

  EEPROMValuesInit();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  pinMode(CLOCK,  OUTPUT);
  pinMode(DATA,   OUTPUT);
  pinMode(LATCH,  OUTPUT); 
  pinMode(PWM,    OUTPUT); 
  pinMode(TX_PIN,  INPUT);
  pinMode(RX_PIN, OUTPUT);

  // Задали вид экрана, сразу ставим ночную, чтобы если отключится элекричество
  // среди ночи, пользователю не выжгло бы глаза
  bigLEDScreen.setBrightness(getNightBrightness());
  // Почистили
  bigLEDScreen.clear();

  // Время удержания энкодера в миллисеундах для сброса к заводским настройкам
  encoder.setHoldTimeout(2000);
  
  DEBUG("Begin RTC");

  // Запускаем часики
  rtc.Begin();
  DEBUG("Begin i2c ok");
  DEBUG(rtc.LastError());
  
  rtc.LastError();
  if (!rtc.GetIsRunning())
    rtc.SetIsRunning(true);

  DEBUG("Start RTC ok");

  DEBUG(rtc.GetDateTime().Hour());
  DEBUG(rtc.GetDateTime().Minute());
  DEBUG("End RTC");    

  char datetime[6] = "00.00";
  byte prevBrightness;

  bigLEDScreen.setText(datetime);
  
  // Если время валидно, покажем его
  if(rtc.IsDateTimeValid())
  {
    makeDateTimeScreen(datetime, rtc.GetDateTime().Hour(), rtc.GetDateTime().Minute(), true);
    // И сразу показали снова, а то вдруг оно поменялось
    prevBrightness = bigLEDScreen.getBrightness();
    bigLEDScreen.setBrightness(0);
    bigLEDScreen.setText(datetime); 
    bigLEDScreen.setBrightness(prevBrightness); 
  }

DEBUG("Begin GPS");
GPS_SoftSerial.begin(9600);
delay(3000);

#ifdef NEED_GPS_SETUP
  DEBUG("Begin GPS setup");
  initATGM332D();
  DEBUG("END GPS setup");
#endif 

  // Первично ставим время и координаты
  //while(!(ATGM332D.time.isValid() && ATGM332D_day.isValid() && ATGM332D_month.isValid() && ATGM332D_year.isValid() && ATGM332D.location.isValid()))
  while(!(ATGM332D.time.isValid() && ATGM332D_day.isValid() && ATGM332D_month.isValid() && ATGM332D_year.isValid()))
    while(GPS_SoftSerial.available())
      {
        ATGM332D.encode(GPS_SoftSerial.read());
        DEBUG(ATGM332D_year.value());
        DEBUG(ATGM332D.location.lat());
        DEBUG(ATGM332D.location.lat());
        DEBUG("****");
      }
  adjustTime(getGMTOffset());
  // Тут будут координаты. Их мы будем далее использовать для расчёта восхода/заката.
  float lat = ATGM332D.location.lat();
  float lon = ATGM332D.location.lat();;

  DEBUG(lat);
  DEBUG(lon);
  DEBUG("End GPS");

  // Тут мы сразу посчитаем время захода и восхода
  DEBUG("Begin sunset/sunrise");
  sunsetSunrise sun;
  SunPosition sunHelper;
  if((lat == 0) && (lon ==0))
  {
    sun.rise = 0;
    sun.set = 0;
  }
  else
  {
    sunHelper.compute(lat, lon, rtc.GetDateTime().TotalSeconds(), getGMTOffset() / 60);
    sun.rise = sunHelper.sunrise();
    sun.set = sunHelper.sunset();
  }
  DEBUG(sun.rise);
  DEBUG(sun.set);
  DEBUG("End sunset/sunrise");

  // Теперь можно задавать правильную яркость
  bigLEDScreen.setBrightness(calculateBrightness(&sun));
  
  makeDateTimeScreen(datetime, rtc.GetDateTime().Hour(), rtc.GetDateTime().Minute(), true);
  // И сразу показали снова, а то вдруг оно поменялось
  prevBrightness = bigLEDScreen.getBrightness();
  bigLEDScreen.setBrightness(0);
  bigLEDScreen.setText(datetime); 
  bigLEDScreen.setBrightness(prevBrightness);
  

  bool dotRefreshFlag = true; // Флаги тут для того, чтобы не делать действия несколько раз за секунду!
  bool minRefreshFlag = true;
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
    while(GPS_SoftSerial.available())
      ATGM332D.encode(GPS_SoftSerial.read());
    
    // Сохраняем значения, чтобы не дёргать лишний раз SPI
    uint8_t hour = rtc.GetDateTime().Hour();
    uint8_t minute = rtc.GetDateTime().Minute();
    uint8_t second = rtc.GetDateTime().Second();

    // Рисуем время только тогда, когда сменится минута (т.е. будет ровно 0 секунд)
    // Флаг тут для того, чтобы не делать это несколько раз за секунду!
    // Тут доп условие. Мигаем точкой только если за последние 10 сек было обновление по GPS
    if(((second % 2) && dotRefreshFlag) && (ATGM332D.time.age() <= 10000))  
    {
      makeDateTimeScreen(datetime, hour, minute, false);
      prevBrightness = bigLEDScreen.getBrightness();
      bigLEDScreen.setBrightness(0); 
      noInterrupts();  // Иначе неприятно мерцает при перерисовке
        bigLEDScreen.setText(datetime); 
      interrupts(); 
      bigLEDScreen.setBrightness(prevBrightness);
      dotRefreshFlag = !dotRefreshFlag;
    }
    else if(!(second % 2) && !dotRefreshFlag)
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
    // И пересчёт восхода/заката сюда же, в 00:05
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
          bigLEDScreen.setBrightness(calculateBrightness(&sun));
          if((hour == 0) && minute == 5)
          {
            if((lat == 0) && (lon == 0))
            {
              sun.rise = 0;
              sun.set = 0;
            }
            else
            {
              sunHelper.compute(lat, lon, rtc.GetDateTime().TotalSeconds(), getGMTOffset() / 60);
              sun.rise = sunHelper.sunrise();
              sun.set = sunHelper.sunset();
            }
          }
      }
    }
    else
      minRefreshFlag = true;
  }
}

void adjustTime(uint32_t GMTSecondsOffset)
{
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
}

byte calculateBrightness(sunsetSunrise* sun)
{
  RtcDateTime timeNow  = rtc.GetDateTime();
  uint32_t    timeSet  = RtcDateTime(timeNow.Year(), timeNow.Month(), timeNow.Day(), 0, 0 ,0).TotalSeconds() + sun->set * 60; 
  uint32_t    timeRise = RtcDateTime(timeNow.Year(), timeNow.Month(), timeNow.Day(), 0, 0 ,0).TotalSeconds() + sun->rise * 60; 

  if((sun->rise == 0) && (sun->set == 0)) 
  {
    return getNightBrightness(); // Если время восхода/заката непонятно, то яркость ночная, чтобы случайно не сжечь глаза.
  }
  else
  {
    if((timeNow.TotalSeconds() > timeRise) && (timeNow.TotalSeconds() < timeSet)) // День, поскольку мы между закатом и рассветом
      return getDayBrightness();
    else
      return getNightBrightness();
  }
}