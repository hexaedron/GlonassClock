#include <Arduino.h>

class Timer32 
{
  public:

    Timer32 () {}

    Timer32 (uint32_t nprd) 
    {
      start(nprd);
    }

    void start(uint32_t nprd) 
    {
      prd = nprd;
      start();
    }

    void start() 
    {
      tmr = millis();
      if (!tmr) tmr = 1;
    }

    void stop() 
    {
      tmr = 0;
    }

    bool ready() 
    {
      if (tmr && millis() - tmr >= prd) 
      {
        start();
        return 1;
      }
      return 0;
    }

  private:
    uint32_t tmr = 0, prd = 0;
};

class Timer16 
{
  public:

    Timer16 () {}

    Timer16 (uint16_t nprd) 
    {
      start(nprd);
    }

    void start(uint16_t nprd) 
    {
      prd = nprd;
      start();
    }

    void start() 
    {
      tmr = millis();
      if (!tmr) tmr = 1;
    }

    void stop() 
    {
      tmr = 0;
    }

    bool ready() 
    {
      if (tmr && millis() - tmr >= prd) 
      {
        start();
        return 1;
      }
      return 0;
    }

  private:
    uint32_t tmr = 0;
    uint16_t prd = 0;
};

class Timer8 
{
  public:

    Timer8 () {}

    Timer8 (uint8_t nprd) 
    {
      start(nprd);
    }

    void start(uint8_t nprd) 
    {
      prd = nprd;
      start();
    }

    void start() 
    {
      tmr = millis();
      if (!tmr) tmr = 1;
    }

    void stop() 
    {
      tmr = 0;
    }

    bool ready() 
    {
      if (tmr && millis() - tmr >= prd) 
      {
        start();
        return 1;
      }
      return 0;
    }
    
  private:
    uint32_t tmr = 0; 
    uint8_t  prd = 0;
};