#include <Arduino.h>
#include "settings.h"
#include <EncButton.h>
#include "lib7SegmentScreenShifted.h"
#include <RtcDateTime.h>

extern EncButton<EB_TICK, ENCODER_S1, ENCODER_S2, ENCODER_KEY> encoder;
extern sevenSegmentScreenShifted bigLEDScreen;

void makeDateTimeScreen(char* datetime, uint8_t hr, uint8_t min, bool dot);
uint32_t getGMTOffset(void);
void setGMTOffset(uint32_t offset);
void setDayBrightnessMode(void);
void setNightrightnessMode(void);
byte getDayBrightness(void);
byte getNightBrightness(void);
void setNightBrightness(byte);
void setDayBrightness(byte);

// Установка смещения часового пояса и потом проваливаемся в утановку дневной яркости
void setupModeGMTOffset(void)
{
    char datetime[6] = "00.00";
    byte prevBrightness;

    uint32_t gmtSecondsOffset = getGMTOffset();
    RtcDateTime time(2023, 12, 17, (byte)(gmtSecondsOffset / 60 / 60), (byte)(gmtSecondsOffset / 60 % 60), 0);
    makeDateTimeScreen(datetime, time.Hour(), time.Minute(), true);
    prevBrightness = bigLEDScreen.getBrightness();
    bigLEDScreen.setBrightness(0);
    bigLEDScreen.setText(datetime); 
    bigLEDScreen.setBrightness(prevBrightness); 

    
    for(;;)
    {
        encoder.tick();
        if (encoder.right())
        {
            time += (15 * 60); // 15 минут в секундах
            makeDateTimeScreen(datetime, time.Hour(), time.Minute(), true);
            prevBrightness = bigLEDScreen.getBrightness();
            bigLEDScreen.setBrightness(0);
            bigLEDScreen.setText(datetime); 
            bigLEDScreen.setBrightness(prevBrightness); 
        }
        if (encoder.left())
        {
            time -= (15 * 60); // 15 минут в секундах
            makeDateTimeScreen(datetime, time.Hour(), time.Minute(), true);
            prevBrightness = bigLEDScreen.getBrightness();
            bigLEDScreen.setBrightness(0);
            bigLEDScreen.setText(datetime); 
            bigLEDScreen.setBrightness(prevBrightness); 
        }
        if (encoder.click())
        {
            uint32_t offset = time.Hour() * 60 * 60 + time.Minute() * 60;
            if(getGMTOffset() != offset)
                setGMTOffset(offset);
            setDayBrightnessMode();
            return;
        }
    }
}

// Ставим дневную яркость и проваливаемся в установку ночной яркости
void setDayBrightnessMode(void)
{
    byte dayBrightness = getDayBrightness();
    char varchr[5] = "    ";

    if (dayBrightness >= 100)
        itoa(dayBrightness / 10, varchr, 10);
    else
        itoa(dayBrightness / 10, varchr + 1, 10);
    bigLEDScreen.setBrightness(0);
    bigLEDScreen.setText(varchr);
    bigLEDScreen.setBrightness(dayBrightness);

    for(;;)
    {
        encoder.tick();

        if (encoder.right())
        {
            switch (dayBrightness)
            {
            case 240:
                dayBrightness = 255;
                break;

            case 255:
                dayBrightness = 10;
                break;
            
            default:
                dayBrightness += 10;
                break;
            }
            
            byte varchrOffset = 0;
            if(dayBrightness <= 90)
            {
                varchr[0] = ' ';
                varchrOffset = 1;
            }
            itoa(dayBrightness / 10, varchr + varchrOffset, 10);
            bigLEDScreen.setBrightness(0);
            bigLEDScreen.setText(varchr);
            bigLEDScreen.setBrightness(dayBrightness);
        }

        if (encoder.left())
        {
            switch (dayBrightness)
            {
            case 10:
                dayBrightness = 255;
                break;

            case 255:
                dayBrightness = 240;
                break;
            
            default:
                dayBrightness -= 10;
                break;
            }
            
            byte varchrOffset = 0;
            if(dayBrightness <= 90)
            {
                varchr[0] = ' ';
                varchrOffset = 1;
            }
            itoa(dayBrightness / 10, varchr + varchrOffset, 10);
            bigLEDScreen.setBrightness(0);
            bigLEDScreen.setText(varchr);
            bigLEDScreen.setBrightness(dayBrightness);
        }

        if(encoder.click())
        {
            if(getDayBrightness() != dayBrightness)
                setDayBrightness(dayBrightness);
            setNightrightnessMode();
            return;
        }
    }
}

// Ставим ночную яркость и возвращаемся обратно в main()
void setNightrightnessMode(void)
{
    byte nightBrightness = getNightBrightness();
    char varchr[5] = "    ";

    if(nightBrightness >= 100)
        itoa(nightBrightness / 10, varchr + 2, 10);
    else
        itoa(nightBrightness / 10, varchr + 3, 10);
    bigLEDScreen.setBrightness(0);
    bigLEDScreen.setText(varchr);
    bigLEDScreen.setBrightness(nightBrightness);

    for(;;)
    {
        encoder.tick();

        if (encoder.right())
        {
            switch (nightBrightness)
            {
            case 240:
                nightBrightness = 255;
                break;

            case 255:
                nightBrightness = 10;
                break;
            
            default:
                nightBrightness += 10;
                break;
            }
            
            byte varchrOffset = 2;
            if(nightBrightness <= 90)
            {
                varchr[2] = ' ';
                varchrOffset = 3;
            }
            itoa(nightBrightness / 10, varchr + varchrOffset, 10);
            bigLEDScreen.setBrightness(0);
            bigLEDScreen.setText(varchr);
            bigLEDScreen.setBrightness(nightBrightness);
        }

        if (encoder.left())
        {
            switch (nightBrightness)
            {
            case 10:
                nightBrightness = 255;
                break;

            case 255:
                nightBrightness = 240;
                break;
            
            default:
                nightBrightness -= 10;
                break;
            }
            
            varchr[0] = ' ';
            varchr[1] = ' ';
            byte varchrOffset = 2;
            if(nightBrightness <= 90)
            {
                varchr[2] = ' ';
                varchrOffset = 3;
            }
            itoa(nightBrightness / 10, varchr + varchrOffset, 10);
            bigLEDScreen.setBrightness(0);
            bigLEDScreen.setText(varchr);
            bigLEDScreen.setBrightness(nightBrightness);
        }

        if(encoder.click())
        {
            if(getNightBrightness() != nightBrightness)
                setNightBrightness(nightBrightness);
            return;
        }
    } 
}