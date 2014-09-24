
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
 * File:   spi.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 29. November 2012, 22:07
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>


#include "spi.h"


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

int fd = -1;;

uint8_t mode = 0;
/*
 *
 */
static int spi_config( SPI_CONFIG_struct * sc){
  
  int ret = 0;
  if( mode == sc->mode)
    return(1);
  mode = sc->mode;
  
  /*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1) {
		printf("can't set spi mode");
    return ret;
  }
	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1) {
		printf("can't get spi mode");
    return ret;
  }
  
	printf("spi mode: %d\n", sc->mode);
	printf("bits per word: %d\n", sc->bits);
	printf("max speed: %d Hz (%d KHz)\n", sc->speed, sc->speed/1000);

  return 1;
}

/*
 *
 */
int spi_transfer( SPI_CONFIG_struct * sc, uint8_t d ){
  
  if(fd<0)
    return -1;
  
  if(spi_config(sc));
 
 
  int ret;
	
  uint8_t a[1];
	a[0]=d;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)((uint8_t *)(sc->tx_buf)),
		.rx_buf = (unsigned long)((uint8_t *)(sc->rx_buf)),
		.len = sc->len,
		.delay_usecs = sc->delay,
		.speed_hz = sc->speed,
		.bits_per_word = sc->bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1) {
		printf("can't send spi message");
    return -1;
  }
  
/*
  if(sc->rx_buf != NULL) {
    for (ret = 0; ret < sc->len; ret++) {
      if (!(ret % 6))
        puts("");
      printf("%.2X ", ((uint8_t *)(sc->rx_buf))[ret]);
    }
    puts("");
  }
*/
  
  return 1;
}

/*
 * 
 */
int spi_deinit() {

  if(fd<0)
    return fd;
  
  close(fd);
  fd = -1;
  return 1;
}

/*
 * 
 */
int spi_init() {
  
  int ret = 0;
  const char *device = SPI_DEVICE;
  
  fd = open(device, O_RDWR);
	if (fd < 0) {
		printf("can't open device");
  }
  
  /*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1) {
		printf("can't set spi mode");
    return ret;
  }
	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1) {
		printf("can't get spi mode");
    return ret;
  }
  
  return fd;
	
}

