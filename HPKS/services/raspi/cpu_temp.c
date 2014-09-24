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
 * File:   cpu_temp.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 2. Dezember 2012, 11:52
 */


#include <defines/used_includes.h>
#include "cpu_temp.h"
 

MEASUREMENT_VIEW_struct CpuTemperatureMem;


void cpu_temp_init(void)
{
	uint8_t i;

	for(i=0;i<128;i++)  CpuTemperatureMem.Mem[i] = 0;
}

void cpu_temp_getData()
{

 
	FILE *fp;
	unsigned int temp;
	uint16_t i;
			
	CpuTemperatureMem.cur_value[0] = 0;
	
	fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");		// open as file
	if(fp != NULL)
	{	
		fgets(CpuTemperatureMem.cur_value,32,fp);			// get line
		fclose(fp);
	}
	
	temp = 0;
	if(CpuTemperatureMem.cur_value[0])
	{
		CpuTemperatureMem.cur_value[3]=0;	// end at 1/10 °C
		sscanf(CpuTemperatureMem.cur_value,"%u",&temp);
/*
		printf("%u\r\n",temp);
*/
		CpuTemperatureMem.cur_value[3]=CpuTemperatureMem.cur_value[2];
    CpuTemperatureMem.cur_value[2]='.';
    CpuTemperatureMem.cur_value[4]=0xBA/*'°'*/;
    CpuTemperatureMem.cur_value[5]='C'; 
    CpuTemperatureMem.cur_value[6]=0;
	}
	
	for(i=126;i>0;i--) CpuTemperatureMem.Mem[i+1] = CpuTemperatureMem.Mem[i];
	CpuTemperatureMem.Mem[1] = CpuTemperatureMem.Mem[0];
	CpuTemperatureMem.Mem[0] = temp;	
 
  
}

MEASUREMENT_VIEW_struct * cpu_temp_getViewMem() {
  return &CpuTemperatureMem;
}