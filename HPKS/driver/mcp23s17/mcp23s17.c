
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

#ifdef USE_PORTEXPANDER
#include "mcp23s17.h"


#define INTERRUPT_POLARITY_BIT 0x02
#define INTERRUPT_MIRROR_BIT   0x40
 
// all register addresses assume IOCON.BANK = 0 (POR default)
 
#define IODIRA   0x00
#define GPINTENA 0x04
#define DEFVALA  0x06
#define INTCONA  0x08
#define IOCON    0x0A
#define GPPUA    0x0C
#define GPIOA    0x12
#define OLATA    0x14
 
// Control settings
 
#define IOCON_BANK  0x80 // Banked registers
#define IOCON_BYTE_MODE 0x20 // Disables sequential operation. If bank = 0, operations toggle between A and B registers
#define IOCON_HAEN  0x08 // Hardware address enable
 
uint8_t tx[3];
uint8_t rx[3];

static uint8_t gpio_a_buf;
static uint8_t gpio_b_buf;


static  SPI_CONFIG_struct mcp23s17_spi_config = {
  SPI_MODE_3|SPI_NO_CS,
  0,
  1000000,
  0,
  tx,
  rx,
  3};

#define SPI_SLAVE_ID    0x40
#define SPI_SLAVE_ADDR  0x00      // A2=0,A1=0,A0=0
#define SPI_SLAVE_WRITE 0x00
#define SPI_SLAVE_READ  0x01      // MCP23S17 Registers Definition for BANK=0 (default)

int _mcp23s17_read(uint8_t regaddr) {
    // Start MCP23S17 OpCode transmission
  // 

  // Active the CS pin
  MCP23S17_CS_CLR;
  
  // 0100 aaa r/w
  tx[0] = SPI_SLAVE_ID | ((SPI_SLAVE_ADDR << 1) & 0x0E) | SPI_SLAVE_READ;
	//spi_transfer(&mcp23s17_spi_config, tx[0]);

  tx[1] = regaddr;
   tx[2] = 0; 
	//spi_transfer(&mcp23s17_spi_config, tx[0]);


	spi_transfer(&mcp23s17_spi_config, tx[0]);
  
  MCP23S17_CS_SET;

  return (EXIT_SUCCESS);
}
 
int _read(Port port, uint8_t address) {
  _mcp23s17_read(address + (uint8_t) port);
  return rx[2];
}
 
int _mcp23s17_write(uint8_t regaddr, uint8_t data) {
 // Start MCP23S17 OpCode transmission
  // 
  // Active the CS pin
  MCP23S17_CS_CLR;
          
  // 0100 aaa r/w
  tx[0] = SPI_SLAVE_ID | ((SPI_SLAVE_ADDR << 1) & 0x0E) | SPI_SLAVE_WRITE;
	//spi_transfer(&mcp23s17_spi_config, tx[0]);

  tx[1] = regaddr;
	//spi_transfer(&mcp23s17_spi_config, tx[0]);

  tx[2] = data;
	spi_transfer(&mcp23s17_spi_config, tx[0]);
  
  MCP23S17_CS_SET;

  return (EXIT_SUCCESS);
}
 
int  _write(Port port, uint8_t address, uint8_t data) {
    _mcp23s17_write(address + (uint8_t) port, data);
}
 
int _mcp23s17_init() {
    _mcp23s17_write(IOCON, (IOCON_BYTE_MODE | IOCON_HAEN )); // Hardware addressing on, operations toggle between A and B registers
}
 

int mcp23s17_direction(Port port, uint8_t direction) {
    _write(port, IODIRA, direction);
}
 
int mcp23s17_configurePullUps(Port port, uint8_t offOrOn) {
    _write(port, GPPUA, offOrOn);
}
 
int mcp23s17_interruptEnable(Port port, uint8_t interruptsEnabledMask) {
    _write(port, GPINTENA, interruptsEnabledMask);
}
 
int mcp23s17_mirrorInterrupts(bool mirror) {
 int iocon = _mcp23s17_read(IOCON);
    if (mirror) {
        iocon = iocon | INTERRUPT_MIRROR_BIT;
    } else {
        iocon = iocon & ~INTERRUPT_MIRROR_BIT;
    }
    _mcp23s17_write(IOCON, iocon);
 
}
 
int  mcp23s17_interruptPolarity(Polarity polarity) {
    char iocon = _mcp23s17_read(IOCON);
    if (polarity == ACTIVE_LOW) {
        iocon = iocon & ~INTERRUPT_POLARITY_BIT;
    } else {
        iocon = iocon | INTERRUPT_POLARITY_BIT;
    }
    _mcp23s17_write(IOCON, iocon);
}
 
int mcp23s17_defaultValue(Port port, uint8_t valuesToCompare) {
    _write(port, DEFVALA, valuesToCompare);
}
 
int mcp23s17_interruptControl(Port port, uint8_t interruptContolBits) {
    _write(port, INTCONA, interruptContolBits);
}
 
int mcp23s17_write(Port port, uint8_t byte) {
    _write(port, GPIOA, byte);
}
 
int mcp23s17_read(Port port) {
  return app_config_is_simulate_mode()?simulate_mcp23s17_read(port):_read(port, GPIOA);

}

int mcp23s17_set_pin(Port port, uint8_t pin) {
  uint8_t dummy = mcp23s17_read( port);
  dummy = dummy | pin;
  mcp23s17_write(port, dummy);
}

int mcp23s17_clr_pin(Port port, uint8_t pin) {
  uint8_t dummy = mcp23s17_read( port);
  dummy = dummy & ~pin;
  mcp23s17_write(port, dummy);
}


/*
 * 
 */
int mcp23s17_init() {
  _mcp23s17_init();
  
  // alle erst mal auf input
 // mcp23s17_write(IODIRA, 0xFF);  //
 // mcp23s17_write(IODIRB, 0xFF ); 
  gpio_a_buf = 0;
  gpio_b_buf = 0;
}

#endif