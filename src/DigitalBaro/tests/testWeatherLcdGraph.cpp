#include <Arduino.h>

#include <math.h>

#include "WeatherLcdGraph.h"
#include "WeatherSample.h"


#define START_ADDR 0
#define BUFFER_SZ 96
#define PERIOD 2

#define BACKLIGHT_LED 3
#define WAIT_LOOP 100

TimePermRingBuffer buffer(START_ADDR, BUFFER_SZ, sizeof(WeatherData), PERIOD);

WeatherLcdGraph graph;

//ST7565 glcd(11, 13, 9, 6, 10);
//ST7565 glcd(9, 8, 7, 6, 5);
//ST7565 glcd(11, 13, 8, 7, 14);

U8GLIB_LM6059_2X glcd(14, 8, 7);

int interval = 10;
unsigned long counter = 100;
unsigned long time, prev_time;

int period = 0;

void setup()
{
  graph.setLimits(900, 1100);
  graph.setBuffer(&buffer);
  Serial.begin(115200);

  // turn on backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);

  time = millis();
  prev_time = time;
}

void draw() {
  graph.draw(glcd);

  glcd.setPrintPos(8, 16);
  glcd.print(counter);
  //Serial.println(counter, DEC);

  //Serial.println(period, DEC);
  glcd.setPrintPos(64, 16);
  glcd.print(period);

}

void loop()
{  
  float y = 1000.0 + 100.0*sin(counter/20.0)*cos(counter/100.0);
  //Serial.println(counter, DEC);
  //Serial.println(y, DEC);
  WeatherSample sample;
  sample.setPressure((uint16_t)y);
  buffer.insert(sample, counter);

  if ( (counter % interval) == 0 ) {
    time = millis();
    period = (int)(time-prev_time)/interval;
    prev_time = time;
  }

  glcd.firstPage();  
  do {
    draw();
  } 
  while( glcd.nextPage() );

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
