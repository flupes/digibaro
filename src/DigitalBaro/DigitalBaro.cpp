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
TimePermRingBuffer buffer(0, 96, sizeof(WeatherData), 600);
WeatherLcdGraph graph(buffer, 3);

AnalogFiveButtons a5b(A2, 5.0);
uint16_t ladder[6] = { 4990, 22100, 9310, 4990, 2100, 1039 };

byte state = 0;

int counter;
unsigned long prevtime;
time_t currentTime;
time_t lastTime;

int squareSize = 12;
int squareSpacing = 24;
int startX = 6;
int startY = 40; 
int buttonsX[5];

char tempStr[24];
char pressureStr[24];
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

  for (int i=0; i<5; i++) {
    buttonsX[5-i-1] = startX+i*squareSpacing;
  }

  graph.setLimits(10100, 10200);

  counter = 0;
  prevtime = millis();

  Serial.print("buffer storage size = ");
  Serial.println(buffer.storageSize(), DEC);
  time_t startTime = now();
  tmElements_t tm;
  long bufferTimeStamp = buffer.lastTimeStamp();
  if ( bufferTimeStamp == 0xFFFFFFFFl || 
       startTime > bufferTimeStamp+buffer.period()*buffer.bufferSize() ) {
    breakTime(startTime, tm);
    tm.Second = 0;
    if ( buffer.period() > 3600 ) tm.Minute = 0;
    startTime = makeTime(tm);
    buffer.setTimeStamp((long)startTime);
  }

}

void loop()                     
{
  byte lastMinute = 127;
  byte currentMinute = 127;
  WeatherSample sample;

  //glcd.clear();

  a5b.update();

  if ( a5b.getState(AnalogFiveButtons::BM_5) && 
       a5b.getState(AnalogFiveButtons::BM_4) ) {
    digitalWrite(BACKLIGHT_LED, LOW);
  }

  if ( a5b.getState(AnalogFiveButtons::BM_5) && 
       a5b.getState(AnalogFiveButtons::BM_3) ) {
    digitalWrite(BACKLIGHT_LED, HIGH);
  }

  if ( a5b.buttonPressed(AnalogFiveButtons::BM_4) ) {
    graph.setLimits(graph.minY(), graph.maxY()+10);
    graph.draw(glcd);
    a5b.clearButton(AnalogFiveButtons::BM_4);
  }
  if ( a5b.buttonPressed(AnalogFiveButtons::BM_3) ) {
    graph.setLimits(graph.minY(), graph.maxY()-10);
    graph.draw(glcd);
    a5b.clearButton(AnalogFiveButtons::BM_3);
  }
  if ( a5b.buttonPressed(AnalogFiveButtons::BM_2) ) {
    graph.setLimits(graph.minY()+10, graph.maxY());
    graph.draw(glcd);
    a5b.clearButton(AnalogFiveButtons::BM_2);
  }
  if ( a5b.buttonPressed(AnalogFiveButtons::BM_1) ) {
    graph.setLimits(graph.minY()-10, graph.maxY());
    graph.draw(glcd);
    a5b.clearButton(AnalogFiveButtons::BM_1);
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
    glcd.drawstring(8, 0, timeStr);
    currentMinute = minute(now());
//    if ( lastMinute != currentMinute ) {
      baro.readData();
      sample.setPressure(baro.getPressureDeciPa());
      sample.setTemperature(baro.getTemperature());
      buffer.insert(sample, (long)currentTime);
      graph.draw(glcd);
      lastMinute = currentMinute; 
//    }
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
    baro.printTemperature(tempStr);
    glcd.drawstring(8, 1, tempStr);
    baro.printPressure(pressureStr);
    glcd.drawstring(8, 2, pressureStr);
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
