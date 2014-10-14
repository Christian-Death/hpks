
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
 * File:   shaker.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 02. April 2013, 19:04
 */

#include <stdio.h>
#include <stdlib.h>
#include "defines/used_includes.h"
#include "shaker.h"

#define time_diff_sec ((cur_ms - reuttler_change_sec)/1000) 
static bool ruettler_on = OFF;
static unsigned int last_ms;
static unsigned int reuttler_time_sec;

static unsigned int reuttler_change_sec;
static unsigned int reuttler_change_count;
static unsigned int shaker_run_count = 0;
static unsigned int shaker_shake_count = 0;

typedef enum {
  SHAKER_IDLE,
  SHAKER_RUN,
  SHAKER_RUNNING_ON,
  SHAKER_RUNNING_OFF,
} eSHAKER_STATE;

typedef enum {
  SHAKER_NUM1 = 1,
  SHAKER_NUM2,
  SHAKER_NUM1_NUM2,
  SHAKER_DIM
} eSHAKERS;

static eSHAKER_STATE shaker_state = SHAKER_IDLE;
static eSHAKERS shakers = 0;
static time_t shaker_block_time;
static uint8_t shaker_block;
/*********************************************************************/


struct shake_info *cur_shake_info = NULL;
struct shake_info *anfang = NULL;

/* Wir hängen einen Datensatz an oder geben einen neuen ein:
 * n=name,v=vornam,at=alter.tage,am=alter.monat,aj=alter.jahr
 * eint=eigestellt tag,einm=eingestellt monat,einj=eingest.
 * Jahr  g=gehalt */

void anhaengen(time_t start)
{
  /* Zeiger zum Zugriff auf die einzelnen Elemente
   * der Struktur*/
  struct shake_info *zeiger;

  /* Wir fragen ab, ob es schon ein Element in der Liste
   * gibt. Wir suchen das Element, auf das unser Zeiger
   *  *anfang zeigt. Falls *anfang immer noch auf NULL zeigt,
   *  bekommt *anfang die Adresse unseres 1. Elements und ist
   *  somit der Kopf (Anfang) unserer Liste. */
  if (anfang == NULL)
  {
    /* Wir reservieren Speicherplatz für unsere Struktur
     * für das erste Element der Liste. */
    if ((anfang =
        malloc(sizeof (struct shake_info))) == NULL)
    {
      fprintf(stderr, "Kein Speicherplatz vorhanden "
              "fuer anfang\n");
      return;
    }

    zeiger = anfang; /* zeiger auf neuen Speicherplatz */
  }/* Es scheint schon mindestens ein Element in der Liste
    * vorhanden zu sein, da der Anfang nicht == NULL ist.
    * Jetzt suchen wir so lange nach dem nächsten Element,
    * bis der *next-Zeiger auf NULL zeigt. Somit haben wir
    * das Ende der Liste gefunden und können einen neuen
    * Datensatz anhängen. */
  else
  {
    zeiger = anfang; /* Wir zeigen auf das 1. Element. */
    while (zeiger->next != NULL)
      zeiger = zeiger->next;
    /* Wir reservieren einen Speicherplatz für das letzte
     * Element der Liste und hängen es an. */
    if ((zeiger->next =
        malloc(sizeof (struct shake_info))) == NULL)
    {
      fprintf(stderr, "Kein Speicherplatz fuer das "
              "letzte Element\n");
      return;
    }
    zeiger = zeiger->next; /* zeiger auf neuen Speicherplatz */

  }

  zeiger->begin = start;
  zeiger->count = 0;
  zeiger->dauer = 0;

  /* Somit haben wir unseren Anfang der Liste. Von nun an
   * zeigt der Zeiger anfang immer auf das Element vor ihm.
   * Da dies aber jetzt das 1. Element der Liste war, zeigt
   * der Zeiger anfang auf den Zeiger next. next zeigt am
   * Ende immer wieder  NULL. */
  zeiger->next = NULL;
  cur_shake_info = zeiger;
}

void clear_shake_list(void)
{
  struct shake_info *zeiger, *zeiger1;

  /* Ist überhaupt eine Liste zum Löschen vorhanden? */
  if (anfang != NULL)
  {
    /* Es ist eine vorhanden. */
    zeiger = anfang->next;
    while (zeiger != NULL)
    {
      zeiger1 = anfang->next->next;
      anfang->next = zeiger1;
      free(zeiger);
      zeiger = zeiger1;
    }
    /* Jetzt löschen wir erst den Anfang der Liste. */
    free(anfang->next);
    free(anfang);
    cur_shake_info = anfang = NULL;

    printf("Liste erfolgreich geloescht!!\n");
  }
  else
    fprintf(stderr, "Keine Liste zum Loeschen vorhanden!!\n");
}

/**************************************************************/
static void set_shaker_on(int nr)
{
  
  
  if(shaker_block)  {
    return;
  }
  else  {
    time_t akt_time = time(NULL);
    if(difftime(akt_time, shaker_block_time) < DEF_SHAKER_BLOCK_TIME_SEC )
      return;
  }
  
  shaker_block |= nr;
  
  switch (nr)
  {
    case SHAKER_NUM1:
      
#ifdef USE_PORTEXPANDER  
      mcp23s17_set_pin(PORT_A, PIN0);
#else
      bcm2835_gpio_set(SHAKER1);
#endif
      break;
    case SHAKER_NUM2:
#ifdef USE_PORTEXPANDER  
      mcp23s17_set_pin(PORT_A, PIN1);
#else
      bcm2835_gpio_set(SHAKER2);
#endif
      break;
    case SHAKER_NUM1_NUM2:
#ifdef USE_PORTEXPANDER  
      mcp23s17_set_pin(PORT_A, PIN0);
      mcp23s17_set_pin(PORT_A, PIN1);
#else
      bcm2835_gpio_set(SHAKER1);
      bcm2835_gpio_set(SHAKER2);
#endif
      break;
  }

}

