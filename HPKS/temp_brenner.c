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
 * File:   temp_brenner.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 29. August 2014, 07:40
 */


#include "defines/used_includes.h"
#include "temp_brenner.h"

static bool is_Init = false;
static MEASUREMENT_VIEW_struct BrennerTemperatureMem;
static int maxBrennerTemp = 0;


void temp_brenner_init()
{
  uint8_t i;

  for (i = 0; i < 128; i++) BrennerTemperatureMem.Mem[i] = 0;

  is_Init = true;
}

void temp_brenner_getData()
{
  if (!is_Init) return;

  uint16_t s_temp;
  uint16_t i_temp;
  uint16_t i;

  if (max31855_read(K_SONDE_BRENNER, &s_temp, &i_temp)) {
    dbg_printf("K_SONDE_BRENNER: %u / %u\n", s_temp, i_temp);
    if (s_temp > 100)lambda_startmeasurment();
    else lambda_stopmeasurment();

    sprintf(BrennerTemperatureMem.cur_value, "%4u", s_temp);
    BrennerTemperatureMem.cur_value[4] = 0xBA/*'°'*/;
    BrennerTemperatureMem.cur_value[5] = 'C';
    BrennerTemperatureMem.cur_value[6] = 0;

    BrennerTemperatureMem.max = s_temp;
    BrennerTemperatureMem.min = s_temp;
    if (maxBrennerTemp < s_temp)
      maxBrennerTemp = s_temp;

    for (i = 126; i > 0; i--) {
      BrennerTemperatureMem.Mem[i + 1] = BrennerTemperatureMem.Mem[i];
      if (BrennerTemperatureMem.Mem[i] > BrennerTemperatureMem.max)
        BrennerTemperatureMem.max = BrennerTemperatureMem.Mem[i];
      if (BrennerTemperatureMem.Mem[i] < BrennerTemperatureMem.min)
        BrennerTemperatureMem.min = BrennerTemperatureMem.Mem[i];
    }
    BrennerTemperatureMem.Mem[1] = BrennerTemperatureMem.Mem[0];
    BrennerTemperatureMem.Mem[0] = s_temp;
  }


}

MEASUREMENT_VIEW_struct * temp_brenner_getViewMem() {
  return &BrennerTemperatureMem;
}

int temp_brenner_last_value() {
  return app_config_is_simulate_mode() ? simulate_get_k_sonde_brenner() : BrennerTemperatureMem.Mem[0];
}

int temp_brenner_max_value() {
  return maxBrennerTemp;
}

void reset_temp_brenner_max() {
  maxBrennerTemp = 0;
}