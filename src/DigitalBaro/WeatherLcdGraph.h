#ifndef WeatherLcdGraph_h
#define WeatherLcdGraph_h

#include "TimePermRingBuffer.h"
#include "ST7565.h"

class WeatherLcdGraph
{
public:
  WeatherLcdGraph();

  void setBuffer(TimePermRingBuffer *buf);

  void setLimits(int16_t min, int16_t max);

  int16_t minY();

  int16_t maxY();

  void draw(ST7565 &lcd);

  TimePermRingBuffer *getBuffer();

protected:
  void findExtremas();
  
  int16_t m_minY;
  int16_t m_maxY;

  uint16_t m_minPressure;
  uint16_t m_maxPressure;
  
  long m_minPressTime;
  long m_maxPressTime;

  float m_scale;

  TimePermRingBuffer *m_data;

  uint8_t m_graphX;
  uint8_t m_graphY;

};

#endif
