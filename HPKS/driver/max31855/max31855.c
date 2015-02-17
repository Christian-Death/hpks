/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 * 
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   mcp23s17.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 2. Dezember 2012, 11:52
 */


#include <defines/used_includes.h>

#include "max31855.h"

union bytes_to_uint32 {
  uint8_t bytes[4];
  uint32_t integer;
} buffer;
uint8_t rx[4];

int16_t _sonde_temp;
int16_t _intern_temp;
int16_t data;
uint8_t isfault;
uint8_t max31855_errornr;


double readInternal(uint32_t v)
{
  // ignore bottom 4 bits - they're just thermocouple data
  v >>= 4;

  // pull the bottom 11 bits off
  float internal = v & 0x7FF;
  // check sign bit!
  if (v & 0x800)
  {
    // Convert to negative value by extending sign and casting to signed type.
    int16_t tmp = 0xF800 | (v & 0x7FF);
    internal = tmp;
  }
  internal *= 0.0625; // LSB = 0.0625 degrees
  
  return internal;
}

double readCelsius(int32_t v)
{
    if (v & 0x80000000)
  {
    // Negative value, drop the lower 18 bits and explicitly extend sign bits.
    v = 0xFFFFC000 | ((v >> 18) & 0x00003FFFF);
  }
  else
  {
    // Positive value, just drop the lower 18 bits.
    v >>= 18;
  }

  double centigrade = v;

  // LSB = 0.25 degrees C
  centigrade *= 0.25;
  return centigrade;
}

void print_binar(uint8_t* val, uint8_t len)
{
  char str[9];

  memset(str,0,9);
  
  for(int i=len-1;i>=0;i--)
  {
    uint8_t v = val[i];
    for(int j=7;j>=0;j--)
    {
      str[7-j] = (v & (0x01<<j))? '1':'0';
    }
    dbg_printf("%s ", str);
  }
  dbg_printf("\n");
}


int max31855_read(uint8_t sonde_nr, int16_t *sonde_temp, int16_t *intern_temp)
{
  bcm2835_spi_begin();
  bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST); // The default
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0); // The default
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024); // ~ 4 MHz
  if(sonde_nr == RPI_BPLUS_GPIO_J8_24) // CS0
  {
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0); // 
  }
  else if(sonde_nr == RPI_BPLUS_GPIO_J8_26){ // CS1
    bcm2835_spi_chipSelect(BCM2835_SPI_CS1); // 
  }
  else {
    bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE); // 
  
  }

  if(sonde_nr != RPI_BPLUS_GPIO_J8_24 && sonde_nr != RPI_BPLUS_GPIO_J8_26) // CS1
  {
    MAX31855_CS_CLR(sonde_nr); // 
  }
  
  
  buffer.integer = 0;
  uint8_t mosi[10] = { 0 };
	uint8_t miso[10] = { 0 };
    
  bcm2835_spi_transfernb(mosi,miso, 4);

  if(sonde_nr != RPI_BPLUS_GPIO_J8_24 && sonde_nr != RPI_BPLUS_GPIO_J8_26) // CS1
  {
    MAX31855_CS_SET(sonde_nr);
  }
  bcm2835_spi_end();

#if 0
  buffer.bytes[3]=miso[3];
  buffer.bytes[2]=miso[2];
  buffer.bytes[1]=miso[1];
  buffer.bytes[0]=miso[0];
#else
  buffer.bytes[0]=miso[3];
  buffer.bytes[1]=miso[2];
  buffer.bytes[2]=miso[1];
  buffer.bytes[3]=miso[0];
#endif
  uint32_t v = buffer.integer;
  
  //dbg_printf("Lesen Sonde: %u\n", sonde_nr);
  //print_binar(buffer.bytes, 4);

  if (v & 0x7)
  {
    isfault = true;
    if ((v & 0x1) != 0)
    {
      max31855_errornr = 0x1;
      //dbg_printf("keine Verbindung\n");
    }
    if ((v & 0x2) != 0)
    {
      max31855_errornr = 0x2;
      //dbg_printf("Kurzschluss zu GND\n");
    }
    if ((v & 0x4) != 0)
    {
      max31855_errornr = 0x4;
      //dbg_printf("Kurzschluss zu VCC\n");
    }
  }
  else
  {
    isfault = false;
    _sonde_temp = readCelsius(v);
  }
  _intern_temp = readInternal(v);
 
  *sonde_temp = _sonde_temp;
  *intern_temp = _intern_temp;

  return !isfault;
}

char * max31855_geterrorstr(void)
{
  if ((max31855_errornr & 0x1) != 0) return "keine Verbindung";
  if ((max31855_errornr & 0x2) != 0) return "Kurzschluss zu GND";
  if ((max31855_errornr & 0x4) != 0) return "Kurzschluss zu VCC";

  return "Kein Error";
}

