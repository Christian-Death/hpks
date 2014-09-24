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


uint8_t rx[4];

int16_t _sonde_temp;
int16_t _intern_temp;
int16_t data;
uint8_t isfault;
uint8_t max31855_errornr;


static  SPI_CONFIG_struct max31855_spi_config = {
  SPI_MODE_3|SPI_NO_CS,
  0,
  0,
  0,
  NULL,
  rx,
  4};


int max31855_read(uint8_t sonde_nr, int16_t *sonde_temp, int16_t *intern_temp) {
  
  
  // Deactive the CS pin
  MAX31855_CS_CLR(sonde_nr);

    
  spi_transfer(&max31855_spi_config, 0);
  // Active the CS pin
  MAX31855_CS_SET(sonde_nr);

  
  _sonde_temp = 0;
  _intern_temp = 0;
  
  // Thermocouple temperature data
  data = (int16_t)((rx[0] << 8) | rx[1]);

  // Fault bit
  if ((data & 0x1) != 0) {
    isfault = true;
    //Debug.Print("Fault");
  }
  else {
    isfault = false;
    // Temperature - 14 bits, signed, 0.25 degree C resolution
    _sonde_temp = ((int16_t)(data & 0xFFFC)) / 16.0;
    //Debug.Print(sonde_temp.ToString());
  }

  // Internal temperature data
  data = (int16_t)((rx[2] << 8) | rx[3]);

  // Bits 0-2 are fault flags
  if ((data & 0x7) != 0)
  {
    
    //Debug.Print("Fault");
    if ((data & 0x1) != 0) max31855_errornr = 0x1;
    if ((data & 0x2) != 0) max31855_errornr = 0x2;
    if ((data & 0x4) != 0) max31855_errornr = 0x4;
  }
  else
  {
    // Temperature - 12 bits, signed, 0.0625 degree C resolution
    _intern_temp = ((int16_t)(data & 0xFFF0)) / 256.0;
    //Debug.Print(intern_temp.ToString());
  }
  *sonde_temp = _sonde_temp;
  *intern_temp = _intern_temp;
  
  return !isfault;
}

char * max31855_geterrorstr(void) {
   if ((max31855_errornr & 0x1) != 0) return "keine Verbindung";
   if ((max31855_errornr & 0x2) != 0) return "Kurzschluss zu GND";
   if ((max31855_errornr & 0x4) != 0) return "Kurzschluss zu VCC";
   
   return "Kein Error";
}

