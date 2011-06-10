#include <Wire.h>

#include "ST7565.h"

#include "AnalogFiveButtons.h"
#include "Ds3231.h"
#include "Bmp085.h"
#include "WeatherLcdGraph.h"
#include "WeatherSample.h"

#define BACKLIGHT_LED 3
#define WAIT_LOOP 20

void setup();
void loop();
ST7565 glcd(9, 8, 7, 6, 5);
Ds3231 clock(104);
Bmp085 baro(0x77, 3);
TimePermRingBuffer dayBuffer(0, 96, sizeof(WeatherData), 600, 8);
TimePermRingBuffer weekBuffer(512, 96, sizeof(WeatherData), 3600, 1);
boolean extendedBuffer;

WeatherLcdGraph graph;

AnalogFiveButtons a5b(A2, 5.0);
uint16_t ladder[6] = { 4990, 22100, 9310, 4990, 2100, 1039 };
//uint16_t ladder[6] = { 5010, 22000, 9320, 5010, 2090, 1038 };

int temperatures[8];
long pressures[8];

byte state = 0;

int counter;
unsigned long prevtime;
time_t currentTime;
time_t lastTime;

int avgTemperature;
long avgPressure;

char statusStr[24];
char timeStr[24];
char elapsedStr[4];
char blank[]="    ";

boolean backlight;


void printTime(unsigned long t)
{
  tmElements_t tm;
  breakTime(t, tm);
#ifdef SERIAL_DEBUG
  Serial.print(t, DEC);
  Serial.print(" -> ");
  Serial.print(tm.Year, DEC); 
  Serial.print("/");
  Serial.print(tm.Month, DEC);
  Serial.print("/");
  Serial.print(tm.Day, DEC);
  Serial.print(" -- ");
  Serial.print(tm.Hour, DEC);
  Serial.print(":");
  Serial.print(tm.Minute, DEC);
  Serial.print(":");
  Serial.println(tm.Second, DEC);
#endif
}

