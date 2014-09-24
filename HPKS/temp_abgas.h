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
 * File:   temp_abgas.h
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 29. August 2014, 07:42
 */

#ifndef TEMP_ABGAS_H
#define	TEMP_ABGAS_H

#ifdef	__cplusplus
extern "C" {
#endif

  void temp_abgas_init(void);
  void temp_abgas_getData();
  MEASUREMENT_VIEW_struct * temp_abgas_getViewMem();
  int temp_abgas_last_value();
  int temp_abgas_max_value();
  void reset_temp_abgas_max();
  
#ifdef	__cplusplus
}
#endif

#endif	/* TEMP_ABGAS_H */

