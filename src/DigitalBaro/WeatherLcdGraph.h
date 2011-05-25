#ifndef WeatherLcdGraph_h
#define WeatherLcdGraph_h

#include "TimePermRingBuffer.h"
#include "ST7565.h"

class WeatherLcdGraph
{
public:
  WeatherLcdGraph(TimePermRingBuffer& buf, byte yOffset);

  void setLimits(uint16_t min, uint16_t max);

  void draw(ST7565 &lcd);

protected:
  void findExtremas();
  
  uint16_t m_minY;
  uint16_t m_maxY;

  uint16_t m_minPressure;
  uint16_t m_maxPressure;
  
  long m_minPressTime;
  long m_maxPressTime;

  float m_scale;

  TimePermRingBuffer &m_data;

  uint8_t m_graphX;
  uint8_t m_graphY;
};

#endif
