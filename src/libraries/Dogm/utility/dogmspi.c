/*

  dogmspi.c
  
  (c) 2010 Oliver Kraus (olikraus@gmail.com)
  
  spi abstraction layer
  
  This file is part of the dogm128 library.

  The dogm128 library is free software: you can redistribute it and/or modify
  it under the terms of the Lesser GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  The dogm128 library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  Lesser GNU General Public License for more details.

  You should have received a copy of the Lesser GNU General Public License
  along with dogm128.  If not, see <http://www.gnu.org/licenses/>.


  ST7565R SPI interface
  
    ST7565R reads data with rising edge of the clock signal (SCL)
      --> CPHA = 0 and CPOL = 0
    ST7565R reads  MSB first 
      --> DORD = 0
      
  /usr/lib/avr/include/avr/pgmspace.h
  
  
  This file is controlled by the following defines:
  
  DOG_SPI_USI
    --> ATTINY 
  DOG_SPI_ATMEGA
    --> ATMEGA
  DOG_SPI_ARDUINO or "nothing defined"
    --> Arduino


*/

#include "dogm128.h"

#if defined(DOG_SPI_USI)
#elif defined(DOG_SPI_ATMEGA)
#elif defined(__18CXX)
#else
#define DOG_SPI_ARDUINO
#endif


#if defined(DOG_SPI_USI)

/*=======================================================================*/
/* USI Interface */
/*=======================================================================*/

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define _DOG_SPI_DDR(PCHAR) DDR ## PCHAR
#define DOG_SPI_DDR(PCHAR) _DOG_SPI_DDR(PCHAR)
#define _DOG_SPI_PORT(PCHAR) PORT ## PCHAR
#define DOG_SPI_PORT(PCHAR) _DOG_SPI_PORT(PCHAR)

/* dog_spi_pin_a0 is ignored, instead, port and pins are hard coded */
#ifndef DOG_SPI_SCL_PORT
#define DOG_SPI_SCL_PORT A
#warning DOG_SPI_SCL_PORT not defined, port A assumed
#endif
#ifndef DOG_SPI_SCL_PIN
#define DOG_SPI_SCL_PIN 4
#warning DOG_SPI_SCL_PIN not defined, bit 4 assumed
#endif


#ifndef DOG_SPI_DO_PORT
#define DOG_SPI_DO_PORT A
#warning DOG_SPI_DO_PORT not defined, port A assumed
#endif 

#ifndef DOG_SPI_DO_PIN
#define DOG_SPI_DO_PIN 5
#warning DOG_SPI_DO_PIN not defined, bit 5 assumed
#endif 

#ifndef DOG_SPI_CS_PORT
#define DOG_SPI_CS_PORT B
#warning DOG_SPI_CS_PORT not defined, port B assumed
#endif 

#ifndef DOG_SPI_CS_PIN
#define DOG_SPI_CS_PIN 0
#warning DOG_SPI_CS_PIN not defined, bit 0 assumed
#endif 

#ifndef DOG_SPI_A0_PORT
#define DOG_SPI_A0_PORT B
#warning DOG_SPI_A0_PORT not defined, port B assumed
#endif 

#ifndef DOG_SPI_A0_PIN
#define DOG_SPI_A0_PIN 1
#warning DOG_SPI_A0_PIN not defined, bit 1 assumed
#endif 

void dog_spi_init(void)
{
  /* setup port directions */
  DOG_SPI_DDR(DOG_SPI_CS_PORT) |= _BV(DOG_SPI_CS_PIN);
  DOG_SPI_DDR(DOG_SPI_DO_PORT) |= _BV(DOG_SPI_DO_PIN);
  DOG_SPI_DDR(DOG_SPI_A0_PORT) |= _BV(DOG_SPI_A0_PIN);
  /* DOG_SPI_SCL_DDR |= _BV(DOG_SPI_SCL_PIN); */
  DOG_SPI_DDR(DOG_SPI_SCL_PORT) |= _BV(DOG_SPI_SCL_PIN);
}

unsigned char dog_spi_out(unsigned char data)
{
  uint8_t i = 8;
  /* the SCL level is set during dog_spi_enable_client() */
  /* output the data */
  USIDR = data;
  do
  {
    /* generate edge at the clock pin, edge types depends of the current pin state */
    /* slave should now sample the data */
    USICR = _BV(USIWM0) | _BV(USITC);
    /* output next bit and toggle the clock pin again*/
    USICR = _BV(USIWM0) | _BV(USICLK) | _BV(USITC);
    i--;
  } while( i != 0 );  
  return USIDR;
}

void dog_spi_enable_client(void)
{
  /* before the slave is enabled, esure that the clk pin has a logical zero */
  DOG_SPI_PORT(DOG_SPI_SCL_PORT) &= ~_BV(DOG_SPI_SCL_PIN);
  
  /* now enable the SPI slave */
  DOG_SPI_PORT(DOG_SPI_CS_PORT) &= ~_BV(DOG_SPI_CS_PIN);
}

