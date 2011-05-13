
#include <wiring.h>

class RTC_DS3231 {

public:
  RTC_DS3231(uint8_t address);
  
  void readData();

  void printTime(char str[]);
  
protected:

  char *printTwoDec(byte val, char *ptr);

  uint8_t m_rtcAddress;
  
  byte m_seconds;
  byte m_minutes;
  byte m_hours;
  byte m_day;
  byte m_date;
  byte m_month;
  byte m_year;

};
