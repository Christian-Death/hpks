
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
 * File:   regelung.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 02. April 2013, 19:04
 */

#include <stdio.h>
#include <stdlib.h>
#include "defines/used_includes.h"
#include "regelung.h"

typedef enum {
  wait_for_activate, im_soll, changed, nicht_im_soll, ende, wait_for_restart,
} eREGELSTATUS;
static eREGELSTATUS regel_state = wait_for_restart;


static bool regelung_active;
time_t starttime, endtime, akt_time, last_time;
double time_soll, time_nicht_soll;

int regelung_activ(uint8_t event)
{

  regelung_active = (event == EVENT_INP_REGELUNG_ACTIVE) ? true : false;

  if (regelung_active)
  {
    if (regel_state == wait_for_restart)
    {
      regel_state = wait_for_activate;
      reset_shaker();
      reset_pt100_max();
      reset_temp_brenner_max();
      reset_temp_abgas_max();
      reset_lambda_check_max();
      starttime = endtime = 0;
    }
  }
  else if (regel_state != wait_for_restart)
  {
    switch (regel_state)
    {
      case im_soll:
        last_time = akt_time;
        akt_time = time(NULL);
        time_soll += difftime(akt_time, last_time);
        break;
      case nicht_im_soll:
        last_time = akt_time;
        akt_time = time(NULL);
        time_nicht_soll += difftime(akt_time, last_time);
        break;
    }
    endtime = time(NULL);
    stop_shaker(endtime);

    double Q = ((double)app_config_get_max_volume() * app_config_getd_spez_wasser() * (ds18b20_get_durch_temp() - app_config_get_max_puffer_temp())) / (3600.0);

    app_config_save_endstate(endtime, Q);
    regel_state = wait_for_restart;
  }
}

void init_regelung()
{
  set_event_callback(EVENT_INP_REGELUNG_ACTIVE, regelung_activ);
  set_event_callback(EVENT_INP_REGELUNG_DEACTIVE, regelung_activ);
}

bool get_regelung_active()
{
  return regelung_active;
}

void work_regelung()
{
  if ((regelung_active != true)) return; //1s?


  /*
   Wir müssen bei der Ansteuerung des Rüttlers zwei Fälle unterscheiden.

1.       Ausgangsbasis:
Abgastemperatur: größer 120 °C;
Restsauerstoff: über 7 %

Fall 1: (es ist kein holzgasfähiges Holz im Ofen)
-          Rüttelmotor z.B. für 10 sec. starten; (Motor dreht)
-          2 Minuten Pause; (abwarten)
-          Überprüfen, ob Restauerstoff immer noch über 7 % ist, dann erneuter Rüttler-Start;
 
Wenn Rüttler 5 x hintereinander angesteuert wurde und der Restsauerstoff nicht kleiner 7 % geht, dann Rüttler – Stop – da alles Holz ausgebrannt;
Sonst würde der Rüttler ständig weiterlaufen;
Wie starten oder aktivieren wir den Rüttler erneut, bei einem neuen Abbrand, oder wenn man Holz nachgelegt hat?

Fall 2: (es ist noch holzgasfähiges Holz im Ofen – aber es entsteht ein Hohlbrand)

Sobald Restsauerstoff über 7 % geht, dann:  (Hohlbrand)
-          Rüttelmotor z.B. für 10 sec. starten; (Motor dreht)
-          2 Minuten Pause; (abwarten)

Restsauerstoff geht unter 7 % - alles o.k.
   */
  switch (regel_state)
  {
    case wait_for_activate:
      if (lambda_check_last_value() > 1000.0)
      {
        regel_state = im_soll;
        time_soll = time_nicht_soll = 0;
        akt_time = starttime = time(NULL);
      }

      break;
    case im_soll:
      last_time = akt_time;
      akt_time = time(NULL);
      if (lambda_check_last_value() < (app_config_get_ruettler_activ()*1000.0))
      {
        run_shaker(starttime, akt_time);
        regel_state = nicht_im_soll;
        // ausserhalb sollwert
      }

      time_soll += difftime(akt_time, last_time);
      if (/*pt100_last_value() < app_config_get_abgas_end_temp() && */get_current_shaker_count() >= app_config_get_ruettler_loop())
        regel_state = ende;
      break;
    case nicht_im_soll:
      last_time = akt_time;
      akt_time = time(NULL);
      if (lambda_check_last_value() > (app_config_get_ruettler_deactiv()*1000.0))
      {
        stop_shaker(akt_time);
        regel_state = im_soll;
        // innerhalb sollwert
      }
      time_nicht_soll += difftime(akt_time, last_time);
      if (/*pt100_last_value() < app_config_get_abgas_end_temp() &&*/get_current_shaker_count() >= app_config_get_ruettler_loop())
        regel_state = ende;
      break;
    case ende:
      /*
      time(&ist_nicht_soll);
      time_nicht_soll += difftime(ist_nicht_soll, ist_soll);
       */
      endtime = time(NULL);
      stop_shaker(endtime);
      regel_state = wait_for_restart;
      break;

  }



}

double get_ist_soll()
{
  double ddummy = .0;
  time_t _time;
  if (regel_state == im_soll)
  {
    _time = time(NULL);
    ddummy = difftime(_time, akt_time);
    /*
        time(&ist_nicht_soll);
        ddummy = difftime(ist_nicht_soll, ist_soll);
     */
  }
  return time_soll + ddummy;
}

double get_ist_nicht_soll()
{
  double ddummy = .0;
  time_t _time;
  if (regel_state == nicht_im_soll)
  {
    _time = time(NULL);
    ddummy = difftime(_time, akt_time);
    /*
        time(&ist_soll);
        ddummy = difftime(ist_soll, ist_nicht_soll);
     */
  }
  return time_nicht_soll + ddummy;
}

bool get_control_armed()
{
  return (regel_state == im_soll || regel_state == nicht_im_soll);
}

time_t get_starttime()
{
  return starttime;
}

time_t get_endtime()
{
  return endtime;
}