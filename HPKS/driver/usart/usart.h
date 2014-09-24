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
 * File:   usart.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 12. Dezember 2012, 10:48
 */

#ifndef USART_H
#define	USART_H

#ifdef	__cplusplus
extern "C" {
#endif

extern int   usart_open      (char *device, int baud);
extern void  usart_close     (int fd);
extern void  usart_flush     (int fd);
extern void  usart_putc      (int fd, uint8_t c);
extern int  usart_getc  (int fd);
extern void  usart_puts      (int fd, char *s);
extern void  usart_printf    (int fd, char *message, ...);
extern int   usart_dataAvail (int fd);
extern ssize_t usart_get (int fd, char *buffer, size_t size);


#ifdef	__cplusplus
}
#endif

#endif	/* USART_H */

