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
 * File:   mcp3008.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 2. Dezember 2012, 11:53
 */

#ifndef MCP3008_H
#define	MCP3008_H

#ifdef	__cplusplus
extern "C" {
#endif

#define ADC_0 0
#define ADC_1 1
#define ADC_2 2
#define ADC_3 3
#define ADC_4 4
#define ADC_5 5
#define ADC_6 6
#define ADC_7 7
  
#define MCP3008_ADC_SINGLE 0x08
#define MCP3008_ADC_DIFF 0x00
  
#define	MCP3008_CS_CLR 		bcm2835_gpio_clr(PIN_MCP300X_CS)
#define	MCP3008_CS_SET		bcm2835_gpio_set(PIN_MCP300X_CS)

  
int mcp3008_init();
int mcp3008_adc_read(uint8_t single_diff, uint8_t channel, uint16_t *adcout);


#ifdef	__cplusplus
}
#endif

#endif	/* MCP23S17_H */