void setup() {                

  // initialize and set the contrast to 0x18
  glcd.begin(0x18);

  // show splashscreen
  glcd.display(); 
  delay(500);

  // turn on backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);
  backlight = true;

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

//  Serial.print("day buffer storage size = ");
//  Serial.println(dayBuffer.storageSize(), DEC);
//  Serial.print("week buffer storage size = ");
//  Serial.println(weekBuffer.storageSize(), DEC);

  delay(3000);
  glcd.clear(); 

  setSyncProvider(Ds3231::getTimeSync);
  
  time_t startTime = clock.getRtcTime();
  tmElements_t tm;
  setTime(startTime);
#ifdef SERIAL_DEBUG
  Serial.print("start Time = ");
  printTime(startTime);
#endif

  counter = 0;
  prevtime = millis();
  unsigned long bufferTimeStamp;
  TimePermRingBuffer *buff;

  for (byte i=0; i<2; i++) {

    /* 
       Generate a correct start time (round) if
       brand new buffer or historic data too old
       for current buffers.
    */
    if ( 1 == i ) { 
      buff = &dayBuffer;
#ifdef SERIAL_DEBUG
      Serial.print("last DAY");
#endif
    }
    else {
      buff = &weekBuffer;
#ifdef SERIAL_DEBUG
      Serial.print("last WEEK");
#endif
    }
    bufferTimeStamp = buff->lastTimeStamp();
#ifdef SERIAL_DEBUG
    Serial.print(" buffer time stamp = ");
    printTime(bufferTimeStamp);
#endif

    if ( bufferTimeStamp == 0xFFFFFFFFul || 
         startTime > buff->timeSpan() ) {
      // Reset the time to something rounded
      // this trivial method will work for buffer that
      // are least 1h long (more likely for this application)
      breakTime(startTime, tm);
      tm.Second = 0;
      tm.Minute = 0;
      startTime = makeTime(tm);
      buff->setTimeStamp(startTime);
#ifdef SERIAL_DEBUG
      Serial.print("Reset buffer with start time = ");
      printTime(startTime);
#endif
            
    }
  }

  /*
    Get some reasonable reading of the current pressure
  */
  for (int i=0; i<8; i++) {
    baro.readData();
    temperatures[i] = baro.getTemperature();
    pressures[i] = baro.getPressure();
    delay (50);
  }

  /* 
     Set the graph limits: only for the startup...
     After we should keep whatever the user choose
     We only search the week buffer: it is really unlikely
     that the day buffer present a peek that
     was not captured in the week buffer...

     The limits are rounded to 2hPa below and above the min/max
  */
  graph.setBuffer(&weekBuffer);
  bufferTimeStamp = weekBuffer.lastTimeStamp();
  uint16_t min = graph.getMinPressure();
  uint16_t max = graph.getMaxPressure();
#ifdef SERIAL_DEBUG
  Serial.print("week buffer last timestamp=");
  Serial.println(bufferTimeStamp, DEC);
  Serial.print("current time=");
  Serial.println(startTime, DEC);
  Serial.print("min=");
  Serial.println(min, DEC);
  Serial.print("max=");
  Serial.println(max, DEC);
#endif
  if ( bufferTimeStamp == 0xFFFFFFFFul || 
       startTime > bufferTimeStamp+weekBuffer.timeSpan() ||
       (max-min) < 60 ) {
#ifdef SERIAL_DEBUG
    Serial.println( bufferTimeStamp == 0xFFFFFFFFul ? "cond1" : "!cond1");
    Serial.println( startTime > bufferTimeStamp+weekBuffer.timeSpan() ? "cond2" : "!cond2");
    Serial.println( (max-min) < 60 ? "cond3" : "!cond3");
#endif
    // Scenario when there is no historical data available,
    // or if too few samples are available in the buffer 
    // (the range will be really small):
    // use limits centered on the current pressure
    avgPressure = 0;
    for (int i=0; i<8; i++ ) { 
      avgPressure += pressures[i];      
    }
    avgPressure = avgPressure / 80;
    min = avgPressure - 30;
    max = avgPressure + 20;
  }
  min = (min-4)/20; min = min*20;
  max = (max+16)/20; max = max*20;
  graph.setLimits(min, max);
  /* Now start with the day buffer */
  graph.setBuffer(&dayBuffer);
  extendedBuffer = false;

  graph.draw(glcd);

}

void loop()                     
{
  static byte currentMinute = 0;
  static byte lastMinute = 0;
  static byte avgIndex = 0;
  WeatherSample sample;
  //glcd.clear();

  a5b.update();

  if ( a5b.getState(AnalogFiveButtons::BM_5) ) {
    // First test two button press: BM_5 + BM_3 or BM_4

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
      // Two buttons: BM_2 + BM1
      if ( extendedBuffer ) {
        graph.setBuffer(&dayBuffer);
        graph.draw(glcd);
        extendedBuffer = false;
      }
      else {
        graph.setBuffer(&weekBuffer);
        graph.draw(glcd);
        extendedBuffer = true;
      }
      a5b.clearButton(AnalogFiveButtons::BM_2);
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

  glcd.drawstring(8, 4, blank);
  glcd.drawstring(8, 4, elapsedStr);

  boolean rd = false;
  boolean rw = false;

  currentTime = now();
  //Serial.print("current time = ");
  //Serial.println(currentTime, DEC);
  if ( currentTime != lastTime ) {
    clock.readData();
    clock.printTime(timeStr);
    glcd.drawstring(0, 0, timeStr);
    currentMinute = minute(now());
//    if ( lastMinute != currentMinute ) {
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
      rd = dayBuffer.insert(sample, (long)currentTime);
      rw = weekBuffer.insert(sample, (long)currentTime);
#ifdef SERIAL_DEBUG
      if ( rw ) {
        Serial.print("insert value = ");
        Serial.print(sample.getPressure(), DEC);
        Serial.print(" into week buffer with timestamp = ");
        printTime(currentTime);
      }
#endif
      if ( rd || rw ) 
        graph.draw(glcd);
      lastMinute = currentMinute; 
//    }
    lastTime = currentTime;
  }
  
  if (counter == 20) {
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
