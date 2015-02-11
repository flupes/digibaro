#include "WeatherLcdGraph.h"

#include "WeatherSample.h"

#define GRAPH_Y_PIXELS 32
#define GRAPH_LINE_OFFSET 3

WeatherLcdGraph::WeatherLcdGraph() :
  m_data(0),
  m_graphY(GRAPH_LINE_OFFSET*8)
{
  // do nothing: there is no buffer assigned yet!
}

void WeatherLcdGraph::setBuffer(TimePermRingBuffer *buf)
{
  m_data = buf;
  // TODO: fix the hardcoded value (however we do not know about the display yet...
  m_graphX = 128 - m_data->bufferSize();
  findExtremas();
}

TimePermRingBuffer * WeatherLcdGraph::getBuffer()
{
  return m_data;
}

void WeatherLcdGraph::setLimits(int16_t min, int16_t max)
{
  m_minY = min;
  m_maxY = max;
  m_scale = (max-min)/(float)GRAPH_Y_PIXELS;
}

uint16_t WeatherLcdGraph::getMinPressure(long *timestamp)
{
  if ( 0 != timestamp ) *timestamp = m_minPressTime;
  return m_minPressure;
}

uint16_t WeatherLcdGraph::getMaxPressure(long *timestamp)
{
  if ( 0 != timestamp ) *timestamp = m_maxPressTime;
  return m_maxPressure;
}

void WeatherLcdGraph::findExtremas()
{
  uint16_t pressure;
  long time;
  WeatherSample sample;
  if ( m_data ) {
    m_minPressure = 12000;
    m_maxPressure = 0;
    for (uint16_t i=0; i<m_data->bufferSize(); i++) {
      time = m_data->read(i, sample);
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
}

#if 0
void WeatherLcdGraph::draw(ST7565 &lcd)
{
  uint16_t pressure, size;
  long time;
  uint8_t x, y, j;
  WeatherSample sample;
  if ( m_data ) {
    size = m_data->bufferSize();
    lcd.fillrect(m_graphX, m_graphY, m_graphX+size, m_graphY+GRAPH_Y_PIXELS, 0);
    for (uint16_t i=0; i<size; i++) {
      time = m_data->read(i, sample);
      pressure = sample.getPressure();
      if ( pressure != 0xFFFF && pressure > m_minY ) {
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
    // horizontal axis
    for (j=m_graphX; j<=LCDWIDTH; j++)
      lcd.setpixel(j, m_graphY+GRAPH_Y_PIXELS-1, 1);
    //vertical axis
    for (j=0; j<GRAPH_Y_PIXELS; j++)
      lcd.setpixel(m_graphX-1, m_graphY+j, 1);
    char str[8];
    utoa(m_maxY, str, 10);
    lcd.drawstring(0, GRAPH_LINE_OFFSET, str);
    utoa(m_minY, str, 10);
    lcd.drawstring(0, GRAPH_LINE_OFFSET+3, str);
  }
}
#endif

void WeatherLcdGraph::draw(U8GLIB  &lcd)
{
  uint16_t pressure, size;
  long time;
  u8g_uint_t x, y, j;
  WeatherSample sample;
  if ( m_data ) {
    size = m_data->bufferSize();
    //lcd.fillrect(m_graphX, m_graphY, m_graphX+size, m_graphY+GRAPH_Y_PIXELS, 0);
    lcd.setColorIndex(1);
    for (uint16_t i=0; i<size; i++) {
      time = m_data->read(i, sample);
      pressure = sample.getPressure();
      if ( pressure != 0xFFFF && pressure > m_minY ) {
        x = (uint8_t)(m_graphX+size-i-1);
        y = (uint8_t)((pressure-m_minY)/m_scale);
        if ( y > GRAPH_Y_PIXELS ) {
          y = GRAPH_Y_PIXELS;
        }
        lcd.drawVLine(x, m_graphY+GRAPH_Y_PIXELS-y, y);
      }
    }
    // horizontal axis
    lcd.drawHLine(m_graphX, m_graphY+GRAPH_Y_PIXELS, lcd.getWidth()-m_graphX);
    //vertical axis
    lcd.drawVLine(m_graphX-1, m_graphY, GRAPH_Y_PIXELS);

    // print Y scale
//    lcd.setFont(u8g_font_helvR08n);
    lcd.setFont(u8g_font_6x10);
    lcd.setPrintPos(0, 3*8+8);
    lcd.print(m_maxY);
    lcd.setPrintPos(0, 6*8+8);
    lcd.print(m_minY);
  }
}
