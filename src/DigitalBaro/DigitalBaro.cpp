#include <Wire.h>

#include "ST7565.h"

#include "AnalogFiveButtons.h"
#include "Ds3231.h"
#include "Bmp085.h"
#include "WeatherLcdGraph.h"
#include "WeatherSample.h"

#define BACKLIGHT_LED 3
#define WAIT_LOOP 20

//#include "WProgram.h"
void setup();
void loop();
ST7565 glcd(9, 8, 7, 6, 5);
Ds3231 clock(104);
Bmp085 baro(0x77, 3);
TimePermRingBuffer dayBuffer(0, 96, sizeof(WeatherData), 600, 8);
TimePermRingBuffer weekBuffer(512, 96, sizeof(WeatherData), 5400, 1);
boolean extendedBuffer;

WeatherLcdGraph graph;

AnalogFiveButtons a5b(A2, 5.0);
uint16_t ladder[6] = { 4990, 22100, 9310, 4990, 2100, 1039 };

int temperatures[8];
long pressures[8];

byte state = 0;

int counter;
unsigned long prevtime;
time_t currentTime;
time_t lastTime;

char statusStr[24];
char timeStr[24];
char elapsedStr[4];
char blank[]="    ";

boolean backlight;

void setup() {                

  // initialize and set the contrast to 0x18
  glcd.begin(0x18);

  // show splashscreen
  glcd.display(); 
  delay(500);
  glcd.clear(); 

  // turn on backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);
  backlight = true;

  setSyncProvider(Ds3231::getTimeSync);

  // For I2C
  Wire.begin();

  baro.calibrate();

  // For console debugging
  Serial.begin(9600);

  // Configure the ladder
  a5b.setLadder(3.3f, ladder);
  a5b.removeState(17);
  a5b.removeState(18);
  a5b.setTiming(20, 3);

  graph.setLimits(10100, 10200);
  graph.setBuffer(&dayBuffer);
  extendedBuffer = false;

  counter = 0;
  prevtime = millis();

//  Serial.print("day buffer storage size = ");
//  Serial.println(dayBuffer.storageSize(), DEC);
//  Serial.print("week buffer storage size = ");
//  Serial.println(weekBuffer.storageSize(), DEC);

//  delay(6000);

  time_t startTime = now();
  Serial.print("start Time = ");
  Serial.println(startTime, DEC);
  tmElements_t tm;
  unsigned long bufferTimeStamp = dayBuffer.lastTimeStamp();
  if ( bufferTimeStamp == 0xFFFFFFFFul || 
       startTime > bufferTimeStamp+dayBuffer.timeSpan() ) {
    breakTime(startTime, tm);
    Serial.println(tm.Day, DEC);
    Serial.println(tm.Hour, DEC);
    Serial.println(tm.Minute, DEC);
    Serial.println(tm.Second, DEC);
    tm.Second = 0;
    tm.Minute = 0;
    byte hourOffset = dayBuffer.timeSpan()/3600;
    if ( hourOffset > 0 ) {
      if ( tm.Hour < hourOffset ) {
        tm.Day -= 1;
        tm.Hour += 24-hourOffset;
      }
      else {
        tm.Hour -= hourOffset;
      }
      // if ( dayBuffer.timeSpan() % 3600 != 0 ) 
      //   tm.Hour += 1;
    }
    startTime = makeTime(tm);
    dayBuffer.setTimeStamp(startTime);
    weekBuffer.setTimeStamp(startTime);
    Serial.print("set buffer start time = ");
    Serial.println(startTime, DEC);
    Serial.println(tm.Day, DEC);
    Serial.println(tm.Hour, DEC);
    Serial.println(tm.Minute, DEC);
    Serial.println(tm.Second, DEC);
  }

  for (int i=0; i<8; i++) {
    baro.readData();
    temperatures[i] = baro.getTemperature();
    pressures[i] = baro.getPressure();
    delay (25);
  }

}

