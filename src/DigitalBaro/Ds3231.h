#ifndef Ds3231_h
#define Ds3231_h

#include <wiring.h>

#include "Time.h"

class Ds3231 {

public:
  Ds3231(uint8_t address);
  
  void readData();

  void printTime(char str[]);
  
  bool newSecond();
  bool newMinute();

protected:

  char *printTwoDec(byte val, char *ptr);

  uint8_t m_rtcAddress;
  
  time_t m_time;

  byte m_seconds;
  byte m_minutes;
  byte m_hours;
  byte m_day;
  byte m_date;
  byte m_month;
  byte m_year;

  byte m_oldMinutes;
  byte m_oldSeconds;
};

#endif
