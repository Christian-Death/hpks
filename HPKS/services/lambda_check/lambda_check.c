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
 * File:   lambda_check.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 2. Dezember 2012, 11:52
 */


#include <defines/used_includes.h>
#include "lambda_check.h"

#define DEFAULT_VOLTAGE_MV (3300.0)
#define SPANNUNGSTEILER (24000.0/(24000.0+56000.0))

MEASUREMENT_VIEW_struct LambdaCheckMem;
static bool is_Init = false;
static double max_lambdacheck = .0;

void lambda_check_init(void) {
  uint8_t i;

  for (i = 0; i < 128; i++) LambdaCheckMem.Mem[i] = 0;

  is_Init = true;
}


void lambda_check_getData() {
  if (!is_Init) return;

  uint16_t adc;
  uint16_t i;
  int retval;

  retval = mcp3008_adc_read(MCP3008_ADC_SINGLE, LAMBDA_CHECK_ADC, &adc);

  if (retval == EXIT_SUCCESS) {
    adc = ((DEFAULT_VOLTAGE_MV / 1024.0)*(double) adc) / SPANNUNGSTEILER;
    sprintf(LambdaCheckMem.cur_value, "%4u", adc);

    LambdaCheckMem.cur_value[4] = 0xBA/*'°'*/;
    LambdaCheckMem.cur_value[5] = 'C';
    LambdaCheckMem.cur_value[6] = 0;

    LambdaCheckMem.max = adc;
    LambdaCheckMem.min = adc;
    if (max_lambdacheck < adc)
      max_lambdacheck = adc;

    for (i = 126; i > 0; i--) {
      LambdaCheckMem.Mem[i + 1] = LambdaCheckMem.Mem[i];
      if (LambdaCheckMem.Mem[i] > LambdaCheckMem.max)
        LambdaCheckMem.max = LambdaCheckMem.Mem[i];
      if (LambdaCheckMem.Mem[i] < LambdaCheckMem.min)
        LambdaCheckMem.min = LambdaCheckMem.Mem[i];
    }
    LambdaCheckMem.Mem[1] = LambdaCheckMem.Mem[0];
    LambdaCheckMem.Mem[0] = adc;
  }


}

MEASUREMENT_VIEW_struct * lambda_check_getViewMem() {
  return &LambdaCheckMem;
}

double lambda_check_last_value() {

  return app_config_is_simulate_mode() ? simulate_get_lambda() : LambdaCheckMem.Mem[0];
}

double lambda_check_max_value() {
  return max_lambdacheck;
}

void reset_lambda_check_max() {
  max_lambdacheck = 0;
}