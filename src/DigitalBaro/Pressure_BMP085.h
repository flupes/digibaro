
class Pressure_BMP085
{
  
public:
  Pressure_BMP085(int address, unsigned char overSampling);
  
  void readData();

  int getTemperature();
  
  long getPressure();

  unsigned int getPressureDeciPa();

  void printTemperature(char str[]);

  void printPressure(char str[]);

  void calibrate();

protected:
  
  unsigned int readRawTemperature();
  
  unsigned long readRawPressure();
  
  int readInt(unsigned char address);
  
  unsigned long m_rawPressure;
  long m_calibratedPressure;
  unsigned int m_rawTemperature;
  int m_calibratedTemperature;

  long b5; 
  int m_sensorAddress;

// Calibration values
  int ac1;
  int ac2; 
  int ac3; 
  int b1; 
  int b2;
  int mb;
  int mc;
  int md;
  unsigned int ac4;
  unsigned int ac5;
  unsigned int ac6;
  
  unsigned char OSS;
  
};
