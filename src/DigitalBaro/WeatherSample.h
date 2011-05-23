#ifndef WeatherSample_h
#define WeatherSample_h

#include "DataSample.h"

struct WeatherData {
  uint16_t pressure;
  uint16_t temperature;
};

class WeatherSample : public DataSample
{
public:
  WeatherSample() : DataSample(sizeof(WeatherData))
  {
    m_data.pressure = 0xFFFF;
    m_data.temperature = 0xFFFF;
  }

  void *data() {
    return (void *)&m_data;
  }

  /** Get the pressure in Deci Pascal */
  uint16_t getPressure() {
    return m_data.pressure;
  }

  /** Set the pressure in Deci Pascal */
  void setPressure(uint16_t p) {
    m_data.pressure = p;
  }

protected:
  /** Pressure in Deci Pascal */
  WeatherData m_data;
  
};

#endif
