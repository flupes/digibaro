#ifndef Ds3231_h
#define Ds3231_h

#include <Arduino.h> // just for boolean... this is a shame!

#include "Time.h"

class Ds3231 {

public:
  Ds3231(uint8_t address);

  // Note: this only works when a single instance of Ds3231 is created!
  static time_t getTimeSync();

  time_t getRtcTime();

  void readData();

  void printTime(char str[]);
  
  boolean enableSquareWave(byte freq);
  void disableSquareWave();

  //squarewave frequencies:
  static const byte SQW_1Hz=0x00;   // 1Hz
  static const byte SQW_1kHz=0x01;  // 1024Hz
  static const byte SQW_4kHz=0x02;  // 4069Hz
  static const byte SQW_8kHz=0x03;  // 8192Hz

protected:
  static Ds3231 *m_this;

  char *printTwoDec(byte val, char *ptr);

  uint8_t m_rtcAddress;
  
  tmElements_t m_time;

};

#endif
