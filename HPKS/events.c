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
 * File:   events.c
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 02. April 2013, 19:04
 */

#include <stdio.h>
#include <stdlib.h>
#include "defines/used_includes.h"
#include "events.h"

//################## DEFINES ###########################################
//#

/**
 *	definiert die symbolische Konstante \a EVENT_BUF_MASK welche 
 *	die Maske für den Lese- und Schreibcounter des Eventbuffers enthält.
 */
#define EVENT_BUF_MASK 0x0F

/**
 *	definiert die symbolische Konstante \a EVENT_BUF_DIM welche 
 *	angibt wie groß das Array für den Eventbuffer ist.
 */
#define EVENT_BUF_DIM EVENT_BUF_MASK+1 

#define TIMEOUT_30ms 3

//################## STRUKTUREN ########################################
//#

/**
 *	Struktur stellt den Eventbuffer dar.
 */
typedef struct {
  //! 
  uint8_t on_time;
  //! 
  uint8_t on_event;
  //! 
  uint8_t off_time;
  //! 
  uint8_t off_event;
} t_input_event;

typedef struct {
  event_callback_t func;
} t_event_callbacks;

typedef struct {
  //! 
  uint8_t off;
  //! 
  uint8_t on;

} t_input_count;

/**
 *	Struktur stellt den Eventbuffer dar.
 */
typedef struct {
  //! Array für die Events.
  uint8_t buf[EVENT_BUF_DIM];
  //! Lesezeiger, enthält den Index auf das Feld von dem als nächstes das Event gelesen wird.
  uint8_t buf_read;
  //! Schreibzeiger, enthält den Index auf das nächste Feld wo ein Event geschrieben wird.
  uint8_t buf_write;
} t_events;

/** 
 *	Enum für die einzelnen Tasten und Schliesser.
 */
enum TEnumInputKeys {
  INP_REGELUNG_ACTIVE,
  INP_SHAKER_1,
  INP_SHAKER_2,
  INP_3,

  KEY_DIM //!< Enum value enthält die Anzahl der Enumwerte
};

//################## KONSTANTEN ########################################
//#
/**
 *	Konstantes Array, mit den Werten der Zeiten, wie lange ein Eingang OFF sein muss
 *	damit er sein Ereigniss auslöst.
 */
static const t_input_event input_events[KEY_DIM] = {
  {TIMEOUT_30ms, EVENT_INP_REGELUNG_ACTIVE, TIMEOUT_30ms, EVENT_INP_REGELUNG_DEACTIVE},
  {TIMEOUT_30ms, EVENT_INP_SHAKER_1_ON, TIMEOUT_30ms, EVENT_INP_SHAKER_1_OFF},
  {TIMEOUT_30ms, EVENT_INP_SHAKER_2_ON, TIMEOUT_30ms, EVENT_INP_SHAKER_2_OFF},
  {TIMEOUT_30ms, EVENT_INP_3_ON, TIMEOUT_30ms, EVENT_INP_3_ON},
};

//################## VARIABLEN #########################################
//#
static t_event_callbacks event_callback[EVENT_DIM];

static t_input_count inputs_count[KEY_DIM];
//! Der Eventbuffer.
static t_events events;



/***********************************************************************//*!
 *
 *	\brief	Initialisieren der Ereignisverarbeitung.
 *
 *	\author Christian Rost
 *	\date   17.11.2009
 *
 **************************************************************************/

void init_events(void)
{
  event_start_conditions();
}

/***********************************************************************//*!
 *
 *	\brief	setzt die Variablen auf die Grundeinstellungen.
 *
 *	\author Christian Rost
 *	\date   21.01.2010
 *
 **************************************************************************/

void event_start_conditions(void)
{
  t_input_event dummy;
  uint8_t i;

  events.buf_read = 0;
  events.buf_write = 0;

  for (i = 0; i < KEY_DIM; i++)
  {
    memcpy(&dummy, &input_events[i], sizeof (t_input_event));
    inputs_count[i].off = 0; //dummy.off_time;
    inputs_count[i].on = 0; //dummy.on_time;;

  }

}

