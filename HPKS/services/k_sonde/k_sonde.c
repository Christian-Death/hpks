

/* 
 * File:   ds18b20.c
 * Author: The Death
 *
 * Created on 2. Dezember 2012, 11:52
 */


#include <defines/used_includes.h>
#include "k_sonde.h"

static bool is_Init = false;
MEASUREMENT_VIEW_struct BrennerTemperatureMem;
static int maxBrennerTemp = 0;
static unsigned int last_ms;

void k_sonde_init(void) {
  uint8_t i;

  for (i = 0; i < 128; i++) BrennerTemperatureMem.Mem[i] = 0;

  is_Init = true;
}



void k_sonde_getData() {
  if (!is_Init) return;

  uint16_t s_temp;
  uint16_t i_temp;
  uint16_t i;

  unsigned int cur_ms = millis();
  if ((cur_ms - last_ms) < 1000) return;

  last_ms = cur_ms;

  if (max31855_read(1, &s_temp, &i_temp)) {
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

MEASUREMENT_VIEW_struct * k_sonde_getViewMem() {
  return &BrennerTemperatureMem;
}

int k_sonde_last_value() {
  return BrennerTemperatureMem.Mem[0];
}

int k_sonde_max_value() {
  return maxBrennerTemp;
}

void reset_k_sonde_max() {
  maxBrennerTemp = 0;
}