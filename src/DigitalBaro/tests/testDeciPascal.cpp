#include "Bmp085.h"

#include "Arduino.h"

class TestPressure : public Bmp085
{
public:
  TestPressure() : Bmp085(0x77, 3) { }

  // Fake function to see how getPressureDeciPa performs
  void setPressure(long p) {
    m_calibratedPressure = p;
  }

};

TestPressure tester;

void test(long p)
{
  Serial.print(p, DEC);
  Serial.print(" Pa  ->  ");
  tester.setPressure(p);
  Serial.print(tester.getPressureDeciPa(), DEC);
  Serial.println(" dPa");
  delay(10);
}

int main(void)
{
  init();
  
  Serial.begin(9600);

  delay(6000);

  long p;
  for (p=30000; p<=30100; p+=5) {
    test(p);
  }
  Serial.println("");
  delay(1000);

  for (p=109900; p<=110000; p+=5) {
    test(p);
  }
  Serial.println("");
  delay(1000);

  for (p=102000; p<=102100; p+=1) {
    test(p);
  }

  return 0;
}
