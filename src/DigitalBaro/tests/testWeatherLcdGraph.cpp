#include "WeatherLcdGraph.h"
#include "WeatherSample.h"

#include "math.h"

#define START_ADDR 0
#define BUFFER_SZ 96
#define PERIOD 2

#define BACKLIGHT_LED 3
#define WAIT_LOOP 100

#define USE_CLEAR 1

TimePermRingBuffer buffer(START_ADDR, BUFFER_SZ, sizeof(WeatherData), PERIOD);

WeatherLcdGraph graph;

//ST7565 glcd(11, 13, 9, 6, 10);
//ST7565 glcd(9, 8, 7, 6, 5);
ST7565 glcd(11, 13, 8, 7, 14);

int interval = 10;
unsigned long counter = 100;
unsigned long time, prev_time;

char timeStr[34];
char periodStr[18];

void setup()
{
  graph.setLimits(900, 1100);
  graph.setBuffer(&buffer);
  Serial.begin(115200);

  // initialize GLCD
  glcd.begin(0x18);

  // turn on backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);

  // show splashscreen
  glcd.display(); 
  delay(100);
  glcd.clear();

  time = millis();
  prev_time = time;
}

void loop()
{  
  float y = 1000.0 + 100.0*sin(counter/20.0)*cos(counter/100.0);
  //Serial.println(counter, DEC);
  //Serial.println(y, DEC);
  WeatherSample sample;
  sample.setPressure((uint16_t)y);
  buffer.insert(sample, counter);

#ifdef USE_CLEAR
  glcd.clear();
#else
  glcd.fillrect(0, 8, 128, 8, 0);
#endif
  
  graph.draw(glcd);
  
  glcd.drawstring(8, 1, timeStr);
  if ( (counter % interval) == 0 ) {
    time = millis();
    itoa( (int)(time-prev_time)/interval, periodStr, 10);
    //Serial.println(periodStr);
    prev_time = time;
  }
  ultoa(counter, timeStr, 10);
  //Serial.println(timeStr);
  glcd.drawstring(64, 1, periodStr);

  glcd.display();
  counter++;
  //delay(WAIT_LOOP);
}

int main(void)
{
  init();

  setup();

  for (;;)
    loop();

  return 0;
}
