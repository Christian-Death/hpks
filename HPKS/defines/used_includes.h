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
 * File:   used_includes.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 27. November 2012, 19:28
 */

#ifndef USED_INCLUDES_H
#define	USED_INCLUDES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <bcm2835.h>

#include "board.h"
#include "defines.h"

#include "../utils/utils.h"
#include "../events.h"
#include "../regelung.h"

#include "../driver/spi/spi.h"
    
#ifdef USE_PORTEXPANDER  
#   include "../driver/mcp23s17/mcp23s17.h"
#endif
#include "../driver/max31855/max31855.h"

#include "../driver/mcp3008/mcp3008.h"



#ifdef USE_BUTTONS
# include "../driver/buttons/buttons.h"
#endif
  
#ifdef USE_LCD
#   include "../driver/lcd/lcd.h"
#   include "../driver/lcd/lcd_hw.h"
#   include "lcd_dispay.h"
#endif
  

//#include "../services/k_sonde/k_sonde.h"
#include "../temp_brenner.h"
#include "../temp_abgas.h"
#include "../services/onewire/ds18b20.h"
#include "../services/o2_lambda/o2_lambda.h"
#include "../services/raspi/cpu_temp.h"
#include "../services/pt100/pt100.h"
#include "../services/lambda_check/lambda_check.h"
  
#ifdef USE_WEBSERVER
#   include "../json_worker.h"
#   include "../services/webserver/webserver.h"
#endif
  
#include "../shaker.h"
  
#include "windows.h"

#include "../services/config/app_config.h"
#include "../simulate.h"
  
#ifdef	__cplusplus
}
#endif

#endif	/* USED_INCLUDES_H */

