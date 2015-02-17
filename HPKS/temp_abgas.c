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
 * File:   temp_abgas.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 29. August 2014, 08:40
 */


#include "defines/used_includes.h"
#include "temp_abgas.h"

static bool is_Init = false;
static MEASUREMENT_VIEW_struct AbgasTemperatureMem;
static int maxAbgasTemp = 0;


void temp_abgas_init()
{
  uint8_t i;

  for (i = 0; i < 128; i++) AbgasTemperatureMem.Mem[i] = 0;

  is_Init = true;
}

void temp_abgas_getData()
{
  if (!is_Init) return;

  uint16_t s_temp;
  uint16_t i_temp;
  uint16_t i;

  if (max31855_read(K_SONDE_ABGAS, &s_temp, &i_temp)) {
    dbg_printf("K_SONDE_ABGAS: %u / %u\n", s_temp, i_temp);
    if (s_temp > 100)lambda_startmeasurment();
    else lambda_stopmeasurment();

    sprintf(AbgasTemperatureMem.cur_value, "%4u", s_temp);
    AbgasTemperatureMem.cur_value[4] = 0xBA/*'°'*/;
    AbgasTemperatureMem.cur_value[5] = 'C';
    AbgasTemperatureMem.cur_value[6] = 0;

    AbgasTemperatureMem.max = s_temp;
    AbgasTemperatureMem.min = s_temp;
    if (maxAbgasTemp < s_temp)
      maxAbgasTemp = s_temp;

    for (i = 126; i > 0; i--) {
      AbgasTemperatureMem.Mem[i + 1] = AbgasTemperatureMem.Mem[i];
      if (AbgasTemperatureMem.Mem[i] > AbgasTemperatureMem.max)
        AbgasTemperatureMem.max = AbgasTemperatureMem.Mem[i];
      if (AbgasTemperatureMem.Mem[i] < AbgasTemperatureMem.min)
        AbgasTemperatureMem.min = AbgasTemperatureMem.Mem[i];
    }
    AbgasTemperatureMem.Mem[1] = AbgasTemperatureMem.Mem[0];
    AbgasTemperatureMem.Mem[0] = s_temp;
  }


}

MEASUREMENT_VIEW_struct * temp_abgas_getViewMem() {
  return &AbgasTemperatureMem;
}

int temp_abgas_last_value() {
  return app_config_is_simulate_mode() ? simulate_get_k_sonde_abgas() : AbgasTemperatureMem.Mem[0];
}

int temp_abgas_max_value() {
  return maxAbgasTemp;
}

void reset_temp_abgas_max() {
  maxAbgasTemp = 0;
}