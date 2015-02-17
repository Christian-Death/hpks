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
 * File:   simulate.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 17. Dezember 2012, 14:25
 */

#include <stdio.h>
#include <stdlib.h>
#include "defines/used_includes.h"

#include "../utils/minini/minIni.h"

#include "simulate.h"




static char sim_inifile[PATH_MAX];

/*!***************************************************************************
 * 
 * 
 *****************************************************************************/
int simulate_init() {
  char str[100];


  struct stat buf;

  snprintf(sim_inifile, PATH_MAX, "%s/%s", app_fullpath, SIM_FILE_NAME);
  if ((0 == stat(sim_inifile, &buf)) && (S_ISREG(buf.st_mode))) {
    dbg_printf("Konfigfile %s gefunden\n", sim_inifile);
  } else {
    dbg_printf("Konfigfile %s nicht gefunden\n", sim_inifile);
  }
}

int simulate_get_lambda() {
  long n;
  n = ini_getl("", "lambdacheck", 0, sim_inifile);
  return n;
}

int simulate_get_pt100() {
  long n;
  n = ini_getl("", "pt100", 0, sim_inifile);
  return n;
}

int simulate_get_k_sonde_abgas() {
  long n;
  n = ini_getl("", "temp_abgas", 0, sim_inifile);
  return n;
}

int simulate_get_k_sonde_brenner() {
  long n;
  n = ini_getl("", "temp_brenner", 0, sim_inifile);
  return n;
}

int simulate_get_durch_temp() {
  long n;
  n = ini_getl("", "durch_puffer_temp", 30, sim_inifile);
  return n;
}
