#include "WeatherLcdGraph.h"

#include "WeatherSample.h"

#define GRAPH_Y_PIXELS 32
#define GRAPH_X_POS 56
#define GRAPH_Y_POS 16

WeatherLcdGraph::WeatherLcdGraph(TimePermRingBuffer& buf) :
  m_data(buf)
{
  findExtremas();
}

void WeatherLcdGraph::setLimits(uint16_t min, uint16_t max)
{
  m_minY = min;
  m_maxY = max;
  m_scale = (max-min)/GRAPH_Y_PIXELS;
}

void WeatherLcdGraph::findExtremas()
{
  m_minPressure = 12000;
  m_maxPressure = 0;
  uint16_t pressure;
  long time;
  WeatherSample sample;
  for (uint16_t i=0; i<m_data.bufferSize(); i++) {
    time = m_data.read(i, sample);
    pressure = sample.getPressure();
    if ( pressure != 0xFFFF ) {
      if ( pressure < m_minPressure ) {
        m_minPressure = pressure;
        m_minPressTime = time;
      }
      if ( pressure > m_maxPressure ) {
        m_maxPressure = pressure;
        m_maxPressTime = time;
      }
    }
  }
}

void WeatherLcdGraph::draw(ST7565 &lcd)
{
  uint16_t pressure;
  long time;
  uint8_t x, y;
  WeatherSample sample;
  uint16_t size = m_data.bufferSize();
  lcd.fillrect(GRAPH_X_POS, GRAPH_Y_POS, GRAPH_X_POS+size, GRAPH_Y_POS+GRAPH_Y_PIXELS, 0);
  for (uint16_t i=0; i<size; i++) {
    time = m_data.read(i, sample);
    pressure = sample.getPressure();
    x = (uint8_t)(GRAPH_X_POS+size-i);
    y = (uint8_t)((pressure-m_minY)/m_scale);
    if ( y > GRAPH_Y_PIXELS ) {
      y = GRAPH_Y_PIXELS;
    }
    // Serial.print("pressure=");
    // Serial.print(pressure, DEC);
    // Serial.print(" -> y=");
    // Serial.println(y, DEC);
    //lcd.drawline(x, GRAPH_Y_POS, x, GRAPH_Y_POS+GRAPH_Y_PIXELS, 0);
    //lcd.drawline(x, GRAPH_Y_POS+GRAPH_Y_PIXELS-y, x, GRAPH_Y_POS+GRAPH_Y_PIXELS, 1);
    lcd.setpixel(x, GRAPH_Y_POS+GRAPH_Y_PIXELS-y, 1);
  }
}
