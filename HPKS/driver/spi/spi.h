
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
 * File:   spi.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 29. November 2012, 22:07
 */

#ifndef SPI_H
#define	SPI_H

#include <conf_spi.h>
#include <linux/spi/spidev.h>

#ifdef	__cplusplus
extern "C" {
#endif

 typedef struct 
   {
      uint8_t mode;
      uint8_t bits;
      uint32_t speed;
      uint16_t delay;
      void	*tx_buf;
      void	*rx_buf;
    	__u32		len;
   }SPI_CONFIG_struct ;
   
int spi_init();
int spi_deinit();
int spi_transfer( SPI_CONFIG_struct * sc, uint8_t d);

#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */

