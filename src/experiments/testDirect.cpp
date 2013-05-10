#include <Arduino.h>

class Direct {
public:
  Direct() : m_val(0) { };
  long value() { return m_val; }
  void compute(int i);
protected:
  long m_val;
};

int main(void)
{
  Direct d;
  Direct &r = d;
  unsigned long ts, te;
  const int iter = 25000;
  const int loop = 40;
  
  init();
  
  Serial.begin(9600);

  Serial.print("Start test DIRECT  with ");
  Serial.print((long)iter*(long)loop, DEC);
  Serial.println(" iterations...");
  ts = millis();
  int m;
  for (int k=0; k<loop; k++) {
    if ( k%2 == 0 ) m=1; else m=-1;
    for (int i=0; i<iter; i++) {
      r.compute(i*m);
    }
  }
  te = millis();

  Serial.print("... test completed in ");
  Serial.print(te-ts, DEC);
  Serial.println("ms.");
  Serial.print("Result = ");
  Serial.println(r.value(), DEC);

  return 0;
}

// make sure to compile with -fno-inline-small-functions
// to avoid inlining (in which case the test takes 0ms!
void Direct::compute(int i) {
  m_val += i;
}

/*

SIZE:
Creating eeprom binary for testDirect ...
AVR Memory Usage
----------------
Device: atmega328p

Program:    2366 bytes (7.2% Full)
(.text + .data + .bootloader)

Data:        256 bytes (12.5% Full)
(.data + .bss + .noinit)


OUTPUT:

Start test DIRECT  with 1000000 iterations...
... test completed in 2892ms.
Result = 0

*/
