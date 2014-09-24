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
 * File:   board.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 27. November 2012, 19:17
 */

#ifndef BOARD_H
#define	BOARD_H

#ifdef	__cplusplus
extern "C" {
#endif


//#define USE_LCD    
//#define USE_BUTTONS
//#define USE_PORTEXPANDER
    
#define USE_INPUTS
#define USE_SHAKER
  
#ifdef USE_PORTEXPANDER 
// Pin Setup for PortExpander MCP23S17
#   define	PIN_MCP23S17_CS 8
#endif
    
// Pin Setup for MAX31855 Thermo K-Sonde
#define	PIN_MAX31855_U4_CS RPI_BPLUS_GPIO_J8_22
#define K_SONDE_BRENNER PIN_MAX31855_U4_CS
    
// Pin Setup for MAX31855 Thermo K-Sonde
#define	PIN_MAX31855_U5_CS RPI_BPLUS_GPIO_J8_26 
#define K_SONDE_ABGAS PIN_MAX31855_U5_CS
    
// Pin Setup for PIN_MCP300X_CS A/D Wandler
#define	PIN_MCP300X_CS RPI_BPLUS_GPIO_J8_18
 
#ifdef USE_SHAKER
#   define SHAKER1 RPI_BPLUS_GPIO_J8_35
#   define SHAKER2 RPI_BPLUS_GPIO_J8_37
#endif
    
#ifdef USE_INPUTS
#   define INP1 RPI_BPLUS_GPIO_J8_29
#   define INP2 RPI_BPLUS_GPIO_J8_31
#   define INP3 RPI_BPLUS_GPIO_J8_32
#   define INP4 RPI_BPLUS_GPIO_J8_33
#endif
    
    
// Pin Setup for RaspiLCD
#ifdef USE_LCD
  #ifdef USE_PORTEXPANDER
    # define		PIN_LCD_CS		25
    // Pins am PortExpanter
    # define		PIN_LCD_RST		PIN5
    # define		PIN_LCD_RS		PIN6
    # define		PIN_LCD_BACKLIGHT	PIN7
  #else
    # define		PIN_LCD_RST		25
    # define		PIN_LCD_CS		8
    # define		PIN_LCD_RS		7
    # define		PIN_LCD_BACKLIGHT	18
  #endif
  
#endif

#define USE_WEBSERVER
  
#define PT100_ADC ADC_1
#define LAMBDA_CHECK_ADC ADC_0  
  
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* BOARD_H */

