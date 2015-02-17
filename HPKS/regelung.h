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
 * File:   regelung.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 2. April 2013, 21:00
 */

#ifndef REGELUNG_H
#define	REGELUNG_H

#ifdef	__cplusplus
extern "C" {
#endif

  void init_regelung();
  int regelung_activ(uint8_t event);
  void work_regelung();
  bool get_regelung_active();
  bool get_ruettler_state();
  double get_ist_soll();
  double get_ist_nicht_soll();
  time_t get_starttime();
  time_t get_endtime();
  bool get_control_armed();
#ifdef	__cplusplus
}
#endif

#endif	/* REGELUNG_H */

