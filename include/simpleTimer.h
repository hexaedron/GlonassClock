#include <Arduino.h>

class Timer32 {
  public:
    void Timer(uint32_t nprd = 0) {
     setPeriod(nprd);
    }
    void setPeriod(uint32_t nprd) {
      prd = nprd;
    }
    bool ready() {
      return (prd && millis() - tmr >= prd) ? (tmr = millis(), 1) : 0;
    }
  private:
    uint32_t tmr = 0, prd = 0;
};

class Timer16 {
  public:
    void Timer(uint16_t nprd = 0) {
      setPeriod(nprd);
    }
    void setPeriod(uint16_t nprd) {
      prd = nprd;
    }
    bool ready() {
      return (prd && millis() - tmr >= prd) ? (tmr = millis(), 1) : 0;
    }
  private:
    uint16_t tmr = 0, prd = 0;
};

class Timer8 {
  public:
    void Timer(uint8_t nprd = 0) {
      setPeriod(nprd);
    }
    void setPeriod(uint8_t nprd) {
      prd = nprd;
    }
    bool ready() {
      return (prd && millis() - tmr >= prd) ? (tmr = millis(), 1) : 0;
    }
  private:
    uint8_t tmr = 0, prd = 0;
};