void loop()                     
{
  static byte currentMinute = 0;
  static byte lastMinute = 0;
  static byte avgIndex = 0;
  WeatherSample sample;
  int avgTemperature;
  long avgPressure;
  //glcd.clear();

  a5b.update();

  if ( a5b.getState(AnalogFiveButtons::BM_5) ) {

    // First test two button press

    if ( a5b.buttonPressed(AnalogFiveButtons::BM_4) ) {
      digitalWrite(BACKLIGHT_LED, LOW);
      a5b.clearButton(AnalogFiveButtons::BM_5 | AnalogFiveButtons::BM_4);
    }

    if  ( a5b.buttonPressed(AnalogFiveButtons::BM_3) ) {
      digitalWrite(BACKLIGHT_LED, HIGH);
      a5b.clearButton(AnalogFiveButtons::BM_5 | AnalogFiveButtons::BM_3);
    } 

  }
  else {
    if  ( a5b.getState(AnalogFiveButtons::BM_2) 
          && a5b.buttonPressed(AnalogFiveButtons::BM_1) ) {
      if ( extendedBuffer ) {
        graph.setBuffer(&dayBuffer);
        extendedBuffer = false;
      }
      else {
        graph.setBuffer(&weekBuffer);
        extendedBuffer = true;
      }
      a5b.clearButton(AnalogFiveButtons::BM_1);
    }
 
    else {

      // Then single button press
      switch ( a5b.getPressedState() ) {
        case 8:
          graph.setLimits(graph.minY(), graph.maxY()+10);
          graph.draw(glcd);
          a5b.clearButton(AnalogFiveButtons::BM_4);
          break;
        case 4:
          graph.setLimits(graph.minY(), graph.maxY()-10);
          graph.draw(glcd);
          a5b.clearButton(AnalogFiveButtons::BM_3);
          break;
        case 2:
          graph.setLimits(graph.minY()+10, graph.maxY());
          graph.draw(glcd);
          a5b.clearButton(AnalogFiveButtons::BM_2);
          break;
        case 1:
          graph.setLimits(graph.minY()-10, graph.maxY());
          graph.draw(glcd);
          a5b.clearButton(AnalogFiveButtons::BM_1);
          break;
      }
    }
  }


  unsigned long newtime = millis();
  int elapsed = newtime-prevtime;
  String str(elapsed, DEC);
  str.toCharArray(elapsedStr, 8);
  prevtime = newtime;

  glcd.drawstring(8, 7, blank);
  glcd.drawstring(8, 7, elapsedStr);

  currentTime = now();
  //Serial.print("current time = ");
  //Serial.println(currentTime, DEC);
  if ( currentTime != lastTime ) {
    clock.readData();
    clock.printTime(timeStr);
    glcd.drawstring(0, 0, timeStr);
    currentMinute = minute(now());
    if ( lastMinute != currentMinute ) {
      baro.readData();
      temperatures[avgIndex] = baro.getTemperature();
      pressures[avgIndex] = baro.getPressure();
      avgIndex++;
      if ( avgIndex == 8 ) avgIndex=0;
      avgPressure = 0;
      avgTemperature = 0;
      for (int i=0; i<8; i++ ) { 
        avgPressure += pressures[i];      
        avgTemperature += temperatures[i];
      }
      avgPressure = avgPressure/80;
      avgTemperature = avgTemperature/8;
      sample.setPressure(avgPressure);
      sample.setTemperature(avgTemperature);
      dayBuffer.insert(sample, (long)currentTime);
      weekBuffer.insert(sample, (long)currentTime);
      graph.draw(glcd);
      lastMinute = currentMinute; 
    }
    lastTime = currentTime;
  }
  
  if (counter == 50) {
    /** @warning
        This is something to debug:
        if no "graphic" call to the glcd is made, then
        the first letter of the temperature and pressure
        is missing (no P, not T) !!!
    */
    glcd.setpixel(0,0,1);
    glcd.setpixel(0,0,0);
    baro.readData();
    baro.printPressure(statusStr);
    strcpy(statusStr+8, " /  ");
    baro.printTemperature(statusStr+11);
    strcpy(statusStr+17, " / ");
    if ( graph.getBuffer() == &dayBuffer ) 
      strcpy(statusStr+20, "D");
    else 
      strcpy(statusStr+20, "W");
    glcd.drawstring(0, 1, statusStr);
    counter = 0;    
  }

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