uint8_t read_inputs()
{
  uint8_t ret = 0x00;
  ret |= bcm2835_gpio_lev(INP1) ? (0x01 << 0) : 0x00;
  ret |= bcm2835_gpio_lev(INP2) ? (0x01 << 1) : 0x00;
  ret |= bcm2835_gpio_lev(INP3) ? (0x01 << 2) : 0x00;
  ret |= bcm2835_gpio_lev(INP4) ? (0x01 << 3) : 0x00;
  return ret;
}
/***********************************************************************//*!
 *
 *	\brief	Routine scant die Eingänge.
 *
 *			Nachdem die Eingänge gescant wurden wird überprüft ob sich
 *			der Zustand über eine gewisse Zeit geändert hat, wenn ja
 *			wird das zum Eingang dazugehörige Event ausgelöst.
 *
 *	\author Christian Rost   
 *	\date   17.11.2009
 *
 **************************************************************************/

void scan_inputs(void)
{
  t_input_event dummy;

  uint16_t value;
  uint8_t i;

  value = 0;

#ifdef USE_PORTEXPANDER
  uint8_t input = mcp23s17_read(PORT_A);
  if ((input & (0x01 << 2))) value |= (0x01 << INP_REGELUNG_ACTIVE);
#else
  value |= bcm2835_gpio_lev(INP1) ? 0x00 : (0x01 << INP_REGELUNG_ACTIVE);
  value |= bcm2835_gpio_lev(INP2) ? 0x00 : (0x01 << INP_SHAKER_1);
  value |= bcm2835_gpio_lev(INP3) ? 0x00 : (0x01 << INP_SHAKER_2);
  value |= bcm2835_gpio_lev(INP4) ? 0x00 : (0x01 << INP_3);
#endif


  for (i = 0; i < KEY_DIM; i++)
  {
    memcpy(&dummy, &input_events[i], sizeof (t_input_event));

    if (!(value & (0x0001 << i))) // Eingang aus
    {
      if (inputs_count[i].off < dummy.off_time)
      {
        inputs_count[i].off++;
        if (inputs_count[i].off == dummy.off_time)
        {
          inputs_count[i].on = 0;

          switch (i)
          {

            default:
              event_store(dummy.off_event);
              break;
          }

        }
      }
      else
      {
        inputs_count[i].on = 0;
      }
    }
    else // Eingang ein
    {
      if (inputs_count[i].on < dummy.on_time)
      {
        inputs_count[i].on++;
        if (inputs_count[i].on == dummy.on_time)
        {
          inputs_count[i].off = 0;

          switch (i)
          {
            default:
              break;
          }
          event_store(dummy.on_event);

        }
      }
      else
      {
        inputs_count[i].off = 0;
      }
    }
  }

}

/***********************************************************************//*!
 *
 *	\brief	Speichert ein Ereignis im Ringpuffer und erhöht den
 *			Schreibzeiger.
 *
 *	\author Christian Rost
 *	\date   17.11.2009
 *
 * \param event Ist das Ereignis was gespeichert werden soll.
 *
 **************************************************************************/

void event_store(uint8_t event)
{
  events.buf[events.buf_write] = event;
  events.buf_write++;
  events.buf_write &= EVENT_BUF_MASK;
}

/***********************************************************************//*!
 *
 *	\brief	Liest ein Ereignis aus dem Ringpuffer und erhöht den
 *			Lesezeiger.
 *
 *	\author Christian Rost
 *	\date   17.11.2009
 *
 *	\return	Das Ereignis welches an der Stelle das Lesezeigers steht bevor 
 *			er erhöht wird.
 *
 **************************************************************************/

static uint8_t event_read(void)
{
  uint8_t event;

  if (events.buf_read == events.buf_write)
    return 0;

  event = events.buf[events.buf_read];
  events.buf_read++;
  events.buf_read &= EVENT_BUF_MASK;

  return event;
}

/***********************************************************************//*!
 *
 *	\brief	Prüft, ob noch Ereignisse abgearbeitet werden müssen 
 *
 *	\author Christian Rost
 *	\date   17.11.2009
 *
 *	\return TRUE wenn noch Ereignis verfügbar sind.
 *			FALSE wenn der Ringpuffer leer ist.
 *
 **************************************************************************/

static uint8_t event_available(void)
{
  if (events.buf_read == events.buf_write)
    return FALSE;

  return TRUE;
}

/***********************************************************************//*!
 *
 *	\brief	Verarbeiten der Ereignisse.
 *
 *	\author Christian Rost
 *	\date   17.11.2009
 *
 **************************************************************************/

void event_work(void)
{
  uint8_t event;

  while (event_available())
  {
    event = event_read();
    if (event_callback[event].func)
      event_callback[event].func(event);
  }
}

void set_event_callback(uint8_t event, event_callback_t func)
{
  event_callback[event].func = func;
}