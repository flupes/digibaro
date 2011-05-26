#include "Ds3231.h"

#include <string.h>
#include <stdlib.h>
#include <Wire.h>

/*
  Code inspired from the sketch at:
  http://code.google.com/p/gfb/
*/

Ds3231 *Ds3231::m_this = 0;

Ds3231::Ds3231(uint8_t address) :
  m_rtcAddress(address)
{
  m_this = this;
}

time_t Ds3231::getTimeSync()
{
  return m_this->getRtcTime();
}

time_t Ds3231::getRtcTime()
{
  readData();
  return ( makeTime(m_time) );
}

void Ds3231::readData()
{
  // send request to receive data starting at register 0
  Wire.beginTransmission(m_rtcAddress);
  Wire.send(0x00); // start at register 0
  Wire.endTransmission();
  Wire.requestFrom(m_rtcAddress, (uint8_t)7); // request seven bytes
  
  if(Wire.available()) { 
    m_time.Second =  Wire.receive();
    m_time.Minute = Wire.receive();
    m_time.Hour   = Wire.receive();
    m_time.Wday     = Wire.receive();
    m_time.Day    = Wire.receive();
    m_time.Month   = Wire.receive();
    m_time.Year    = Wire.receive();       
  }
  else {
    //oh noes, no data!
  }

  m_time.Second = ((m_time.Second & B11110000)>>4)*10 +
    (m_time.Second & B00001111);           // convert BCD to decimal
  m_time.Minute = ((m_time.Minute & B11110000)>>4)*10
    + (m_time.Minute & B00001111);         // convert BCD to decimal
  m_time.Hour = ((m_time.Hour & B00110000)>>4)*10
    + (m_time.Hour & B00001111);  // convert BCD to decimal (assume 24 hour mode)
  m_time.Wday = (m_time.Wday & B00000111); // 1-7
  m_time.Day= ((m_time.Day & B00110000)>>4)*10 + (m_time.Day & B00001111); // 1-31
  m_time.Month = ((m_time.Month & B00010000)>>4)*10 + (m_time.Month & B00001111); //msb7 is century overflow
  m_time.Year = ((m_time.Year & B11110000)>>4)*10 + (m_time.Year & B00001111);

}

void Ds3231::printTime(char str[])
{
  char *pdate = str;
  pdate = printTwoDec(m_time.Hour, pdate);
  strcpy(pdate++, ":");
  pdate = printTwoDec(m_time.Minute, pdate);
  strcpy(pdate++, ":");
  pdate = printTwoDec(m_time.Second, pdate);
  strcpy(pdate++, " ");
  strcpy(pdate, dayShortStr(m_time.Wday));
  pdate += 3;
  strcpy(pdate++, " ");
  pdate = printTwoDec(m_time.Month, pdate);
  strcpy(pdate++, "/");
  pdate = printTwoDec(m_time.Day, pdate);
}

char *Ds3231::printTwoDec(byte val, char *ptr)
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