void dog_spi_disable_client(void)
{
  /* disable the client (write a logical zero on the CS line) */
  DOG_SPI_PORT(DOG_SPI_CS_PORT) |= _BV(DOG_SPI_CS_PIN);
}

void dog_cmd_mode(void)
{
  DOG_SPI_PORT(DOG_SPI_A0_PORT) &= ~_BV(DOG_SPI_A0_PIN);
}

void dog_data_mode(void)
{
  DOG_SPI_PORT(DOG_SPI_A0_PORT) |= _BV(DOG_SPI_A0_PIN);
}

#elif defined(DOG_SPI_ATMEGA)

/*=======================================================================*/
/* SPI Interface of ATMEGA */
/*=======================================================================*/

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define _DOG_SPI_DDR(PCHAR) DDR ## PCHAR
#define DOG_SPI_DDR(PCHAR) _DOG_SPI_DDR(PCHAR)
#define _DOG_SPI_PORT(PCHAR) PORT ## PCHAR
#define DOG_SPI_PORT(PCHAR) _DOG_SPI_PORT(PCHAR)

/* dog_spi_pin_a0 is ignored, instead, port and pins are hard coded */
#ifndef DOG_SPI_SCL_PORT
#define DOG_SPI_SCL_PORT B
#warning DOG_SPI_SCL_PORT not defined, port B assumed
#endif
#ifndef DOG_SPI_SCL_PIN
#define DOG_SPI_SCL_PIN 5
#warning DOG_SPI_SCL_PIN not defined, bit 5 assumed
#endif


#ifndef DOG_SPI_MOSI_PORT
#define DOG_SPI_MOSI_PORT B
#warning DOG_SPI_MOSI_PORT not defined, port B assumed
#endif 

#ifndef DOG_SPI_MOSI_PIN
#define DOG_SPI_MOSI_PIN 3
#warning DOG_SPI_MOSI_PIN not defined, bit 3 assumed
#endif 

#ifndef DOG_SPI_A0_PORT
#define DOG_SPI_A0_PORT B
#warning DOG_SPI_A0_PORT not defined, port B assumed
#endif 

#ifndef DOG_SPI_A0_PIN
#define DOG_SPI_A0_PIN 1
#warning DOG_SPI_A0_PIN not defined, bit 1 assumed
#endif 

#ifndef DOG_SPI_SS_PORT
#define DOG_SPI_SS_PORT B
#warning DOG_SPI_SS_PORT not defined, port B assumed
#endif 

#ifndef DOG_SPI_SS_PIN
#define DOG_SPI_SS_PIN 2
#warning DOG_SPI_SS_PIN not defined, bit 2 assumed
#endif 

void dog_spi_init(void)
{
  /* NOTE: SS Pin must be set to output, otherwise the SPI system might assume a collision */
  
  /* setup port directions */
  DOG_SPI_DDR(DOG_SPI_MOSI_PORT) |= _BV(DOG_SPI_MOSI_PIN);
  DOG_SPI_DDR(DOG_SPI_A0_PORT) |= _BV(DOG_SPI_A0_PIN);
  DOG_SPI_DDR(DOG_SPI_SCL_PORT) |= _BV(DOG_SPI_SCL_PIN);
  DOG_SPI_DDR(DOG_SPI_SS_PORT) |= _BV(DOG_SPI_SS_PIN);
  
    /*
    SPR1 SPR0
	0	0		fclk/4
	0	1		fclk/16
	1	0		fclk/64
	1	1		fclk/128
  */
  
  
  
  SPCR = 0;
  SPCR =  (1<<SPE) | (1<<MSTR)|(0<<SPR1)|(0<<SPR0)|(0<<CPOL)|(0<<CPHA);
}

unsigned char dog_spi_out(unsigned char data)
{
  /* send data */
  SPDR = data;
  /* wait for transmission */
  while (!(SPSR & (1<<SPIF))) 
    ;
  /* clear the SPIF flag by reading SPDR */
  //dog_Delay(1);
  return  SPDR;
  //return data;
}

void dog_spi_enable_client(void)
{
  /* before the slave is enabled, esure that the clk pin has a logical zero */
  DOG_SPI_PORT(DOG_SPI_SCL_PORT) &= ~_BV(DOG_SPI_SCL_PIN);
  
  /* now enable the SPI slave */
  DOG_SPI_PORT(DOG_SPI_SS_PORT) &= ~_BV(DOG_SPI_SS_PIN);
}

void dog_spi_disable_client(void)
{
  /* disable the client (write a logical zero on the CS line) */
  DOG_SPI_PORT(DOG_SPI_SS_PORT) |= _BV(DOG_SPI_SS_PIN);
}

void dog_cmd_mode(void)
{
  DOG_SPI_PORT(DOG_SPI_A0_PORT) &= ~_BV(DOG_SPI_A0_PIN);
}

