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
 * File:   mcp23s17.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 2. Dezember 2012, 11:53
 */

#ifndef MCP23S17_H
#define	MCP23S17_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum  { ACTIVE_LOW , ACTIVE_HIGH }Polarity;
typedef enum  { PORT_A, PORT_B }Port;

#define PIN0 (0x01 << 0)
#define PIN1 (0x01 << 1)
#define PIN2 (0x01 << 2)
#define PIN3 (0x01 << 3)
#define PIN4 (0x01 << 4)
#define PIN5 (0x01 << 5)
#define PIN6 (0x01 << 6)
#define PIN7 (0x01 << 7)
  
#define	MCP23S17_CS_CLR 		bcm2835_gpio_clr(PIN_MCP23S17_CS)
#define	MCP23S17_CS_SET			bcm2835_gpio_set(PIN_MCP23S17_CS)

  
int mcp23s17_init();

    int mcp23s17_direction(Port port, uint8_t direction);
    int mcp23s17_configurePullUps(Port port, uint8_t offOrOn);
    int mcp23s17_interruptEnable(Port port, uint8_t interruptsEnabledMask);
    int mcp23s17_interruptPolarity(Polarity polarity);
    int mcp23s17_mirrorInterrupts(bool mirror);
    int mcp23s17_defaultValue(Port port, uint8_t valuesToCompare);
    int mcp23s17_interruptControl(Port port, uint8_t interruptContolBits);
    int mcp23s17_read(Port port);
    int mcp23s17_write(Port port, uint8_t byte);
    int mcp23s17_set_pin(Port port, uint8_t pin);
    int mcp23s17_clr_pin(Port port, uint8_t pin);

#ifdef	__cplusplus
}
#endif

#endif	/* MCP23S17_H */

