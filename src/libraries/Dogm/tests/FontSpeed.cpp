#include <Dogm.h>

#include "WProgram.h"
void setup();
void loop();

int a0Pin = 7;      // address line a0 for the dogm module

Dogm dogm(a0Pin);

void setup() {
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  dogm.start();
}


void loop() {
  DOG_PGM_P p;
  unsigned char h;
  char text[4];

  static unsigned long prevtime = 0;
  static unsigned long curtime = 0;
  h = 12;

  curtime = millis();
  int elapsed = curtime-prevtime;
  prevtime = curtime;
  itoa(elapsed, text, 10);
//for (int i = 0; i<3; i++ )
//    dogm.next();
//  dog_set_page(3);
  do {
    p = font_04B_25;
    dogm.setFont(p);
    dogm.setXY(0,62-h);
    dogm.print(" : 0123456789 .");
    p = font_atomicsc;
    dogm.setFont(p);
    dogm.setXY(100,62-h);
    dogm.print(text);
//    dog_transfer_page();
  } while( dogm.next() );
}

int main(void)
{
  init();
  
  setup();
  
  for (;;)
    loop();
  
  return 0;
}
