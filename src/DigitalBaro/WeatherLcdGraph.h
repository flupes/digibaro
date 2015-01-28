#ifndef WeatherLcdGraph_h
#define WeatherLcdGraph_h

#include "TimePermRingBuffer.h"
#include "U8glib.h"

class WeatherLcdGraph
{
public:
  WeatherLcdGraph();

  void setBuffer(TimePermRingBuffer *buf);

  void setLimits(int16_t min, int16_t max);

  int16_t minY() { return m_minY; }
  int16_t maxY() { return m_maxY; }

  uint16_t getMinPressure(long *timestamp=0);
  uint16_t getMaxPressure(long *timestamp=0);

  void draw(U8GLIB &lcd);

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

  u8g_uint_t m_graphX;
  u8g_uint_t m_graphY;

};

#endif
