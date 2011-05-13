
class Pressure_BMP085
{
  
public:
  Pressure_BMP085(int address, unsigned char overSampling);
  
  void readData();

  short getTemperature();
  
  long getPressure();

  void printTemperature(char str[]);

  void printPressure(char str[]);

  void calibrate();

protected:
  
  unsigned int readRawTemperature();
  
  unsigned long readRawPressure();
  
  int readInt(unsigned char address);

// Calibration values
  int ac1;
  int ac2; 
  int ac3; 
  unsigned int ac4;
  unsigned int ac5;
  unsigned int ac6;
  int b1; 
  int b2;
  int mb;
  int mc;
  int md;
  
// b5 is calculated in bmp085GetTemperature(...), this variable is also used in bmp085GetPressure(...)
// so ...Temperature(...) must be called before ...Pressure(...).
  long b5; 

  unsigned int rawTemperature;
  unsigned long rawPressure;
  
  int m_sensorAddress;

  unsigned char OSS;
  
};
