#include "WeatherLcdGraph.h"

#include "WeatherSample.h"

#define GRAPH_Y_PIXELS 32

WeatherLcdGraph::WeatherLcdGraph(TimePermRingBuffer& buf, byte lineOffset) :
  m_data(buf),
  m_graphX(LCDWIDTH-m_data.bufferSize()),
  m_graphY(lineOffset*8),
  m_lineOffset(lineOffset)
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
  uint8_t x, y, j;
  WeatherSample sample;
  uint16_t size = m_data.bufferSize();
  lcd.fillrect(m_graphX, m_graphY, m_graphX+size, m_graphY+GRAPH_Y_PIXELS, 0);
  for (uint16_t i=0; i<size; i++) {
    time = m_data.read(i, sample);
    pressure = sample.getPressure();
    if ( pressure != 0xFFFF ) {
      x = (uint8_t)(m_graphX+size-i-1);
      y = (uint8_t)((pressure-m_minY)/m_scale);
      if ( y > GRAPH_Y_PIXELS ) {
        y = GRAPH_Y_PIXELS;
      }
    // Serial.print("pressure=");
    // Serial.print(pressure, DEC);
    // Serial.print(" -> y=");
    // Serial.println(y, DEC);
    //lcd.drawline(x, m_graphY, x, m_graphY+GRAPH_Y_PIXELS, 0);
    //lcd.drawline(x, m_graphY+GRAPH_Y_PIXELS-y, x, m_graphY+GRAPH_Y_PIXELS, 1);
      for (j=0; j<=y; j++)
        lcd.setpixel(x, m_graphY+GRAPH_Y_PIXELS-j, 1);
    }
  }
  for (j=m_graphX; j<=LCDWIDTH; j++)
    lcd.setpixel(j, m_graphY+GRAPH_Y_PIXELS, 1);
  for (j=0; j<=GRAPH_Y_PIXELS; j++)
    lcd.setpixel(m_graphX-1, m_graphY+j, 1);
  char str[8];
  utoa(m_maxY, str, 10);
  lcd.drawstring(0, m_lineOffset, str);
  utoa(m_minY, str, 10);
  lcd.drawstring(0, m_lineOffset+3, str);
}
