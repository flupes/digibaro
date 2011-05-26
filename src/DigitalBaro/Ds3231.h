#ifndef Ds3231_h
#define Ds3231_h

#include <wiring.h>

#include "Time.h"

class Ds3231 {

public:
  Ds3231(uint8_t address);
  
  time_t getRtcTime();

  void readData();

  void printTime(char str[]);
  
protected:

  char *printTwoDec(byte val, char *ptr);

  uint8_t m_rtcAddress;
  
  tmElements_t m_time;

};

#endif
