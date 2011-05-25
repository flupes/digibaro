#include "WeatherLcdGraph.h"
#include "WeatherSample.h"

#include "math.h"

#define START_ADDR 0
#define BUFFER_SZ 96
#define PERIOD 2

#define BACKLIGHT_LED 3
#define WAIT_LOOP 50

TimePermRingBuffer buffer(START_ADDR, BUFFER_SZ, sizeof(WeatherData), PERIOD);

WeatherLcdGraph graph(buffer, 3);

ST7565 glcd(9, 8, 7, 6, 5);

long counter = 200;

char timeStr[16];

void setup()
{
  graph.setLimits(900, 1100);
  Serial.begin(9600);

  // initialize GLCD
  glcd.begin(0x18);

  // turn on backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);

  // show splashscreen
  glcd.display(); 
  delay(100);
  glcd.clear();
}

void loop()
{  
  float y = 1000.0 + 100.0*sin(counter/20.0)*cos(counter/100.0);
  Serial.println(y, DEC);
  WeatherSample sample;
  sample.setPressure((uint16_t)y);
  buffer.insert(sample, counter);
  graph.draw(glcd);
  ltoa(counter, timeStr, 10);
  glcd.setpixel(0,0,1);
  glcd.setpixel(0,0,0);
  glcd.drawstring(8, 1, timeStr);
  glcd.display();
  counter++;
  delay(WAIT_LOOP);
}

int main(void)
{
  init();

  setup();

  for (;;)
    loop();

  return 0;
}
