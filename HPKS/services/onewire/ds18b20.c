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
 * File:   ds18b20.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 2. Dezember 2012, 11:52
 */


#include <defines/used_includes.h>
#include "ds18b20.h"

#define MAXBYTES 1024
#define _devicePath  "/sys/bus/w1/devices/w1_bus_master1/"

#define MAX_DEVICE 20

static bool is_Init = false;

int _ds18b20_count = 0;
int _ds18b20_current_device = 0;
static uint8_t control_ds18b20_count = 0;
DS18B20_struct _devicelist[MAX_DEVICE] = {0};
static double average = 0.0;
MEASUREMENT_VIEW_struct PufferTemperatureMem;
static unsigned int last_ms;

int ds18b20_controlDeviceCount() {

  char filename[256];
  int fd = -1;
  int bufcount;

  uint8_t i;


  char buffer[MAXBYTES + 1] = {0};

  strcpy(filename, _devicePath);
  strcat(filename, "w1_master_slaves");

  // Open the master /dev/memory device
  if ((fd = open(filename, O_RDONLY)) < 0) {
    dbg_printf("ds18b20_getDeviceList: Unable to open %s: %s\n",
            filename, strerror(errno));
    return -1;
  }

  char * nextWordPtr;
  int count = 0;
  while (bufcount = read(fd, buffer, MAXBYTES)) {
    nextWordPtr = strtok(buffer, "\n"); // split using space as divider

    if (!strncmp("not found.", nextWordPtr, strlen("not found.")))
      return 0;

    while (nextWordPtr != NULL) {

      count++;
      nextWordPtr = strtok(NULL, "\n");
    }
  };

  close(fd);

  return count;

}

/*!***************************************************************************
 * 
 * 
 *****************************************************************************/
int ds18b20_getDeviceList() {
  char filename[256];
  int fd = -1;
  int count;

  uint8_t i;
  for (i = 0; i < 128; i++) PufferTemperatureMem.Mem[i] = 0;
  is_Init = true;

  char buffer[MAXBYTES + 1] = {0};

  strcpy(filename, _devicePath);
  strcat(filename, "w1_master_slaves");

  // Open the master /dev/memory device
  if ((fd = open(filename, O_RDONLY)) < 0) {
    dbg_printf("ds18b20_getDeviceList: Unable to open %s: %s\n",
            filename, strerror(errno));
    return -1;
  }

  char * nextWordPtr;
  _ds18b20_count = 0;
  _ds18b20_current_device = 0;
  while (count = read(fd, buffer, MAXBYTES)) {
    nextWordPtr = strtok(buffer, "\n"); // split using space as divider

    if (!strncmp("not found.", nextWordPtr, strlen("not found.")))
      return _ds18b20_count;

    while (nextWordPtr != NULL) {
      strncpy(_devicelist[_ds18b20_count]._deviceid, nextWordPtr, 20);
      dbg_printf("word%d %s\n", _ds18b20_count, nextWordPtr);
      _ds18b20_count++;
      nextWordPtr = strtok(NULL, "\n");
    }
  };

  close(fd);

  return _ds18b20_count;
}

/*!***************************************************************************
 * 
 * 
 *****************************************************************************/
double ds18b20_getAverage() {
  return average;
}

