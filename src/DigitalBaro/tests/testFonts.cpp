#include <Wire.h>

#include "ST7565.h"

#define BACKLIGHT_LED 3

ST7565 glcd(9, 8, 7, 6, 5);

int main(void)
{
  init();
  
  pinMode(BACKLIGHT_LED, OUTPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);

  glcd.begin(0x18);
  glcd.clear(); 
  for (uint8_t i=0; i < 168; i++) {
    glcd.drawchar((i % 21) * 6, i/21, i+87);
  }    
  glcd.display();

  for (;;)
  
  return 0;
}
