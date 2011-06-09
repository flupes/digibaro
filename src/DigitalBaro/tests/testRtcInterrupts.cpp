#include <Wire.h>

#include "Ds3231.h"

int main(void)
{
  init();

  delay(500);
  
  Ds3231 clock(104);

  clock.enableSquareWave(Ds3231::SQW_1kHz);

  delay(500);

  return 0;
}
