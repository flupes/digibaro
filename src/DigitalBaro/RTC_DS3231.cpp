#include "RTC_DS3231.h"

#include <string.h>
#include <stdlib.h>
#include <Wire.h>

/*
  Code inspired from the sketch at:
  http://code.google.com/p/gfb/
*/

RTC_DS3231::RTC_DS3231(uint8_t address) :
  m_rtcAddress(address)
{

}

void RTC_DS3231::readData()
{
  // send request to receive data starting at register 0
  Wire.beginTransmission(m_rtcAddress);
  Wire.send(0x00); // start at register 0
  Wire.endTransmission();
  Wire.requestFrom(m_rtcAddress, (uint8_t)7); // request seven bytes
  
  if(Wire.available()) { 
    m_seconds = Wire.receive();
    m_minutes = Wire.receive();
    m_hours   = Wire.receive();
    m_day     = Wire.receive();
    m_date    = Wire.receive();
    m_month   = Wire.receive();
    m_year    = Wire.receive();       
  }
  else {
    //oh noes, no data!
  }

  m_seconds = (((m_seconds & B11110000)>>4)*10 + (m_seconds & B00001111)); // convert BCD to decimal
  m_minutes = (((m_minutes & B11110000)>>4)*10 + (m_minutes & B00001111)); // convert BCD to decimal
  m_hours   = (((m_hours & B00110000)>>4)*10 + (m_hours & B00001111)); // convert BCD to decimal (assume 24 hour mode)
  m_day     = (m_day & B00000111); // 1-7
  m_date    = (((m_date & B00110000)>>4)*10 + (m_date & B00001111)); // 1-31
  m_month   = (((m_month & B00010000)>>4)*10 + (m_month & B00001111)); //msb7 is century overflow
  m_year    = (((m_year & B11110000)>>4)*10 + (m_year & B00001111));

}

void RTC_DS3231::printTime(char str[])
{
  char *pdate = str;
  pdate = printTwoDec(m_hours, pdate);
  strcpy(pdate++, ":");
  pdate = printTwoDec(m_minutes, pdate);
  strcpy(pdate++, ":");
  pdate = printTwoDec(m_seconds, pdate);
  strcpy(pdate++, " ");
  switch (m_day) {
    case 1:
      strcpy(pdate, "Sun");
      break;
    case 2:
      strcpy(pdate, "Mon");
      break;
    case 3:
      strcpy(pdate, "Tue");
      break;
    case 4:
      strcpy(pdate, "Wed");
      break;
    case 5:
      strcpy(pdate, "Thu");
      break;
    case 6:
      strcpy(pdate, "Fri");
      break;
    case 7:
      strcpy(pdate, "Sat");
      break;
  }
  pdate += 3;
  strcpy(pdate++, " ");
  pdate = printTwoDec(m_month, pdate);
  strcpy(pdate++, "/");
  pdate = printTwoDec(m_date, pdate);
}

char *RTC_DS3231::printTwoDec(byte val, char *ptr)
{
  int num;
  num = (int)val;
  if (val<10) {
    strcpy(ptr++, "0");
    itoa(num, ptr++, 10);
  }
  else {
    itoa(num, ptr, 10);
    ptr += 2;
  }
  return ptr;
}
