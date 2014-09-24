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
 * File:   ds18b20.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 10. Dezember 2012, 20:32
 */

#ifndef DS18B20_H
#define	DS18B20_H

#ifdef	__cplusplus
extern "C" {
#endif


typedef struct {
  char _deviceid[DS18B20_ID_LENGTH];
  int16_t puf_id;
  int8_t puf_pos;
  uint8_t crc;
  double tempGrad;
}DS18B20_struct;

int ds18b20_getDeviceList();
void ds18b20_readData();
void ds18b20_getData();
double ds18b20_getAverage();
int ds18b20_getDeviceCount();
DS18B20_struct *ds18b20_getDevice(int number);
DS18B20_struct *ds18b20_getDevicewithID(const char* ID);

double ds18b20_get_durch_temp();
MEASUREMENT_VIEW_struct * ds12b20_getViewMem();

#ifdef	__cplusplus
}
#endif

#endif	/* DS18B20_H */

