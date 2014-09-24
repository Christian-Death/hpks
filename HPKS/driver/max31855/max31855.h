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
 * File:   max31855.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 2. Dezember 2012, 15:18
 */

#ifndef MAX31855_H
#define	MAX31855_H

#ifdef	__cplusplus
extern "C" {
#endif

#define	MAX31855_CS_CLR(K_SONDE_CS) 		bcm2835_gpio_clr(K_SONDE_CS)
#define	MAX31855_CS_SET(K_SONDE_CS)		bcm2835_gpio_set(K_SONDE_CS)


int max31855_read(uint8_t sonde_nr, int16_t *sonde_temp, int16_t *intern_temp);
char * max31855_geterrorstr(void);

#ifdef	__cplusplus
}
#endif

#endif	/* MAX31855_H */

