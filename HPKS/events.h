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
 * File:   events.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 24. Oktober 2013, 10:14
 */

#ifndef EVENTS_H
#define	EVENTS_H

#ifdef	__cplusplus
extern "C" {
#endif

    /*! 
     *	Enum für die einzelnen Events welche ausgelöst werden können.
     */
    enum TEnumEvents {
        EVENT_INIT0, //! Initialisierungsevent 0
        EVENT_INP_REGELUNG_ACTIVE,
        EVENT_INP_REGELUNG_DEACTIVE,
        EVENT_INP_SHAKER_1_ON,
        EVENT_INP_SHAKER_1_OFF,
        EVENT_INP_SHAKER_2_ON,
        EVENT_INP_SHAKER_2_OFF,
        EVENT_INP_3_ON,
        EVENT_INP_3_OFF,

        //! Enum value enthält die Anzahl der Enumwerte
        EVENT_DIM
    };

    typedef int (*event_callback_t) (uint8_t);

    void set_event_callback(uint8_t event, event_callback_t func);
    void init_events(void);
    void event_start_conditions(void);
    void scan_inputs(void);


    void event_store(uint8_t event);
    void event_work(void);


#ifdef	__cplusplus
}
#endif

#endif	/* EVENTS_H */

