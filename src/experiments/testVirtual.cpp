#include <WProgram.h>

class Base {
public:
  Base() : m_val(0) { };
  long value() { return m_val; }
  virtual void compute(int i) = 0;
protected:
  long m_val;
};

class Derived : public Base {
public:
  void compute(int i);
};

int main(void)
{
  Derived d;
  Base &b = d;
  unsigned long ts, te;
  const int iter = 25000;
  const int loop = 40;
  
  init();
  
  Serial.begin(9600);

  Serial.print("Start test VIRTUAL with ");
  Serial.print((long)iter*(long)loop, DEC);
  Serial.println(" iterations...");
  ts = millis();
  int m;
  for (int k=0; k<loop; k++) {
    if ( k%2 == 0 ) m=1; else m=-1;
    for (int i=0; i<iter; i++) {
      b.compute(i*m);
    }
  }
  te = millis();

  Serial.print("... test completed in ");
  Serial.print(te-ts, DEC);
  Serial.println("ms.");
  Serial.print("Result = ");
  Serial.println(b.value(), DEC);

  return 0;
}

void Derived::compute(int i) {
  m_val += i;
}

/*

SIZE:
Creating eeprom binary for testVirtual ...
AVR Memory Usage
----------------
Device: atmega328p

Program:    2394 bytes (7.3% Full)
(.text + .data + .bootloader)

Data:        262 bytes (12.8% Full)
(.data + .bss + .noinit)


OUTPUT:

Start test VIRTUAL with 1000000 iterations...
... test completed in 3395ms.
Result = 0

*/
