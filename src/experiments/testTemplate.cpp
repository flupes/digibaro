#include <WProgram.h>

#define LED_PIN 13

template<int n>
struct Factorial
{ enum {RET=Factorial<n-1>::RET*n}; };

template<>
struct Factorial<0>
{ enum{RET=1}; };

void blinkNumber(int num)
{
  int i;
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  delay(500);
  for (i=0; i<16; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(60);
    digitalWrite(LED_PIN, LOW);
    delay(60);
  }
  delay(1000);
  for (i=0; i<num; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
  delay(500);
}

int main()
{
  init();

  blinkNumber(Factorial<3>::RET);
  delay(1000);
  blinkNumber(Factorial<4>::RET);
  delay(1000);
}