void dog_data_mode(void)
{
  DOG_SPI_PORT(DOG_SPI_A0_PORT) |= _BV(DOG_SPI_A0_PIN);
}



#elif defined(__18CXX)

/*=======================================================================*/
/* Microchip PIC18 SPI */
/*=======================================================================*/
#include <p18cxxx.h>

#define LCD_A0_TRIS		(TRISCbits.TRISC1)
#define LCD_A0_IO		(LATCbits.LATC1)
#define LCD_CS_TRIS		(TRISCbits.TRISC2)
#define LCD_CS_IO		(LATCbits.LATC2)

#define LCD_CLK_TRIS	(TRISCbits.TRISC3)
#define LCD_SDO_TRIS	(TRISCbits.TRISC5)
#define LCD_SDI_TRIS	(TRISCbits.TRISC4) // remove it!!!!
#define LCD_SPI_IF		(PIR1bits.SSPIF)
#define LCD_SSPBUF		(SSPBUF)
#define LCD_SPICON1		(SSPCON1)
#define LCD_SPICON1bits	(SSPCON1bits)
#define LCD_SPICON2		(SSPCON2)
#define LCD_SPISTAT		(SSPSTAT)
#define LCD_SPISTATbits	(SSPSTATbits)
#define LCD_SPI_ON_BIT  (LCD_SPICON1bits.SSPEN)
#define WaitForDataByte()   do{while(!LCD_SPI_IF); LCD_SPI_IF = 0;}while(0)


void dog_spi_init(void)
{
	LCD_CS_IO=1;
	LCD_A0_IO=1;

	LCD_A0_TRIS=0;
	LCD_CS_TRIS =0;

	LCD_CLK_TRIS =0;
	LCD_SDO_TRIS =0;
	LCD_SDI_TRIS =1;

	SSPSTAT=0x40;
	SSPCON1=0x21;

	PIR1bits.SSPIF=0;
}

unsigned char dog_spi_out(unsigned char data)
{
  		volatile uint8_t vDummy;
		LCD_SSPBUF=data;
		//     while (LCD_SPISTATbits.SPITBF == 1);
		WaitForDataByte();
		//LCD_SPI_IF=0;
		vDummy = LCD_SSPBUF;

}

void dog_spi_enable_client(void)
{
LCD_CS_IO=0;
}

void dog_spi_disable_client(void)
{
  LCD_CS_IO=1;
}

void dog_cmd_mode(void)
{
  LCD_A0_IO=0;
}

void dog_data_mode(void)
{
  LCD_A0_IO=1;
}

#elif defined(DOG_SPI_ARDUINO)

/*=======================================================================*/
/* Arduino SPI */
/*=======================================================================*/


#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <wiring.h>	/* arduino pinMode */

void dog_spi_init(void)
{
  pinMode(PIN_SCK, OUTPUT);
  pinMode(PIN_MOSI, OUTPUT);
  /* pinMode(PIN_MISO, INPUT); */

  // This this redundant with dog_spi_pin_cs... 
  // pinMode(PIN_SS, OUTPUT);			/* this must always be an output port */

  pinMode(dog_spi_pin_a0, OUTPUT);
  pinMode(dog_spi_pin_cs, OUTPUT);			/* this is the user chip select */

  /*
    SPR1 SPR0
	0	0		fclk/4
	0	1		fclk/16
	1	0		fclk/64
	1	1		fclk/128
  */
#ifndef ADA_ST7565P
  SPCR = 0;
  SPCR =  (1<<SPE) | (1<<MSTR)|(0<<SPR1)|(0<<SPR0)|(0<<CPOL)|(0<<CPHA);
#else
  pinMode(dog_spi_pin_rst, OUTPUT);
#endif
  /*
  {
  unsigned char x;
    x = SPSR;
    x= SPDR;
  }
  */
}

unsigned char dog_spi_out(unsigned char data)
{
#ifndef ADA_ST7565P
  /* unsigned char x = 100; */
  /* send data */
  SPDR = data;
  /* wait for transmission */
  while (!(SPSR & (1<<SPIF))) 
    ;
  /* clear the SPIF flag by reading SPDR */
  return  SPDR;
#else
  shiftOut(PIN_MOSI, PIN_SCK, MSBFIRST, data);
  // not sure what we should return here...
  // luckily, nothing upstream ever seems to use the return value!
  return data;
#endif
}

void dog_spi_enable_client(void)
{
        digitalWrite(dog_spi_pin_cs, LOW);  
}

void dog_spi_disable_client(void)
{
        digitalWrite(dog_spi_pin_cs, HIGH);
}

void dog_cmd_mode(void)
{
  digitalWrite(dog_spi_pin_a0, LOW);
}

void dog_data_mode(void)
{
  digitalWrite(dog_spi_pin_a0, HIGH);
}

#endif

