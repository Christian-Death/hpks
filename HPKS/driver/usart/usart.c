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
 * File:   usart.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 12. Dezember 2012, 10:52
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "usart.h"


/*
 * serialOpen:
 *	Open and initialise the serial port, setting all the right
 *	port parameters - or as many as are required - hopefully!
 *********************************************************************************
 */

int usart_open(char *device, int baud) {
    
  struct termios options ;
  speed_t spBaud ;
  int     status, fd ;

  switch (baud)
  {
    case     50:        spBaud =     B50 ; break ;
    case     75:	spBaud =     B75 ; break ;
    case    110:	spBaud =    B110 ; break ;
    case    134:	spBaud =    B134 ; break ;
    case    150:	spBaud =    B150 ; break ;
    case    200:	spBaud =    B200 ; break ;
    case    300:	spBaud =    B300 ; break ;
    case    600:	spBaud =    B600 ; break ;
    case   1200:	spBaud =   B1200 ; break ;
    case   1800:	spBaud =   B1800 ; break ;
    case   2400:	spBaud =   B2400 ; break ;
    case   9600:	spBaud =   B9600 ; break ;
    case  19200:	spBaud =  B19200 ; break ;
    case  38400:	spBaud =  B38400 ; break ;
    case  57600:	spBaud =  B57600 ; break ;
    case 115200:	spBaud = B115200 ; break ;
    case 230400:	spBaud = B230400 ; break ;

    default:
      return -2 ;
  }

  if ((fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1)
    return -1 ;

  fcntl (fd, F_SETFL, O_RDWR) ;

// Get and modify current options:

  tcgetattr (fd, &options) ;

    cfmakeraw   (&options) ;
    cfsetispeed (&options, spBaud) ;
    cfsetospeed (&options, spBaud) ;

    options.c_cflag |= (CLOCAL | CREAD) ;
    options.c_cflag &= ~PARENB ;
    options.c_cflag &= ~CSTOPB ;
    options.c_cflag &= ~CSIZE ;
    options.c_cflag |= CS8 ;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    options.c_oflag &= ~OPOST ;

    options.c_cc [VMIN]  =   0 ;
    options.c_cc [VTIME] = 100 ;	// Ten seconds (100 deciseconds)

  tcsetattr (fd, TCSANOW | TCSAFLUSH, &options) ;

  ioctl (fd, TIOCMGET, &status);

  status |= TIOCM_DTR ;
  status |= TIOCM_RTS ;

  ioctl (fd, TIOCMSET, &status);

  usleep (10000) ;	// 10mS

  return fd ;
}

/*
 * serialClose:
 *	Release the serial port
 *********************************************************************************
 */
void usart_close (int fd) {
  close (fd) ;
}

/*
 * serialFlush:
 *	Flush the serial buffers (both tx & rx)
 *********************************************************************************
 */
void usart_flush (int fd) {
  tcflush (fd, TCIOFLUSH) ;
}


/*
 * serialPutchar:
 *	Send a single character to the serial port
 *********************************************************************************
 */

void usart_putc (int fd, uint8_t c)
{
  write (fd, &c, 1) ;
}


/*
 * serialPuts:
 *	Send a string to the serial port
 *********************************************************************************
 */

void usart_puts (int fd, char *s)
{
  write (fd, s, strlen (s)) ;
}

/*
 * serialPrintf:
 *	Printf over Serial
 *********************************************************************************
 */

void usart_printf (int fd, char *message, ...)
{
  va_list argp ;
  char buffer [1024] ;

  va_start (argp, message) ;
  vsnprintf (buffer, 1023, message, argp) ;
  va_end (argp) ;

  usart_puts (fd, buffer) ;
}


/*
 * serialDataAvail:
 *	Return the number of bytes of data avalable to be read in the serial port
 *********************************************************************************
 */

int usart_dataAvail (int fd)
{
  int result ;

  if (ioctl (fd, FIONREAD, &result) == -1)
    return -1 ;

  return result ;
}


/*
 * serialGetchar:
 *	Get a single character from the serial device.
 *	Note: Zero is a valid character and this function will time-out after
 *	10 seconds.
 *********************************************************************************
 */

int usart_getc (int fd)
{
  uint8_t x ;

  if (read (fd, &x, 1) != 1)
    return -1 ;

  return ((int)x) & 0xFF ;
}

ssize_t usart_get (int fd, char *buffer, size_t size)
{
  return read (fd, buffer, size);
   
}
