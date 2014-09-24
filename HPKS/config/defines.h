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
 * File:   defines.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 2. April 2013, 20:21
 */

#ifndef DEFINES_H
#define	DEFINES_H

#ifdef	__cplusplus
extern "C" {
#endif

#define OFF 0
#define ON 1

#define DS18B20_ID_LENGTH 20

#define MAX_PUFFER_COUNT 5
#define DEF_PUFFERVOLUMEN 2000
#define MIN_PUFFERVOLUMEN 100
#define MAX_PUFFERVOLUMEN 10000

#define MAX_DS18B20_COUNT 20 

#define DEF_SPEZ_WAERMEKAP_WASSER 4187
#define DEF_MAX_PUFFER_TEMP 80
#define DEF_MIN_PUFFER_TEMP 40
  
#define DEF_HEIZWERT_HOLZ 3.5
#define DEF_SERVER_PORT 5100

#define DEF_SHAKER_AKTIVE 0.5
#define DEF_SHAKING 1.1
#define DEF_SHAKER_RUN 3
#define DEF_SHAKER_PAUSE 30
#define DEF_SHAKING_COUNT 20
#define DEF_ABGAS_END_TEMP 100
  


extern char app_fullpath[FILENAME_MAX];

#define sizearray(a)  (sizeof(a) / sizeof((a)[0]))
#define  CONFIG_FILE_NAME "hvs.ini"
#define  SIM_FILE_NAME "sim.ini"

  typedef struct {
    uint8_t dec_counter;
    uint16_t min;
    uint16_t max;
    char cur_value[32];
    uint16_t Mem[128];
  } MEASUREMENT_VIEW_struct;


#ifdef	__cplusplus
}
#endif

#endif	/* DEFINES_H */

