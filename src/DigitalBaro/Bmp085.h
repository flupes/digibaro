#ifndef Bmp085_h
#define Bmp085_h

class Bmp085
{
  
public:
  Bmp085(int address, unsigned char overSampling);
  
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

  int m_sensorAddress;

  // Calibration values
  int m_ac1;
  int m_ac2; 
  int m_ac3; 
  unsigned int m_ac4;
  unsigned int m_ac5;
  unsigned int m_ac6;
  int m_b1; 
  int m_b2;
  int m_mb;
  int m_mc;
  int m_md;

  // Derived calibration value from temperature
  long m_b5; 

  // Oversampling setting
  unsigned char m_oss;
  
};

#endif
