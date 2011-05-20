#include "Pressure_BMP085.h"

#include <string.h>
#include <stdlib.h>
#include <Wire.h>
#include <wiring.h>

Pressure_BMP085::Pressure_BMP085(int address, unsigned char overSampling) :
  m_sensorAddress(address), OSS(overSampling)
{
  if ( overSampling > 3 ) {
    exit(-1);
  }
}
  
void Pressure_BMP085::readData()
{
  m_rawTemperature = readRawTemperature();
  m_rawPressure = readRawPressure();
}

int Pressure_BMP085::getTemperature()
{
  long x1, x2;
  
  x1 = (((long)m_rawTemperature - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  m_calibratedTemperature = (b5 + 8) >> 4;

  return m_calibratedTemperature;  
}
  
long Pressure_BMP085::getPressure()
{
  long x1, x2, x3, b3, b6;
  unsigned long b4, b7;
  
  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;
  
  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;
  
  b7 = ((unsigned long)(m_rawPressure - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    m_calibratedPressure = (b7<<1)/b4;
  else
    m_calibratedPressure = (b7/b4)<<1;
    
  x1 = (m_calibratedPressure>>8) * (m_calibratedPressure>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * m_calibratedPressure)>>16;
  m_calibratedPressure += (x1 + x2 + 3791)>>4;
  
  return m_calibratedPressure;
}

unsigned int Pressure_BMP085::getPressureDeciPa()
{
  return (m_calibratedPressure/2+2)/5;
}

void Pressure_BMP085::calibrate() 
{
  ac1 = readInt(0xAA);
  ac2 = readInt(0xAC);
  ac3 = readInt(0xAE);
  ac4 = readInt(0xB0);
  ac5 = readInt(0xB2);
  ac6 = readInt(0xB4);
  b1 = readInt(0xB6);
  b2 = readInt(0xB8);
  mb = readInt(0xBA);
  mc = readInt(0xBC);
  md = readInt(0xBE);
}
  
unsigned int Pressure_BMP085::readRawTemperature()
{
  unsigned int ut;
  
  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(m_sensorAddress);
  Wire.send(0xF4);
  Wire.send(0x2E);
  Wire.endTransmission();
  
  // Wait at least 4.5ms
  delay(5);
  
  // Read two bytes from registers 0xF6 and 0xF7
  ut = readInt(0xF6);
  return ut;
}
  
unsigned long Pressure_BMP085::readRawPressure()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;
  
  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(m_sensorAddress);
  Wire.send(0xF4);
  Wire.send(0x34 + (OSS<<6));
  Wire.endTransmission();
  
  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<OSS));
  
  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(m_sensorAddress);
  Wire.send(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(m_sensorAddress, 3);
  
  // Wait for data to become available
  while(Wire.available() < 3)
    ;
  msb = Wire.receive();
  lsb = Wire.receive();
  xlsb = Wire.receive();
  
  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);
  
  return up;
}

int Pressure_BMP085::readInt(unsigned char address)
{
  unsigned char msb, lsb;
  
  Wire.beginTransmission(m_sensorAddress);
  Wire.send(address);
  Wire.endTransmission();
  
  Wire.requestFrom(m_sensorAddress, 2);
  while(Wire.available()<2)
    ;
  msb = Wire.receive();
  lsb = Wire.receive();
  
  return (int) msb<<8 | lsb;
}


void Pressure_BMP085::printTemperature(char str[])
{
  int temperature = (int)getTemperature();
  strcpy(str, "T: ");
  itoa(temperature, str+3, 10);
  strcpy(str+6, " * 0.1 deg C");
}

void Pressure_BMP085::printPressure(char str[])
{
  long pressure = getPressure();
  strcpy(str, "P: ");
  ltoa(pressure, str+3, 10);
  strcpy(str+9, " Pa");
}