void ds18b20_readData() {
  char filename[256];
  int fd = -1;
  int count, i;

  char buffer[MAXBYTES + 1];

  if (!_ds18b20_count)
    return;

  strcpy(filename, _devicePath);
  strcat(filename, _devicelist[_ds18b20_current_device]._deviceid);
  strcat(filename, "/w1_slave");

  // Open the master /dev/memory device
  if ((fd = open(filename, O_RDONLY)) < 0) {
    dbg_printf("ds18b20_readData: Unable to open %s: %s\n",
            filename, strerror(errno));
    return;
  }

  char * nextWordPtr;

char temp[6];
  while (count = read(fd, buffer, MAXBYTES)) {
    // Fehler beim einlesen?
    if (count < 70) break;

    nextWordPtr = strtok(buffer, "\n"); // split using space as divider


    _devicelist[_ds18b20_current_device].crc = (strncmp(nextWordPtr + 36, "YES", 3)) ? 0 : 1;

    nextWordPtr = strtok(NULL, "\0\n");
    strncpy(temp, nextWordPtr + 29, 5);
    temp[5]=0;
    _devicelist[_ds18b20_current_device].tempGrad = atoi(temp) / 1000.0;

    //my $temp = (substr(@data[1], 29, 5)/1000.00);
    //my $fahr = ($temp*9.0/5.0)+32.00;


  };

  close(fd);

  if (++_ds18b20_current_device >= _ds18b20_count) {
    average = 0.0;
    do {
      --_ds18b20_current_device;
      average += _devicelist[_ds18b20_current_device].tempGrad;
    } while (_ds18b20_current_device);
    average /= _ds18b20_count;


    sprintf(PufferTemperatureMem.cur_value, "%5.1f", average);
    PufferTemperatureMem.cur_value[5] = 0xBA/*'°'*/;
    PufferTemperatureMem.cur_value[6] = 'C';
    PufferTemperatureMem.cur_value[7] = 0;

    PufferTemperatureMem.max = average;
    PufferTemperatureMem.min = average;

    for (i = 126; i > 0; i--) {
      PufferTemperatureMem.Mem[i + 1] = PufferTemperatureMem.Mem[i];
      if (PufferTemperatureMem.Mem[i] > PufferTemperatureMem.max)
        PufferTemperatureMem.max = PufferTemperatureMem.Mem[i];
      if (PufferTemperatureMem.Mem[i] < PufferTemperatureMem.min)
        PufferTemperatureMem.min = PufferTemperatureMem.Mem[i];
    }
    PufferTemperatureMem.Mem[1] = PufferTemperatureMem.Mem[0];
    PufferTemperatureMem.Mem[0] = average;
  }



}

int ds18b20_getDeviceCount() {
  return _ds18b20_count;
}

DS18B20_struct *ds18b20_getDevice(int number) {
  if (number > _ds18b20_count)
    return NULL;

  return &_devicelist[number];
}

DS18B20_struct *ds18b20_getDevicewithID(const char* ID)
{
  for(int i=0; i<_ds18b20_count; i++) {
    if(!strncmp(_devicelist[i]._deviceid, ID, strlen(_devicelist[i]._deviceid)))
      return &_devicelist[i];
  }
  return NULL;
}

void ds18b20_refreshData()
{
  int i = ds18b20_getDeviceList();
  while (i--) {
    ds18b20_readData();
    SleepMs(10);
  };
  app_config_read_ds18b20();
      
  control_ds18b20_count = 0;

}

void ds18b20_getData() {
  if (!is_Init) return;

  if (++control_ds18b20_count > 30) {
    int count = ds18b20_controlDeviceCount();
    if (count > 0 && count != _ds18b20_count) {
      int i = ds18b20_getDeviceList();
      while (i--) {
        ds18b20_readData();
        SleepMs(10);
      };
      app_config_read_ds18b20();
    }
    control_ds18b20_count = 0;
  }

  ds18b20_readData();


}

MEASUREMENT_VIEW_struct * ds12b20_getViewMem() {
  return &PufferTemperatureMem;
}

double ds18b20_get_durch_temp() {
  DS18B20_struct *ds18b20_dev;
  double puffer_temp = 0.0;
  int puffer_temp_count = 0;

  if(app_config_is_simulate_mode())
  {
    return simulate_get_durch_temp();
  }
  else
  {
    for (int i = 0; i < _ds18b20_count; i++) {
      ds18b20_dev = ds18b20_getDevice(i);
      if (ds18b20_dev->crc) {
        if (ds18b20_dev->puf_id != -1 && ds18b20_dev->puf_id != 255) {
          puffer_temp += ds18b20_dev->tempGrad;
          puffer_temp_count++;
        }
      }
    }

    return (puffer_temp / (double) puffer_temp_count);
  }
}