static void set_shaker_off(int nr)
{
  if(shaker_block)
    shaker_block_time = time(NULL);
  
  shaker_block &= ~nr;
  switch (nr)
  {
    case SHAKER_NUM1:
#ifdef USE_PORTEXPANDER  
      mcp23s17_clr_pin(PORT_A, PIN0);
#else
      bcm2835_gpio_clr(SHAKER1);
#endif
      break;
    case SHAKER_NUM2:
#ifdef USE_PORTEXPANDER  
      mcp23s17_clr_pin(PORT_A, PIN1);
#else
      bcm2835_gpio_clr(SHAKER2);
#endif
      break;
    case SHAKER_NUM1_NUM2:
#ifdef USE_PORTEXPANDER  
      mcp23s17_clr_pin(PORT_A, PIN0);
      mcp23s17_clr_pin(PORT_A, PIN1);
#else
      bcm2835_gpio_clr(SHAKER1);
      bcm2835_gpio_clr(SHAKER2);
#endif      
      break;
  }

}

void run_shaker(time_t start_time, time_t akt_time)
{

  shaker_state = SHAKER_RUN;
  anhaengen(akt_time);
}

void stop_shaker(time_t akt_time)
{
  if (cur_shake_info != NULL)
  {
    cur_shake_info->count = reuttler_change_count;
    cur_shake_info->dauer = akt_time;
  }
  set_shaker_off(SHAKER_NUM1_NUM2);
  shaker_state = SHAKER_IDLE;
  ruettler_on = OFF;
}

bool get_shaker_state()
{
  return ruettler_on;
}

int get_current_shaker_count()
{
  return reuttler_change_count;
}

int get_shaker_shake_count()
{
  return shaker_shake_count;
}

int get_shaker_run_count()
{
  return shaker_run_count;
}

void reset_shaker()
{
  stop_shaker(0);
  shaker_run_count = 0;
  shaker_shake_count = 0;
  reuttler_change_count = 0;
  clear_shake_list();
}

int event_callback_shaker(uint8_t event)
{
  switch(event)
  {
    case EVENT_INP_SHAKER_1_ON:
      set_shaker_on(SHAKER_NUM1);
      break;
    case EVENT_INP_SHAKER_1_OFF:
      set_shaker_off(SHAKER_NUM1);
      break;
    case EVENT_INP_SHAKER_2_ON:
      set_shaker_on(SHAKER_NUM2);
      break;
    case EVENT_INP_SHAKER_2_OFF:
      set_shaker_off(SHAKER_NUM2);
      break;
  }
}

void init_shaker()
{
  set_event_callback(EVENT_INP_SHAKER_1_ON, event_callback_shaker);
  set_event_callback(EVENT_INP_SHAKER_1_OFF, event_callback_shaker);
  set_event_callback(EVENT_INP_SHAKER_2_ON, event_callback_shaker);
  set_event_callback(EVENT_INP_SHAKER_2_OFF, event_callback_shaker);
}

void shaker_work()
{

  unsigned int cur_ms = millis();
  

  switch (shaker_state)
  {
    case SHAKER_IDLE:
      break;
    case SHAKER_RUN:
      shaker_run_count++;
      reuttler_time_sec = cur_ms;
      reuttler_change_sec = cur_ms;
      reuttler_change_count = 0;
      shaker_state = SHAKER_RUNNING_ON;
      ruettler_on = ON;
      shakers = SHAKER_NUM1;
      set_shaker_on(shakers);
      break;
    case SHAKER_RUNNING_ON:
      if (time_diff_sec >= app_config_get_ruettler_on_time())
      {
        set_shaker_off(shakers);
        if (++shakers >= SHAKER_NUM1_NUM2/*SHAKER_DIM*/)
        {
          shakers = SHAKER_NUM1;
          /*if (++reuttler_change_count >= app_config_get_ruettler_loop()) {
            shaker_state = SHAKER_IDLE;
            ruettler_on = OFF;
            return;
          }*/
        }
        shaker_shake_count++;
        cur_shake_info->count++;
        if (!app_config_get_shake_alltimes() && (++reuttler_change_count >= app_config_get_ruettler_loop()))
        {
          shaker_state = SHAKER_IDLE;
          ruettler_on = OFF;
          return;
        }
        shaker_state = SHAKER_RUNNING_OFF;
        reuttler_change_sec = cur_ms;

      }


      break;
    case SHAKER_RUNNING_OFF:
      if (time_diff_sec >= app_config_get_ruettler_off_time())
      {
        set_shaker_on(shakers);
        shaker_state = SHAKER_RUNNING_ON;
        reuttler_change_sec = cur_ms;
      }


      break;
  }
}

struct shake_info* get_shake_info(struct shake_info* sinfo)
{
  if (sinfo == NULL)
  {
    return anfang;
  }
  else
  {
    return sinfo->next;
  }
}