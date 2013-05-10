#include "Bmp085.h"

#include <string.h>
#include <stdlib.h>
#include <Time.h>
#include <Arduino.h>
#include <Wire.h>

Bmp085::Bmp085(int address, unsigned char overSampling) :
  m_sensorAddress(address), m_oss(overSampling)
{
  if ( overSampling > 3 ) {
    exit(-1);
  }
}
  
void Bmp085::readData()
{
  m_rawTemperature = readRawTemperature();
  m_rawPressure = readRawPressure();
}

int Bmp085::getTemperature()
{
  long x1, x2;
  
  x1 = (((long)m_rawTemperature - (long)m_ac6)*(long)m_ac5) >> 15;
  x2 = ((long)m_mc << 11)/(x1 + m_md);
  m_b5 = x1 + x2;

  m_calibratedTemperature = (m_b5 + 8) >> 4;

  return m_calibratedTemperature;  
}
  
long Bmp085::getPressure()
{
  long x1, x2, x3, b3, b6, b8;
  unsigned long b4, b7;
  
  b6 = m_b5 - 4000;
  b8 = (b6 * b6)>>12;
  // Calculate B3
  x1 = (m_b2 * b8)>>11;
  x2 = (m_ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)m_ac1)*4 + x3)<<m_oss) + 2)>>2;
  
  // Calculate B4
  x1 = (m_ac3 * b6)>>13;
  x2 = (m_b1 * b8)>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (m_ac4 * (unsigned long)(x3 + 32768))>>15;
  
  b7 = ((unsigned long)(m_rawPressure - b3) * (50000>>m_oss));
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

unsigned int Bmp085::getPressureDeciPa()
{
  return (m_calibratedPressure/2+2)/5;
}

void Bmp085::calibrate() 
{
  m_ac1 = readInt(0xAA);
  m_ac2 = readInt(0xAC);
  m_ac3 = readInt(0xAE);
  m_ac4 = readInt(0xB0);
  m_ac5 = readInt(0xB2);
  m_ac6 = readInt(0xB4);
  m_b1 = readInt(0xB6);
  m_b2 = readInt(0xB8);
  m_mb = readInt(0xBA);
  m_mc = readInt(0xBC);
  m_md = readInt(0xBE);
}
  
unsigned int Bmp085::readRawTemperature()
{
  unsigned int ut;
  
  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(m_sensorAddress);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  
  // Wait at least 4.5ms
  delay(5);
  
  // Read two bytes from registers 0xF6 and 0xF7
  ut = readInt(0xF6);
  return ut;
}
  
unsigned long Bmp085::readRawPressure()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;
  
  // Write 0x34+(m_oss<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(m_sensorAddress);
  Wire.write(0xF4);
  Wire.write(0x34 + (m_oss<<6));
  Wire.endTransmission();
  
  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<m_oss));
  
  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(m_sensorAddress);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(m_sensorAddress, 3);
  
  // Wait for data to become available
  while(Wire.available() < 3)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();
  
  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-m_oss);
  
  return up;
}

int Bmp085::readInt(unsigned char address)
{
  unsigned char msb, lsb;
  
  Wire.beginTransmission(m_sensorAddress);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(m_sensorAddress, 2);
  while(Wire.available()<2)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  
  return (int) msb<<8 | lsb;
}


void Bmp085::printTemperature(char str[])
{
  int temperature = getTemperature();
  itoa(temperature, str, 10);
  memcpy(str+3, str+2, 1);
  memcpy(str+2, ".", 1);
  strcpy(str+4, "\x5E");
  strcpy(str+5, "C");
}

void Bmp085::printPressure(char str[])
{
  int pressure = getPressureDeciPa();
  itoa(pressure, str, 10);
  memcpy(str+5, str+4, 1);
  memcpy(str+4, ".", 1);
  strcpy(str+6, "Pa");
}

