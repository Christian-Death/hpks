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
 * File:   pt100.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 2. Dezember 2012, 11:52
 */


#include <defines/used_includes.h>
#include "pt100.h"

static bool is_Init = false;
#define DEFAULT_VOLTAGE_MV (3300.0)
#define SPANNUNGSTEILER (24000.0/(24000.0+56000.0))

#define MAX_TEMP_BEREICH 400.0
#define MAX_TEMP_VOLTAGE 10000.0

MEASUREMENT_VIEW_struct AbgasTemperatureMem;
static double abgasverlust;
static double maxAbgasTemperature = .0;

void pt100_init(void) {
  uint8_t i;

  for (i = 0; i < 128; i++) AbgasTemperatureMem.Mem[i] = 0;

  is_Init = true;
}


void pt100_getData() {
  if (!is_Init) return;

  uint16_t adc;
  double abgas_temp;
  uint16_t i;
  int retval;


  retval = mcp3008_adc_read(MCP3008_ADC_SINGLE, PT100_ADC, &adc);

  if (retval == EXIT_SUCCESS) {

    adc = ((DEFAULT_VOLTAGE_MV / 1024.0)*(double) adc) / SPANNUNGSTEILER;
    abgas_temp = MAX_TEMP_BEREICH * ((double) adc / MAX_TEMP_VOLTAGE);
    double lambda;
    if (get_lastO2Value(&lambda) == EXIT_SUCCESS) {
      abgasverlust = (adc - 20.0)*(0.6842 / (21.0 - lambda) + 0.0125);
    }

    sprintf(AbgasTemperatureMem.cur_value, "%5.1f", abgas_temp);
    AbgasTemperatureMem.cur_value[5] = 0xBA/*'°'*/;
    AbgasTemperatureMem.cur_value[6] = 'C';
    AbgasTemperatureMem.cur_value[7] = 0;


    AbgasTemperatureMem.max = abgas_temp;
    AbgasTemperatureMem.min = abgas_temp;
    if (maxAbgasTemperature < abgas_temp)
      maxAbgasTemperature = abgas_temp;

    for (i = 126; i > 0; i--) {
      AbgasTemperatureMem.Mem[i + 1] = AbgasTemperatureMem.Mem[i];
      if (AbgasTemperatureMem.Mem[i] > AbgasTemperatureMem.max)
        AbgasTemperatureMem.max = AbgasTemperatureMem.Mem[i];
      if (AbgasTemperatureMem.Mem[i] < AbgasTemperatureMem.min)
        AbgasTemperatureMem.min = AbgasTemperatureMem.Mem[i];
    }
    AbgasTemperatureMem.Mem[1] = AbgasTemperatureMem.Mem[0];
    AbgasTemperatureMem.Mem[0] = abgas_temp;
  }


}

MEASUREMENT_VIEW_struct * pt100_getViewMem() {
  return &AbgasTemperatureMem;
}

double get_abgasverlust() {
  return abgasverlust;
}

double pt100_last_value() {
  return app_config_is_simulate_mode() ? simulate_get_pt100() : AbgasTemperatureMem.Mem[0];
}

double pt100_max_value() {
  return maxAbgasTemperature;
}

void reset_pt100_max() {
  maxAbgasTemperature = 0;
}